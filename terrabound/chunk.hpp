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

constexpr int subchunk_size = 32;
constexpr int chunk_height_subchunks = 12;

struct Subchunk
{
	block_data blocks[subchunk_size][subchunk_size][subchunk_size];
	Mesh subchunk_mesh { };
	Model subchunk_model { };
};

class Chunk
{
public:
	Chunk(Vector3 position, uint64_t _seed);
	~Chunk();

	void GenerateMesh();
	void GenerateModel();

	block_data& GetBlock(Vector3 Relative);
	Model* GetModel(int subchunk);

	void SetNeighbour(int x, int y, Chunk* c);
	bool IsSolidAt(int x, int y, int z);

private:
	Vector3 Position;
	Subchunk subchunks[chunk_height_subchunks];
	Chunk* neighbours[4] = { };
	bool has_mesh = false;
	bool has_model = false;
	uint64_t seed;
};