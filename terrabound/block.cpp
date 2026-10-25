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
#include "raymath.h"
#include <iostream>

static float u[24];
static float v[24];

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




void Block::getTileTextures(block_data &data) {
	switch (data.type) {



	case BLOCK_DIRT : {
		//North
					//bottom left corner
		u[0] = 0.125f;
		v[0] = 0.0625;
		//bottom right corner
		u[1] = 0.1875;
		v[1] = 0.0625;
		//top right corner
		u[2] = 0.1875;
		v[2] = 0.00390625f;
		//top left corner
		u[3] = 0.125f;
		v[3] = 0.00390625f;

		//South
		//bottom left corner
		u[4] = u[0];
		v[4] = v[0];
		//bottom right corner
		u[5] = u[1];
		v[5] = v[1];
		//top right corner
		u[6] = u[2];
		v[6] = v[2];
		//top left corner
		u[7] = u[3];
		v[7] = v[3];

		//East
		//bottom left corner 
		u[8] = u[0];
		v[8] = v[0];
		//bottom right corner
		u[9] = u[1];
		v[9] = v[1];
		//top right corner
		u[10] = u[2];
		v[10] = v[2];
		//top left corner
		u[11] = u[3];
		v[11] = v[3];

		//West
		//bottom left corner
		u[12] = u[0];
		v[12] = v[0];
		//bottom right corner
		u[13] = u[1];
		v[13] = v[1];
		//top right corner
		u[14] = u[2];
		v[14] = v[2];
		//top left corner
		u[15] = u[3];
		v[15] = v[3];

		//Up Face
		//bottom left corner
		u[16] = u[0];
		v[16] = v[0];
		//bottom right corner
		u[17] = u[1];
		v[17] = v[1];
		//top right corner
		u[18] = u[2];
		v[18] = v[2];
		//top left corner
		u[19] = u[3];
		v[19] = v[3];

		//Down face
		//bottom left corner
		u[20] = u[0];
		v[20] = v[0];
		//bottom right corner
		u[21] = u[1];
		v[21] = v[1];
		//top right corner
		u[22] = u[2];
		v[22] = v[2];
		//top left corner
		u[23] = u[3];
		v[23] = v[3];
		//std::cout << "Dirt Block!" << "\n";
		break;
	}

	case BLOCK_STONE : {
		//North
		//bottom left corner
		u[0] = 0.1875;
		v[0] = 0.0625f;
		//bottom right corner
		u[1] = 0.25;
		v[1] = 0.0625f;
		//top right corner
		u[2] = 0.25;
		v[2] = 0.00390625f;
		//top left corner
		u[3] = 0.1875;
		v[3] = 0.00390625f;

		//South
		//bottom left corner
		u[4] = u[0];
		v[4] = v[0];
		//bottom right corner
		u[5] = u[1];
		v[5] = v[1];
		//top right corner
		u[6] = u[2];
		v[6] = v[2];
		//top left corner
		u[7] = u[3];
		v[7] = v[3];

		//East
		//bottom left corner 
		u[8] = u[0];
		v[8] = v[0];
		//bottom right corner
		u[9] = u[1];
		v[9] = v[1];
		//top right corner
		u[10] = u[2];
		v[10] = v[2];
		//top left corner
		u[11] = u[3];
		v[11] = v[3];

		//West
		//bottom left corner
		u[12] = u[0];
		v[12] = v[0];
		//bottom right corner
		u[13] = u[1];
		v[13] = v[1];
		//top right corner
		u[14] = u[2];
		v[14] = v[2];
		//top left corner
		u[15] = u[3];
		v[15] = v[3];

		//Up Face
		//bottom left corner
		u[16] = u[0];
		v[16] = v[0];
		//bottom right corner
		u[17] = u[1];
		v[17] = v[1];
		//top right corner
		u[18] = u[2];
		v[18] = v[2];
		//top left corner
		u[19] = u[3];
		v[19] = v[3];

		//Down face
		//bottom left corner
		u[20] = u[0];
		v[20] = v[0];
		//bottom right corner
		u[21] = u[1];
		v[21] = v[1];
		//top right corner
		u[22] = u[2];
		v[22] = v[2];
		//top left corner
		u[23] = u[3];
		v[23] = v[3];
		//std::cout << "Stone Block!" << "\n";
		break;
	}


	//REMEMBER to start from 0.00390625f, (pixel 0, 0) then multiply this value by how far your texture starts and ends in pixels
	case BLOCK_WOOD_LOG : {
		//North
			//bottom left corner
		u[0] = 0.375f;
		v[0] = 0.0625f;
		//bottom right corner
		u[1] = 0.4375f;
		v[1] = 0.0625f;
		//top right corner
		u[2] = 0.4375f;
		v[2] = 0.00390625f;
		//top left corner
		u[3] = 0.375f;
		v[3] = 0.00390625f;

		//South
		u[4] = u[0];
		v[4] = v[0];

		u[5] = u[1];
		v[5] = v[1];

		u[6] = u[2];
		v[6] = v[2];
		//top left corner
		u[7] = u[3];
		v[7] = v[3];

		//East
		u[8] = u[0];
		v[8] = v[0];

		u[9] = u[1];
		v[9] = v[1];

		u[10] = u[2];
		v[10] = v[2];
		//top left corner
		u[11] = u[3];
		v[11] = v[3];

		//West
		u[12] = u[0];
		v[12] = v[0];

		u[13] = u[1];
		v[13] = v[1];

		u[14] = u[2];
		v[14] = v[2];
		//top left corner
		u[15] = u[3];
		v[15] = v[3];

		//Up Face
		//bottom left
		u[16] = 0.4375f; //0.00390625f
		v[16] = 0.0625f;
		//bottom right
		u[17] = 0.49609375f;
		v[17] = 0.0625f;
		//top right
		u[18] = 0.49609375f;
		v[18] = 0.00390625f;
		//top left corner
		u[19] = 0.4375f;
		v[19] = 0.00390625f;

		//Down face

		u[20] = u[16];
		v[20] = v[16];

		u[21] = u[17];
		v[21] = v[17];
		//top right
		u[22] = u[18];
		v[22] = v[18];
		//top left corner
		u[23] = u[19];
		v[23] = v[19];
	//	std::cout << "Wood Block!" << "\n";
		break;
	}

	case BLOCK_TREE_LEAVES : {
		//North
		//bottom left corner
		u[0] = 0.5f;
		v[0] = 0.0625f;
		//bottom right corner //0.00390625f
		u[1] = 0.5625f;
		v[1] = 0.0625f;
		//top right corner
		u[2] = 0.5625f;
		v[2] = 0.00390625f;
		//top left corner
		u[3] = 0.5f;
		v[3] = 0.00390625f;

		//South
		//bottom left corner
		u[4] = u[0];
		v[4] = v[0];
		//bottom right corner
		u[5] = u[1];
		v[5] = v[1];
		//top right corner
		u[6] = u[2];
		v[6] = v[2];
		//top left corner
		u[7] = u[3];
		v[7] = v[3];

		//East
		//bottom left corner 
		u[8] = u[0];
		v[8] = v[0];
		//bottom right corner
		u[9] = u[1];
		v[9] = v[1];
		//top right corner
		u[10] = u[2];
		v[10] = v[2];
		//top left corner
		u[11] = u[3];
		v[11] = v[3];

		//West
		//bottom left corner
		u[12] = u[0];
		v[12] = v[0];
		//bottom right corner
		u[13] = u[1];
		v[13] = v[1];
		//top right corner
		u[14] = u[2];
		v[14] = v[2];
		//top left corner
		u[15] = u[3];
		v[15] = v[3];

		//Up Face
		//bottom left corner
		u[16] = u[0];
		v[16] = v[0];
		//bottom right corner
		u[17] = u[1];
		v[17] = v[1];
		//top right corner
		u[18] = u[2];
		v[18] = v[2];
		//top left corner
		u[19] = u[3];
		v[19] = v[3];

		//Down face
		//bottom left corner
		u[20] = u[0];
		v[20] = v[0];
		//bottom right corner
		u[21] = u[1];
		v[21] = v[1];
		//top right corner
		u[22] = u[2];
		v[22] = v[2];
		//top left corner
		u[23] = u[3];
		v[23] = v[3];
		//std::cout << "Leaves Block!" << "\n";
		break;
	}

	case BLOCK_BUSH_LEAVES : {
		//break;
	}

	case BLOCK_GRASS: {
		//North
		//bottom left corner
		u[0] = 0.0625f;
		v[0] = 0.0625f;
		//bottom right corner
		u[1] = 0.125f;
		v[1] = 0.0625f;
		//top right corner
		u[2] = 0.125f;
		v[2] = 0.00390625f;
		//top left corner
		u[3] = 0.0625f;
		v[3] = 0.00390625f;


		//South
		u[4] = u[0];
		v[4] = v[0];

		u[5] = u[1];
		v[5] = v[1];

		u[6] = u[2];
		v[6] = v[2];
		//top left corner
		u[7] = u[3];
		v[7] = v[3];

		//East
		u[8] = u[0];
		v[8] = v[0];

		u[9] = u[1];
		v[9] = v[1];

		u[10] = u[2];
		v[10] = v[2];
		//top left corner
		u[11] = u[3];
		v[11] = v[3];

		//West
		u[12] = u[0];
		v[12] = v[0];

		u[13] = u[1];
		v[13] = v[1];

		u[14] = u[2];
		v[14] = v[2];
		//top left corner
		u[15] = u[3];
		v[15] = v[3];

		//Up Face
		u[16] = 0.00390625f;
		v[16] = 0.00390625f;

		u[17] = 0.0625;
		v[17] = 0.00390625f;

		u[18] = 0.0625;
		v[18] = 0.0625;
		//top left corner
		u[19] = 0.00390625f;
		v[19] = 0.0625;

		//Down face

		u[20] = 0.125f;
		v[20] = 0.0625f;

		u[21] = 0.1875f;
		v[21] = 0.0625f;
		//top right
		u[22] = 0.1875;
		v[22] = 0.00390625f;
		//top left corner
		u[23] = 0.125f;
		v[23] = 0.00390625f;
		//	std::cout << "Grass Block!" << "\n";
		break;
	}

	case BLOCK_SHORT_GRASS : {
		//break;
	}

	default : {
		break;
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

	//REMEMBER to start from 0.00390625f, (pixel 0, 0) then multiply this value by how far your texture starts and ends in pixels

	float texCoordsExpr[6][4][2] = {
		{ {u[0], v[0]}, {u[1], v[1]}, {u[2], v[2]}, {u[3], v[3]}}, //North
		{ {u[4], v[4]}, {u[5], v[5]}, {u[6], v[6]}, {u[7], v[7]}}, //East
		{ {u[8], v[8]}, {u[9], v[9]}, {u[10], v[10]}, {u[11], v[11]}}, //West
		{ {u[12], v[12]}, {u[13], v[13]}, {u[14], v[14]}, {u[15], v[15]}}, //South
		{ {u[16], v[16]}, {u[17], v[17]}, {u[18], v[18]}, {u[19], v[19]}}, //Up
		{ {u[20], v[20]}, {u[21], v[21]}, {u[22], v[22]}, {u[23], v[23]}} //Down

		//{ { 0.1875, 0.0625f }, {0.25f, 0.0625f}, {0.25f, 0.00390625f}, {0.1875f, 0.00390625f} }, //North
		//{ {0.1875, 0.0625f}, {0.25f, 0.0625f}, {0.25f, 0.00390625f}, {0.1875f, 0.00390625f} }, //East
		//{ {0.1875, 0.0625f}, {0.25f, 0.0625f}, {0.25f, 0.00390625f}, {0.1875f, 0.00390625f} }, //West
		//{ {0.1875, 0.0625f}, {0.25f, 0.0625f}, {0.25f, 0.00390625f}, {0.1875f, 0.00390625f} }, //South
		//{ {0.1875, 0.0625f}, {0.25f, 0.0625f}, {0.25f, 0.00390625f}, {0.1875f, 0.00390625f} }, //Up
		//{ {0.1875, 0.0625f}, {0.25f, 0.0625f}, {0.25f, 0.00390625f}, {0.1875f, 0.00390625f} } //Down

		//{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //North
		//{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //East
		//{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //West
		//{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //South
		//{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //Up
		//{ {0, 0}, {1, 0}, {1, 1}, {0, 1} } //Down
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

			for (int cd = 0; cd < 2; cd++) {
				mesh.texCoords.push_back(texCoordsExpr[f][v][cd]);
			}
		}

		for (int i = 0; i < 6; i++)
		{
			mesh.indices.push_back(base + order[f][i]);
		}

		mesh.cull.push_back(cull_bits[f]);
	}
	//mesh.texCoords.clear();
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

	float texCoordsExpr[6][4][2] = {
		{ {u[0], v[0]}, {u[1], v[1]}, {u[2], v[2]}, {u[3], v[3]}}, //North
		{ {u[4], v[4]}, {u[5], v[5]}, {u[6], v[6]}, {u[7], v[7]}}, //East
		{ {u[8], v[8]}, {u[9], v[9]}, {u[10], v[10]}, {u[11], v[11]}}, //West
		{ {u[12], v[12]}, {u[13], v[13]}, {u[14], v[14]}, {u[15], v[15]}}, //South
		{ {u[16], v[16]}, {u[17], v[17]}, {u[18], v[18]}, {u[19], v[19]}}, //Up
		{ {u[20], v[20]}, {u[21], v[21]}, {u[22], v[22]}, {u[23], v[23]}} //Down
	};

	//float texCoordsExpr[6][4][2] = {
	//	{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //North
	//	{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //East
	//	{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //West
	//	{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //South
	//	{ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, //Up
	//	{ {0, 0}, {1, 0}, {1, 1}, {0, 1} } //Down
	//};


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

			for (int cd = 0; cd < 2; cd++) {
				mesh.texCoords.push_back(texCoordsExpr[f][v][cd]);
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

	switch (data.type)
	{
		case BLOCK_AIR:
		case BLOCK_GRASS:
		case BLOCK_DIRT:
		case BLOCK_STONE:
		case BLOCK_WOOD_LOG:
		case BLOCK_TREE_LEAVES:
		case BLOCK_BUSH_LEAVES:
		{
			return cube;
		}

		case BLOCK_SHORT_GRASS:
		{
			return plant;
		}

		default:
			return cube;
	}
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