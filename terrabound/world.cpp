/*
 * ============================================================================
 * File        : world.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     World contains chunks around the camera, unloading far chunks to disk.
 *     New chunks are generated on a background thread.
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
#include <future>
#include <algorithm>
#include <filesystem>
#include <string>

namespace
{
	std::map<std::pair<int, int>, Chunk*> chunks;
	std::set<std::pair<int, int>> to_remesh;
	uint64_t seed;
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

	auto inside = [&](int x, int z)
		{
			int a = (x - chunk_x) * (x - chunk_x);
			int b = (z - chunk_z) * (z - chunk_z);
			return a + b <= r * r;
		};

	// Pass 1: create and link new chunks, within budget
	int created = 0;
	for (int x = chunk_x - r; x <= chunk_x + r; x++)
	{
		for (int z = chunk_z - r; z <= chunk_z + r; z++)
		{
			if (!inside(x, z)) continue;

			Chunk*& c = chunks[{ x, z }];
			if (c || created >= max_new_chunks) continue;

			created++;
			Vector3 block_origin = { (float)(x * subchunk_size), 0, (float)(z * subchunk_size) };
			c = new Chunk(block_origin, seed);

			for (int i = 0; i < 4; i++)
			{
				auto it = chunks.find({ x + dx[i], z + dz[i] });
				if (it == chunks.end() || !it->second) continue;

				c->SetNeighbour(dx[i], dz[i], it->second);
				it->second->SetNeighbour(-dx[i], -dz[i], c);
				to_remesh.insert({ x + dx[i], z + dz[i] });
			}
			to_remesh.insert({ x, z });
		}
	}

	// Pass 2: remesh dirty chunks, within budget
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
	for (int x = chunk_x - r; x <= chunk_x + r; x++)
	{
		for (int z = chunk_z - r; z <= chunk_z + r; z++)
		{
			if (!inside(x, z)) continue;

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
}