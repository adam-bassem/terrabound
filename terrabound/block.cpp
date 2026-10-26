/*
 * ============================================================================
 * File        : block.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Block struct, used by chunk code
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#include "block.hpp"

uint32_t Block::GetColour(block_data data)
{
	switch (data.type)
	{
		case BLOCK_AIR:
		{
			return 0xFF000000;
		}

		case BLOCK_GRASS:
		{
			return 0xFF1F642D;
		}

		case BLOCK_DIRT:
		{
			return 0xFF73523B;
		}

		case BLOCK_STONE:
		{
			return 0xFF888C8D;
		}

		case BLOCK_WOOD_LOG:
		{
			return 0xFF583C25;
		}

		case BLOCK_TREE_LEAVES:
		{
			return 0xFF71AA34;
		}

		case BLOCK_BUSH_LEAVES:
		{
			return 0xFF228B22;
		}

		case BLOCK_SHORT_GRASS:
		{
			return 0xFF0E431C;
		}

		default:
		{
			return 0xFF8889B8;
		}
	}
}

float Block::GetHardness(block_data& data)
{
	switch (data.type)
	{
		case BLOCK_AIR:
		{
			return 0;
		}

		case BLOCK_GRASS:
		{
			return 0.6f;
		}

		case BLOCK_DIRT:
		{
			return 0.5f;
		}

		case BLOCK_STONE:
		{
			return 2.0f;
		}

		case BLOCK_WOOD_LOG:
		{
			return 1.5f;
		}

		case BLOCK_TREE_LEAVES:
		{
			return 0.5f;
		}

		case BLOCK_BUSH_LEAVES:
		{
			return 0.5f;
		}

		case BLOCK_SHORT_GRASS:
		{
			return 0.25f;
		}

		default:
		{
			return 1e9;
		}
	}
}

#define CULL_NORTH 0b00000001
#define CULL_SOUTH 0b00000010
#define CULL_EAST 0b00000100
#define CULL_WEST 0b00001000
#define CULL_UP 0b00010000
#define CULL_DOWN 0b00100000

static block_mesh Block::CubeBlock()
{
	block_mesh mesh;

	// for each face there are 4 corners in push order, face normal,
	// indcies, and the cull bit that hides it
	constexpr float corners[6][4][3] =
	{
		{ { 0,0,1 }, { 1,0,1 }, { 1,1,1 }, { 0,1,1 } }, // north
		{ { 0,0,0 }, { 1,0,0 }, { 1,1,0 }, { 0,1,0 } }, // south
		{ { 1,0,0 }, { 1,0,1 }, { 1,1,1 }, { 1,1,0 } }, // east
		{ { 0,0,0 }, { 0,0,1 }, { 0,1,1 }, { 0,1,0 } }, // west
		{ { 0,1,0 }, { 0,1,1 }, { 1,1,1 }, { 1,1,0 } }, // up
		{ { 0,0,0 }, { 1,0,0 }, { 1,0,1 }, { 0,0,1 } }, // down
	};

	constexpr float normals[6][3] =
	{
		{ 0, 0, 1 }, { 0, 0, -1 }, { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }
	};

	constexpr uint8_t cull_bits[6] =
	{
		CULL_NORTH, CULL_SOUTH, CULL_EAST, CULL_WEST, CULL_UP, CULL_DOWN
	};

	constexpr unsigned int order[6][6] =
	{
		{ 0,1,2, 2,3,0 }, // north
		{ 0,3,2, 2,1,0 }, // south
		{ 0,3,2, 2,1,0 }, // east
		{ 0,1,2, 2,3,0 }, // west
		{ 0,1,2, 2,3,0 }, // up
		{ 0,1,2, 2,3,0 }, // down
	};

	for (int f = 0; f < 6; f++)
	{
		const unsigned int base = mesh.vertices.size() / 3;

		for (int v = 0; v < 4; v++)
		{
			for (int c = 0; c < 3; c++)
			{
				mesh.vertices.push_back(corners[f][v][c]);
				mesh.normals.push_back(normals[f][c]);
			}
		}

		for (int i = 0; i < 6; i++)
		{
			mesh.indices.push_back(base + order[f][i]);
		}

		mesh.cull.push_back(cull_bits[f]);
	}

	return mesh;
}

block_mesh Block::PlantBlock()
{
	block_mesh mesh;

	// two crossed quads, each one drawn from both sides by reversing its winding
	constexpr float corners[4][4][3] =
	{
		{ { 0,0,0 }, { 1,0,1 }, { 1,1,1 }, { 0,1,0 } },
		{ { 0,0,0 }, { 0,1,0 }, { 1,1,1 }, { 1,0,1 } },
		{ { 1,0,0 }, { 0,0,1 }, { 0,1,1 }, { 1,1,0 } },
		{ { 1,0,0 }, { 1,1,0 }, { 0,1,1 }, { 0,0,1 } },
	};

	// normals point across each diagonal, the reversed copies point the other way
	constexpr float normals[4][3] =
	{
		{ 0.7071f, 0, -0.7071f }, { -0.7071f, 0, 0.7071f },
		{ -0.7071f, 0, -0.7071f }, { 0.7071f, 0, 0.7071f },
	};

	// plants are never hidden by neighbours
	constexpr uint8_t cull_bits[4] = { 0, 0, 0, 0 };

	// vertex order already carries the winding, so the index pattern is the same for each quad
	constexpr unsigned int order[6] = { 0,1,2, 2,3,0 };

	for (int f = 0; f < 4; f++)
	{
		const unsigned int base = mesh.vertices.size() / 3;

		for (int v = 0; v < 4; v++)
		{
			for (int c = 0; c < 3; c++)
			{
				mesh.vertices.push_back(corners[f][v][c]);
				mesh.normals.push_back(normals[f][c]);
			}
		}

		for (int i = 0; i < 6; i++)
		{
			mesh.indices.push_back(base + order[i]);
		}

		mesh.cull.push_back(cull_bits[f]);
	}

	mesh.allow_ao = false;
	return mesh;
}

block_mesh Block::GetBlockMesh(block_data& data)
{
	static const block_mesh cube = CubeBlock();
	static const block_mesh plant = PlantBlock();

	block_mesh mesh;

	switch (data.type)
	{
		case BLOCK_SHORT_GRASS:
		{
			mesh = plant;
			break;
		}

		default:
		{
			mesh = cube;
			break;
		}
	}

	mesh.texcoords = GetUV(data.type, 0);
	return mesh;
}

block_data MakeBlock(block_type type)
{
	return block_data { type, (block_state)0, false };
}

block_data Block::GetBlock(block_type type)
{
	switch (type)
	{
		case BLOCK_AIR:
		{
			return block_data { type, (block_state)0, true };
		}

		case BLOCK_GRASS:
		case BLOCK_DIRT:
		case BLOCK_STONE:
		case BLOCK_WOOD_LOG:
		{
			return MakeBlock(type);
		}

		case BLOCK_TREE_LEAVES:
		case BLOCK_BUSH_LEAVES:
		case BLOCK_SHORT_GRASS:
		{
			return block_data { type, (block_state)0, true };
		}

		default:
		{
			return block_data { type, (block_state)0, true };
		}
	}
}

#define ATLAS_SIZE 256
#define BLOCK_TEXTURE_WIDTH 16
#define CELL_TO_UV(x) ((float)(x) * BLOCK_TEXTURE_WIDTH / (float)ATLAS_SIZE)

std::vector<UV> Block::GetUV(block_type type, int variant)
{
	(void)variant;

	static const block_mesh cube = CubeBlock();
	static const block_mesh plant = PlantBlock();

	const bool is_plant = type == BLOCK_SHORT_GRASS;
	const block_mesh& geo = is_plant ? plant : cube;

	const size_t vertex_count = geo.vertices.size() / 3;

	std::vector<UV> uvs;
	uvs.reserve(vertex_count);

	for (size_t i = 0; i < vertex_count; i++)
	{
		const size_t face = i / 4;
		const float* p = &geo.vertices[i * 3];
		const float* n = &geo.normals[i * 3];

		// atlas cell for this block type, grass picks by face
		int col = 0;
		int row = 0;

		switch (type)
		{
			case BLOCK_GRASS:
			{
				if (face == 4) col = 0;
				else if (face == 5) col = 1;
				else col = 2;
				break;
			}

			case BLOCK_DIRT:
			{
				col = 1;
				break;
			}

			case BLOCK_STONE:
			{
				col = 3;
				break;
			}

			case BLOCK_WOOD_LOG:
			{
				col = 4;
				break;
			}

			case BLOCK_TREE_LEAVES:
			{
				col = 5;
				break;
			}

			case BLOCK_BUSH_LEAVES:
			{
				col = 6;
				break;
			}

			case BLOCK_SHORT_GRASS:
			{
				col = 7;
				break;
			}

			default:
			{
				col = 0;
				row = 15;
				break;
			}
		}

		const float x1 = CELL_TO_UV(col);
		const float x2 = CELL_TO_UV(col + 1);
		const float y1 = CELL_TO_UV(row);
		const float y2 = CELL_TO_UV(row + 1);

		// a runs across the face, b runs up it, both in [0, 1]
		float a, b;
		if (is_plant)
		{
			// the two diagonals run along different axes
			a = (n[0] * n[2] < 0.0f) ? (p[0] + p[2]) * 0.5f : (1.0f - p[0] + p[2]) * 0.5f;
			b = p[1];
		}
		else if (face < 2)
		{
			a = p[0];
			b = p[1];
		}
		else if (face < 4)
		{
			a = p[2];
			b = p[1];
		}
		else
		{
			a = p[0];
			b = p[2];
		}

		uvs.push_back(UV { x1 + a * (x2 - x1), y1 + (1.0f - b) * (y2 - y1) });
	}

	return uvs;
}