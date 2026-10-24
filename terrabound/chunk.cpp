/*
 * =============================================================================
 * File        : chunk.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Chunk class, contains block data for full chunk, and mesh data
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#include "chunk.hpp"
#include <vector>

Chunk::Chunk(Vector3 position) : Position(position)
{
	for (int y = 0; y < subchunk_size * chunk_height_subchunks; y++)
		for (int x = 0; x < subchunk_size; x++)
			for (int z = 0; z < subchunk_size; z++)
			{
				block_data& b = GetBlock({ (float)x, (float)y, (float)z });
				b.type = BLOCK_FILLED;
				b.state = (block_state)GetRandomValue(0, 2);
			}
}

static void ReleaseSubchunk(Subchunk& subchunk)
{
	if (subchunk.subchunk_model.meshes) UnloadModel(subchunk.subchunk_model);
	subchunk.subchunk_model = Model{};

	if (subchunk.subchunk_mesh.vertices) UnloadMesh(subchunk.subchunk_mesh);
	subchunk.subchunk_mesh = Mesh{};
}

Chunk::~Chunk()
{
	for (int i = 0; i < chunk_height_subchunks; i++)
	{
		ReleaseSubchunk(subchunks[i]);
	}
}

#define CULL_NORTH 0b00000001
#define CULL_SOUTH 0b00000010
#define CULL_EAST  0b00000100
#define CULL_WEST  0b00001000
#define CULL_UP    0b00010000
#define CULL_DOWN  0b00100000

void Chunk::GenerateMesh()
{
	for (int i = 0; i < chunk_height_subchunks; i++)
	{
		ReleaseSubchunk(subchunks[i]);
	}
	has_model = false;

	for (int subchunk_i = 0; subchunk_i < chunk_height_subchunks; subchunk_i++)
	{
		std::vector<float> vertices;
		std::vector<unsigned char> colours;
		std::vector<float> normals;
		std::vector<unsigned short> indices;

		Subchunk& subchunk = subchunks[subchunk_i];

		for (int x = 0; x < subchunk_size; x++)
		for (int y = 0; y < subchunk_size; y++)
		for (int z = 0; z < subchunk_size; z++)
		{
			block_data& this_block = subchunk.blocks[x][y][z];
			if (this_block.type == BLOCK_AIR) continue;
			uint8_t cull = 0;

			uint8_t r = 0xFF, g = 0xFF, b = 0xFF, a = 0xFF;

			switch (this_block.state)
			{
				case BLOCK_RED:
				{
					r = 0xFF;
					g = 0x00;
					b = 0x00;
					a = 0xFF;
					break;
				}

				case BLOCK_GREEN:
				{
					r = 0x00;
					g = 0xFF;
					b = 0x00;
					a = 0xFF;
					break;
				}

				case BLOCK_BLUE:
				{
					r = 0x00;
					g = 0x00;
					b = 0xFF;
					a = 0xFF;
					break;
				}

				default:
				{
					break;
				}
			}

			if (x > 0 && subchunk.blocks[x - 1][y][z].type != BLOCK_AIR) cull |= CULL_WEST;
			if (x < subchunk_size - 1 && subchunk.blocks[x + 1][y][z].type != BLOCK_AIR) cull |= CULL_EAST;
			if (y > 0 && subchunk.blocks[x][y - 1][z].type != BLOCK_AIR) cull |= CULL_DOWN;
			if (y < subchunk_size - 1 && subchunk.blocks[x][y + 1][z].type != BLOCK_AIR) cull |= CULL_UP;
			if (z > 0 && subchunk.blocks[x][y][z - 1].type != BLOCK_AIR) cull |= CULL_SOUTH;
			if (z < subchunk_size - 1 && subchunk.blocks[x][y][z + 1].type != BLOCK_AIR) cull |= CULL_NORTH;

			int _y = y + (subchunk_size * subchunk_i);

			if (!(cull & CULL_NORTH))
			{
				const int indcs = vertices.size() / 3;

				// 0,0,1
				normals.push_back(0 + x);
				normals.push_back(0 + _y);
				normals.push_back(1 + z);
				vertices.push_back(0 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(1 + z);

				// 1,0,1
				normals.push_back(1 + x);
				normals.push_back(0 + _y);
				normals.push_back(1 + z);
				vertices.push_back(1 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(1 + z);

				// 1,1,1
				normals.push_back(1 + x);
				normals.push_back(1 + _y);
				normals.push_back(1 + z);
				vertices.push_back(1 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(1 + z);

				// 0,1,1
				normals.push_back(0 + x);
				normals.push_back(1 + _y);
				normals.push_back(1 + z);
				vertices.push_back(0 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(1 + z);

				// 0,1,2,2,3,0 NORTH
				indices.push_back(0 + indcs);
				indices.push_back(1 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(3 + indcs);
				indices.push_back(0 + indcs);

				for (int i = 0; i < 4; i++)
				{
					colours.push_back(r);
					colours.push_back(g);
					colours.push_back(b);
					colours.push_back(a);
				}
			}

			if (!(cull & CULL_SOUTH))
			{
				const int indcs = vertices.size() / 3;

				// 0,0,0
				normals.push_back(0 + x);
				normals.push_back(0 + _y);
				normals.push_back(0 + z);
				vertices.push_back(0 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(0 + z);

				// 1,0,0
				normals.push_back(1 + x);
				normals.push_back(0 + _y);
				normals.push_back(0 + z);
				vertices.push_back(1 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(0 + z);

				// 1,1,0
				normals.push_back(1 + x);
				normals.push_back(1 + _y);
				normals.push_back(0 + z);
				vertices.push_back(1 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(0 + z);

				// 0,1,0
				normals.push_back(0 + x);
				normals.push_back(1 + _y);
				normals.push_back(0 + z);
				vertices.push_back(0 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(0 + z);

				// 0,1,2,2,3,0
				indices.push_back(0 + indcs);
				indices.push_back(3 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(1 + indcs);
				indices.push_back(0 + indcs);

				for (int i = 0; i < 4; i++)
				{
					colours.push_back(r);
					colours.push_back(g);
					colours.push_back(b);
					colours.push_back(a);
				}
			}

			if (!(cull & CULL_EAST))
			{
				const int indcs = vertices.size() / 3;

				// 1,0,0
				normals.push_back(1);
				normals.push_back(0);
				normals.push_back(0);
				vertices.push_back(1 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(0 + z);

				// 1,0,1
				normals.push_back(1);
				normals.push_back(0);
				normals.push_back(0);
				vertices.push_back(1 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(1 + z);

				// 1,1,1
				normals.push_back(1);
				normals.push_back(0);
				normals.push_back(0);
				vertices.push_back(1 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(1 + z);

				// 1,1,0
				normals.push_back(1);
				normals.push_back(0);
				normals.push_back(0);
				vertices.push_back(1 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(0 + z);

				// 0,1,2,2,3,0
				indices.push_back(0 + indcs);
				indices.push_back(3 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(1 + indcs);
				indices.push_back(0 + indcs);

				for (int i = 0; i < 4; i++)
				{
					colours.push_back(r);
					colours.push_back(g);
					colours.push_back(b);
					colours.push_back(a);
				}
			}

			if (!(cull & CULL_WEST))
			{
				const int indcs = vertices.size() / 3;

				// 0,0,0
				normals.push_back(-1);
				normals.push_back(0);
				normals.push_back(0);
				vertices.push_back(0 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(0 + z);

				// 0,0,1
				normals.push_back(-1);
				normals.push_back(0);
				normals.push_back(0);
				vertices.push_back(0 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(1 + z);

				// 0,1,1
				normals.push_back(-1);
				normals.push_back(0);
				normals.push_back(0);
				vertices.push_back(0 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(1 + z);

				// 0,1,0
				normals.push_back(-1);
				normals.push_back(0);
				normals.push_back(0);
				vertices.push_back(0 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(0 + z);

				// 0,1,2,2,3,0
				indices.push_back(0 + indcs);
				indices.push_back(1 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(3 + indcs);
				indices.push_back(0 + indcs);

				for (int i = 0; i < 4; i++)
				{
					colours.push_back(r);
					colours.push_back(g);
					colours.push_back(b);
					colours.push_back(a);
				}
			}

			if (!(cull & CULL_UP))
			{
				const int indcs = vertices.size() / 3;

				// 0,1,0
				normals.push_back(0 + x);
				normals.push_back(1 + _y);
				normals.push_back(0 + z);
				vertices.push_back(0 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(0 + z);

				// 0,1,1
				normals.push_back(0 + x);
				normals.push_back(1 + _y);
				normals.push_back(1 + z);
				vertices.push_back(0 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(1 + z);

				// 1,1,1
				normals.push_back(1 + x);
				normals.push_back(1 + _y);
				normals.push_back(1 + z);
				vertices.push_back(1 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(1 + z);

				// 1,1,0
				normals.push_back(1 + x);
				normals.push_back(1 + _y);
				normals.push_back(0 + z);
				vertices.push_back(1 + x);
				vertices.push_back(1 + _y);
				vertices.push_back(0 + z);

				// 0,1,2,2,3,0
				indices.push_back(0 + indcs);
				indices.push_back(1 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(3 + indcs);
				indices.push_back(0 + indcs);

				for (int i = 0; i < 4; i++)
				{
					colours.push_back(r);
					colours.push_back(g);
					colours.push_back(b);
					colours.push_back(a);
				}
			}

			if (!(cull & CULL_DOWN))
			{
				const int indcs = vertices.size() / 3;

				// 0,0,0
				normals.push_back(0 + x);
				normals.push_back(0 + _y);
				normals.push_back(0 + z);
				vertices.push_back(0 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(0 + z);

				// 1,0,0
				normals.push_back(1 + x);
				normals.push_back(0 + _y);
				normals.push_back(0 + z);
				vertices.push_back(1 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(0 + z);

				// 1,0,1
				normals.push_back(1 + x);
				normals.push_back(0 + _y);
				normals.push_back(1 + z);
				vertices.push_back(1 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(1 + z);

				// 0,0,1
				normals.push_back(0 + x);
				normals.push_back(0 + _y);
				normals.push_back(1 + z);
				vertices.push_back(0 + x);
				vertices.push_back(0 + _y);
				vertices.push_back(1 + z);

				// 0,1,2,2,3,0
				indices.push_back(0 + indcs);
				indices.push_back(1 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(2 + indcs);
				indices.push_back(3 + indcs);
				indices.push_back(0 + indcs);

				for (int i = 0; i < 4; i++)
				{
					colours.push_back(r);
					colours.push_back(g);
					colours.push_back(b);
					colours.push_back(a);
				}
			}
		}

		Mesh mesh { };
		mesh.vertexCount = vertices.size() / 3;
		mesh.triangleCount = indices.size() / 3;
		mesh.vertices = (float*)MemAlloc(vertices.size() * sizeof(float));
		mesh.normals = (float*)MemAlloc(normals.size() * sizeof(float));
		mesh.colors = (unsigned char*)MemAlloc(colours.size() * sizeof(unsigned char));
		mesh.indices = (unsigned short*)MemAlloc(indices.size() * sizeof(unsigned short));

		std::copy(vertices.begin(), vertices.end(), mesh.vertices);
		std::copy(normals.begin(), normals.end(), mesh.normals);
		std::copy(colours.begin(), colours.end(), mesh.colors);
		std::copy(indices.begin(), indices.end(), mesh.indices);

		vertices.clear();
		indices.clear();
		normals.clear();
		colours.clear();

		subchunk.subchunk_mesh = mesh;

		UploadMesh(&subchunk.subchunk_mesh, false);
	}

	has_mesh = true;
}

void Chunk::GenerateModel()
{
	if (!has_mesh || has_model) return;

	for (int i = 0; i < chunk_height_subchunks; i++)
	{
		Subchunk& subchunk = subchunks[i];
		subchunk.subchunk_model = LoadModelFromMesh(subchunk.subchunk_mesh);
		subchunk.subchunk_mesh = Mesh { };
	}

	has_model = true;
}

Model* Chunk::GetModel(int subchunk)
{
	if (!has_model) return nullptr;
	if (subchunk < 0 || subchunk >= chunk_height_subchunks) return nullptr;
	return &subchunks[subchunk].subchunk_model;
}

static block_data null_block =
{
	.type = BLOCK_AIR,
	.state = BLOCK_RED
};
block_data& Chunk::GetBlock(Vector3 Relative)
{
	int x = (int)Relative.x;
	int y = (int)Relative.y;
	int z = (int)Relative.z;

	if (x < 0 || x >= subchunk_size ||
		y < 0 || y >= subchunk_size * chunk_height_subchunks ||
		z < 0 || z >= subchunk_size) return null_block;

	return subchunks[y / subchunk_size].blocks[x][y % subchunk_size][z];
}