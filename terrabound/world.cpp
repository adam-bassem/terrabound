/*
 * ============================================================================
 * File        : world.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     World code
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#include "world.hpp"
#include "chunk.hpp"
#include "raylib.h"
#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "raymath.h"

namespace
{
	std::map<std::pair<int, int>, Chunk*> chunks;
	std::set<std::pair<int, int>> to_remesh;
	uint64_t seed;

	// Chunk offsets inside the render circle, sorted nearest first. Built once per radius.
	std::vector<std::pair<int, int>> load_order;
	int load_order_radius = -1;

	void BuildLoadOrder(int r)
	{
		load_order.clear();
		for (int x = -r; x <= r; x++)
		{
			for (int z = -r; z <= r; z++)
			{
				if (x * x + z * z <= r * r) load_order.push_back({ x, z });
			}
		}

		std::sort(load_order.begin(), load_order.end(), [](const auto& a, const auto& b)
		{
			return a.first * a.first + a.second * a.second < b.first * b.first + b.second * b.second;
		});

		load_order_radius = r;
	}

	// Creates the chunk at (x, z) if it doesn't exist yet. Returns true if one was created.
	bool ProcessChunk(int x, int z)
	{
		if (chunks.count({ x, z })) return false;

		Vector3 block_origin = { (float)(x * subchunk_size), 0, (float)(z * subchunk_size) };
		Chunk* c = new Chunk(block_origin, seed);
		chunks[{ x, z }] = c;

		constexpr int dx[4] = { 0, 0, 1, -1 };
		constexpr int dz[4] = { 1, -1, 0, 0 };

		for (int i = 0; i < 4; i++)
		{
			auto it = chunks.find({ x + dx[i], z + dz[i] });
			if (it == chunks.end() || !it->second) continue;

			c->SetNeighbour(dx[i], dz[i], it->second);
			it->second->SetNeighbour(-dx[i], -dz[i], c);
			to_remesh.insert({ x + dx[i], z + dz[i] });
		}
		to_remesh.insert({ x, z });
		return true;
		//delete[] c;
	}
}

void World::GenerateWorld()
{
	seed = (uint64_t)GetRandomValue(0, 0x7FFFFFFF);
}

void World::Render(PlayerCamera& camera)
{
	Vector3 position = camera.GetPosition();
	int chunk_x = (int)floorf(position.x / subchunk_size);
	int chunk_z = (int)floorf(position.z / subchunk_size);
	int r = camera.GetRenderDistanceChunks();

	// Index matches Chunk::neighbours, 0 N (+z), 1 S (-z), 2 E (+x), 3 W (-x)
	constexpr int dx[4] = { 0, 0, 1, -1 };
	constexpr int dz[4] = { 1, -1, 0, 0 };

	constexpr int max_new_chunks = 2;
	constexpr int max_remesh = 4;

	if (r != load_order_radius) BuildLoadOrder(r);

	// Pass 1: create and link new chunks, nearest first, within budget
	int created = 0;
	for (const auto& offset : load_order)
	{
		if (created >= max_new_chunks) break;
		if (ProcessChunk(chunk_x + offset.first, chunk_z + offset.second)) created++;
	}

	// Pass 2: decorate chunks within 75% of render distance, then remesh dirty chunks
	const float decorate_r = r * 0.75f;
	const float decorate_sq = decorate_r * decorate_r;

	for (const auto& offset : load_order)
	{
		if (offset.first * offset.first + offset.second * offset.second > decorate_sq) continue;

		const int x = chunk_x + offset.first;
		const int z = chunk_z + offset.second;
		auto found = chunks.find({ x, z });
		if (found == chunks.end() || !found->second) continue;
		if (found->second->decorated) continue;

		found->second->DecorateSecondPass();
		to_remesh.insert({ x, z });
	}

	int remeshed = 0;
	for (auto it = to_remesh.begin(); it != to_remesh.end() && remeshed < max_remesh; )
	{
		auto found = chunks.find(*it);
		if (found != chunks.end() && found->second)
		{
			found->second->GenerateMesh();
			found->second->GenerateModel();
			remeshed++;
		}
		it = to_remesh.erase(it);
	}

	// Pass 3: draw
	for (const auto& offset : load_order)
	{
		const int x = chunk_x + offset.first;
		const int z = chunk_z + offset.second;

		auto found = chunks.find({ x, z });
		if (found == chunks.end() || !found->second) continue;

		Vector3 block_origin = { (float)(x * subchunk_size), 0, (float)(z * subchunk_size) };
		for (int i = 0; i < chunk_height_subchunks; i++)
		{
			Model* model = found->second->GetModel(i);
			if (!model) continue;
			DrawModel(*model, block_origin, 1, WHITE);
		}
	}
}

block_data* World::GetBlock(Vector3 Global)
{
	const int wx = (int)std::floor(Global.x);
	const int wy = (int)std::floor(Global.y);
	const int wz = (int)std::floor(Global.z);

	if (wy < 0 || wy >= subchunk_size * chunk_height_subchunks) return nullptr;

	const int cx = (int)std::floor(wx / (float)subchunk_size);
	const int cz = (int)std::floor(wz / (float)subchunk_size);

	auto found = chunks.find({ cx, cz });
	if (found == chunks.end() || !found->second) return nullptr;

	const int lx = wx - cx * subchunk_size;
	const int lz = wz - cz * subchunk_size;
	return &found->second->GetBlock({ (float)lx, (float)wy, (float)lz });
}

void World::BreakBlock(Vector3 Global)
{
	block_data* b = GetBlock(Global);
	if (!b) return;

	b->type = BLOCK_AIR;
	b->state = (block_state)0;

	const int wx = (int)std::floor(Global.x);
	const int wy = (int)std::floor(Global.y);
	const int wz = (int)std::floor(Global.z);

	if (wy < 0 || wy >= subchunk_size * chunk_height_subchunks) return;

	const int cx = (int)std::floor(wx / (float)subchunk_size);
	const int cz = (int)std::floor(wz / (float)subchunk_size);

	auto found = chunks.find({ cx, cz });
	if (found == chunks.end() || !found->second) return;

	const int lx = wx - cx * subchunk_size;
	const int lz = wz - cz * subchunk_size;
	found->second->GenerateMesh();
	found->second->GenerateModel();
}

void World::PlaceBlock(Vector3 Global)
{
	block_data* b = GetBlock(Global);
	if (!b) return;

	b->type = BLOCK_STONE;
	b->state = (block_state)0;

	const int wx = (int)std::floor(Global.x);
	const int wy = (int)std::floor(Global.y);
	const int wz = (int)std::floor(Global.z);

	if (wy < 0 || wy >= subchunk_size * chunk_height_subchunks) return;

	const int cx = (int)std::floor(wx / (float)subchunk_size);
	const int cz = (int)std::floor(wz / (float)subchunk_size);

	auto found = chunks.find({ cx, cz });
	if (found == chunks.end() || !found->second) return;

	const int lx = wx - cx * subchunk_size;
	const int lz = wz - cz * subchunk_size;
	found->second->GenerateMesh();
	found->second->GenerateModel();
}