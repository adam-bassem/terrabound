/*
 * =============================================================================
 * File        : chunk.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Chunk class, contains block data for full chunk, and mesh data and generation
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#pragma once

#include "block.hpp"
#include "raylib.h"
#include <string>
#include <random>
//#include "WorldDecorations.h" //Doesn't work

constexpr int subchunk_size = 32;
constexpr int chunk_height_subchunks = 12; //Default 12

struct Subchunk
{
	block_data blocks[subchunk_size][subchunk_size][subchunk_size] { };
	Mesh subchunk_mesh { };
	Model subchunk_model { };
	bool is_dirty = true;
};

class Chunk
{
public:
	Chunk(Vector3 position, uint64_t _seed);
	~Chunk();

	void DecorateSecondPass();

	void GenerateMesh();
	void GenerateModel();

	block_data& GetBlock(Vector3 Relative);
	void SetBlock(Vector3 Relative, block_data data);
	Model* GetModel(int subchunk);

	void SetNeighbour(int x, int y, Chunk* c);
	bool IsSolidAt(int x, int y, int z, bool transparent_is_solid = false);
	float GetAO(int x, int y, int z, const int n[3], const int u[3], const int v[3], int su, int sv);

	bool decorated = false;
	void MarkDirtyAt(int x, int y, int z);

	uint64_t seed;
	Vector3 Position;
	int heightmap[subchunk_size][subchunk_size];
	Subchunk subchunks[chunk_height_subchunks];
private:

	Chunk* neighbours[4] = { };
	bool has_mesh = false;
	bool has_model = false;

	

	
	void DecorateTree(Vector3 base, std::mt19937_64& rng);
	void DecoratePineTree(Vector3 base, std::mt19937_64& rng);
	void DecorateBush(Vector3 base);
	void decoratePumpkins(Vector3 base);
	void decorateSkyIsland(Vector3 base, std::mt19937_64& rng);
};