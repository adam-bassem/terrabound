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
	BLOCK_PINE_LOG,
	BLOCK_PINE_LEAVES,
	BLOCK_BUSH_LEAVES,
	BLOCK_SHORT_GRASS,
	BLOCK_RED_FLOWER,
	BLOCK_OXEYE_DAISY,
	BLOCK_SAND,
	BLOCK_WATER,
	BLOCK_LAVA,
	BLOCK_ROCK,
	BLOCK_CLAY,
	BLOCK_BRICKS,
	BLOCK_GRAVEL,
	BLOCK_SNOW,
	BLOCK_BEDROCK,
	BLOCK_TREE,
	BLOCK_COAL_ORE,
	BLOCK_IRON_ORE,
	BLOCK_DRY_FARLAND,
	BLOCK_MOIST_FARLAND,
	BLOCK_PINE_TREE,
	BLOCK_WOOD_PLANK,
	BLOCK_PINE_WOOD_PLANK,
	BLOCK_MOSSY_ROCK,
	BLOCK_GOLD_ORE,
	BLOCK_DIAMOND_ORE,
	BLOCK_TITANIUM_ORE	
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

struct UV
{
	float x1, y1;
	float x2, y2;
};

struct block_mesh
{
	// local 0..1, 4 corners per quad, 3 floats each
	std::vector<float> vertices;
	// face direction, 4 per quad, 3 floats each
	std::vector<float> normals;
	// Texture
	std::vector<UV> texcoords;
	// 6 per quad, values 0..3 within that quad
	std::vector<unsigned int> indices;
	// which neighbour hides each quad, zero means no neighbour hides it.
	std::vector<uint8_t> cull;
	// if true then the block can be shaded with AO
	// just helpful for blocks like grass or flowers
	bool allow_ao = true;
};

#define GRASS_BLOCK_VARIANT_TOP 0
#define GRASS_BLOCK_VARIANT_SIDE 1
#define GRASS_BLOCK_VARIANT_BOTTOM 2

#define TREE_LOG_VARIANT_TOP 0
#define TREE_LOG_VARIANT_SIDE 1

namespace Block
{
	uint32_t GetColour(block_data data);
	float GetHardness(block_data& data);
	block_mesh GetBlockMesh(block_data& data);
	static block_mesh CubeBlock();
	static block_mesh PlantBlock();
	static block_mesh flowerBlock();
	block_data GetBlock(block_type type);
	std::vector<UV> GetUV(block_type type, int variant);
}
