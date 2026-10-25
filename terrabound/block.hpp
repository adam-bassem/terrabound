/*
 * =============================================================================
 * File        : block.hpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Block struct, used by chunk code
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#pragma once

#include <cstdint>
#include <vector>

enum block_type
{
	BLOCK_AIR,
	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_WOOD_LOG,
	BLOCK_TREE_LEAVES,
	BLOCK_BUSH_LEAVES,
	BLOCK_SHORT_GRASS,
};

// test states for filled block,
// later add actual states for actual blocks
enum block_state
{
};

struct block_data
{
	block_type type;
	block_state state;
	bool transparent;
};

struct block_mesh
{
	// local 0..1, 4 corners per quad, 3 floats each
	std::vector<float> vertices;
	// face direction, 4 per quad, 3 floats each
	std::vector<float> normals;
	// RGBA per corner, alpha channel is used by AO
	std::vector<unsigned char> colours;
	// 6 per quad, values 0..3 within that quad
	std::vector<unsigned int> indices;
	// which neighbour hides each quad, zero means no neighbour hides it.
	std::vector<uint8_t> cull;
	// if true then the block can be shaded with AO
	// just helpful for blocks like grass or flowers
	bool allow_ao = true;
};

namespace Block
{
	uint32_t GetColour(block_data data);
	float GetHardness(block_data& data);
	block_mesh GetBlockMesh(block_data& data);
	static block_mesh CubeBlock();
	static block_mesh PlantBlock();
	block_data GetBlock(block_type type);
}
