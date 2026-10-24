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

#include "chunk.hpp"
#include <vector>
#include "FastNoiseLite.hpp"
#include <algorithm>
#include "postprocessing.hpp"

Chunk::Chunk(Vector3 position, uint64_t _seed) : Position(position), seed(_seed)
{
	FastNoiseLite heightmap;
	heightmap.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	heightmap.SetSeed((int)seed);
	heightmap.SetFrequency(0.005f);
	heightmap.SetFractalType(FastNoiseLite::FractalType_FBm);
	heightmap.SetFractalOctaves(8);
	heightmap.SetFractalLacunarity(1.85f);
	heightmap.SetFractalGain(0.37f);
	heightmap.SetFractalWeightedStrength(0.11f);
	heightmap.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
	heightmap.SetDomainWarpAmp(22.0f);

	FastNoiseLite cave_a_spag;
	cave_a_spag.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	cave_a_spag.SetSeed((int)seed);
	cave_a_spag.SetFrequency(0.03f);
	cave_a_spag.SetFractalType(FastNoiseLite::FractalType_None);

	FastNoiseLite cave_b_spag;
	cave_b_spag.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	cave_b_spag.SetSeed((int)seed + 1337);
	cave_b_spag.SetFrequency(0.03f);
	cave_b_spag.SetFractalType(FastNoiseLite::FractalType_None);

	constexpr float terrain_base = 96.0f;
	constexpr float terrain_amp = 120.0f;
	constexpr float cave_threshold = 0.08f;

	int heightmap_heights[subchunk_size][subchunk_size];

	// Pass 0: heightmap
	for (int x = 0; x < subchunk_size; x++)
	{
		for (int z = 0; z < subchunk_size; z++)
		{
			float n = heightmap.GetNoise(position.x + x, position.z + z);
			float h = std::pow(std::tanh(n * 2.0f) * 0.5f + 0.5f, 1.5f);
			heightmap_heights[x][z] = (int)(terrain_base + h * terrain_amp);
		}
	}

	// Pass 1: fill layers and carve caves in one walk over each column
	for (int x = 0; x < subchunk_size; x++)
	{
		for (int z = 0; z < subchunk_size; z++)
		{
			const int top = heightmap_heights[x][z];
			const float wx = position.x + x;
			const float wz = position.z + z;

			for (int y = 0; y < top; y++)
			{
				block_data& b = subchunks[y / subchunk_size].blocks[x][y % subchunk_size][z];

				if (y == top - 1)
				{
					b.type = BLOCK_GRASS;
				}
				else if (y >= top - 4)
				{
					b.type = BLOCK_DIRT;
				}
				else
				{
					b.type = BLOCK_STONE;
				}
				b.state = (block_state)GetRandomValue(0, 2);

				if (y < 1 || y > top - 5) continue;

				const float fy = (float)y;
				if (fabsf(cave_a_spag.GetNoise(wx, fy, wz)) >= cave_threshold) continue;
				if (fabsf(cave_b_spag.GetNoise(wx, fy, wz)) >= cave_threshold) continue;

				b.type = BLOCK_AIR;
			}
		}
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
#define CULL_EAST 0b00000100
#define CULL_WEST 0b00001000
#define CULL_UP 0b00010000
#define CULL_DOWN 0b00100000

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
		std::vector<unsigned int> indices;

		Subchunk& subchunk = subchunks[subchunk_i];

		for (int x = 0; x < subchunk_size; x++)
		{
			for (int y = 0; y < subchunk_size; y++)
			{
				for (int z = 0; z < subchunk_size; z++)
				{
					block_data& this_block = subchunk.blocks[x][y][z];
					if (this_block.type == BLOCK_AIR) continue;
					uint8_t cull = 0;

					uint32_t colour = Block::GetColour(this_block);
					// 0xAARRGGBB
					uint8_t a = (colour >> 24) & 0xFF;
					uint8_t r = (colour >> 16) & 0xFF;
					uint8_t g = (colour >> 8) & 0xFF;
					uint8_t b = colour & 0xFF;

					int _y = y + (subchunk_size * subchunk_i);

					if (IsSolidAt(x - 1, _y, z))
					{
						cull |= CULL_WEST;
					}

					if (IsSolidAt(x + 1, _y, z))
					{
						cull |= CULL_EAST;
					}

					if (IsSolidAt(x, _y - 1, z))
					{
						cull |= CULL_DOWN;
					}

					if (IsSolidAt(x, _y + 1, z))
					{
						cull |= CULL_UP;
					}

					if (IsSolidAt(x, _y, z - 1))
					{
						cull |= CULL_SOUTH;
					}

					if (IsSolidAt(x, _y, z + 1))
					{
						cull |= CULL_NORTH;
					}

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

						constexpr int n[3] = { 0, 0, 1 };
						constexpr int u[3] = { 1, 0, 0 };
						constexpr int v[3] = { 0, 1, 0 };

						const float ao[4] = {
							GetAO(x, _y, z, n, u, v, -1, -1),
							GetAO(x, _y, z, n, u, v, 1, -1),
							GetAO(x, _y, z, n, u, v, 1, 1),
							GetAO(x, _y, z, n, u, v, -1, 1),
						};
						for (int i = 0; i < 4; i++)
						{
							colours.push_back(r);
							colours.push_back(g);
							colours.push_back(b);
							colours.push_back((unsigned char)(ao[i] * 255.0f));
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

						constexpr int n[3] = { 0, 0, -1 };
						constexpr int u[3] = { 1, 0, 0 };
						constexpr int v[3] = { 0, 1, 0 };

						const float ao[4] = {
							GetAO(x, _y, z, n, u, v, -1, -1),
							GetAO(x, _y, z, n, u, v, 1, -1),
							GetAO(x, _y, z, n, u, v, 1, 1),
							GetAO(x, _y, z, n, u, v, -1, 1),
						};
						for (int i = 0; i < 4; i++)
						{
							colours.push_back(r);
							colours.push_back(g);
							colours.push_back(b);
							colours.push_back((unsigned char)(ao[i] * 255.0f));
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

						constexpr int n[3] = { 1, 0, 0 };
						constexpr int u[3] = { 0, 0, 1 };
						constexpr int v[3] = { 0, 1, 0 };

						const float ao[4] = {
							GetAO(x, _y, z, n, u, v, -1, -1),
							GetAO(x, _y, z, n, u, v, 1, -1),
							GetAO(x, _y, z, n, u, v, 1, 1),
							GetAO(x, _y, z, n, u, v, -1, 1),
						};
						for (int i = 0; i < 4; i++)
						{
							colours.push_back(r);
							colours.push_back(g);
							colours.push_back(b);
							colours.push_back((unsigned char)(ao[i] * 255.0f));
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

						constexpr int n[3] = { -1, 0, 0 };
						constexpr int u[3] = { 0, 0, 1 };
						constexpr int v[3] = { 0, 1, 0 };

						const float ao[4] = {
							GetAO(x, _y, z, n, u, v, -1, -1),
							GetAO(x, _y, z, n, u, v, 1, -1),
							GetAO(x, _y, z, n, u, v, 1, 1),
							GetAO(x, _y, z, n, u, v, -1, 1),
						};
						for (int i = 0; i < 4; i++)
						{
							colours.push_back(r);
							colours.push_back(g);
							colours.push_back(b);
							colours.push_back((unsigned char)(ao[i] * 255.0f));
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

						constexpr int n[3] = { 0, 1, 0 };
						constexpr int u[3] = { 1, 0, 0 };
						constexpr int v[3] = { 0, 0, 1 };

						const float ao[4] = {
							GetAO(x, _y, z, n, u, v, -1, -1),
							GetAO(x, _y, z, n, u, v, -1, 1),
							GetAO(x, _y, z, n, u, v, 1, 1),
							GetAO(x, _y, z, n, u, v, 1, -1),
						};
						for (int i = 0; i < 4; i++)
						{
							colours.push_back(r);
							colours.push_back(g);
							colours.push_back(b);
							colours.push_back((unsigned char)(ao[i] * 255.0f));
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

						constexpr int n[3] = { 0, -1, 0 };
						constexpr int u[3] = { 1, 0, 0 };
						constexpr int v[3] = { 0, 0, 1 };

						const float ao[4] = {
							GetAO(x, _y, z, n, u, v, -1, -1),
							GetAO(x, _y, z, n, u, v, 1, -1),
							GetAO(x, _y, z, n, u, v, 1, 1),
							GetAO(x, _y, z, n, u, v, -1, 1),
						};
						for (int i = 0; i < 4; i++)
						{
							colours.push_back(r);
							colours.push_back(g);
							colours.push_back(b);
							colours.push_back((unsigned char)(ao[i] * 255.0f));
						}
					}
				}
			}
		}

		Mesh mesh{};
		mesh.vertexCount = (int)indices.size();
		mesh.triangleCount = (int)indices.size() / 3;
		mesh.vertices = (float*)MemAlloc(indices.size() * 3 * sizeof(float));
		mesh.normals = (float*)MemAlloc(indices.size() * 3 * sizeof(float));
		mesh.colors = (unsigned char*)MemAlloc(indices.size() * 4 * sizeof(unsigned char));
		mesh.indices = nullptr;

		for (size_t i = 0; i < indices.size(); i++)
		{
			const unsigned int k = indices[i];
			for (int c = 0; c < 3; c++)
			{
				mesh.vertices[i * 3 + c] = vertices[k * 3 + c];
				mesh.normals[i * 3 + c] = normals[k * 3 + c];
			}
			for (int c = 0; c < 4; c++)
			{
				mesh.colors[i * 4 + c] = colours[k * 4 + c];
			}
		}

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
		if (subchunk.subchunk_mesh.vertexCount == 0) continue;

		subchunk.subchunk_model = LoadModelFromMesh(subchunk.subchunk_mesh);
		subchunk.subchunk_mesh = Mesh{};
		subchunk.subchunk_model.materials[0].shader = PostProcessing::GetTerrainShader();
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

void Chunk::SetNeighbour(int x, int y, Chunk* c)
{
	if (x == 0 && y == 1) neighbours[0] = c; // N (+z)
	else if (x == 0 && y == -1) neighbours[1] = c; // S (-z)
	else if (x == 1 && y == 0) neighbours[2] = c; // E (+x)
	else if (x == -1 && y == 0) neighbours[3] = c; // W (-x)
}

bool Chunk::IsSolidAt(int x, int y, int z)
{
	if (y < 0 || y >= subchunk_size * chunk_height_subchunks) return false;

	if (x < 0)
	{
		if (!neighbours[3]) return false;
		return neighbours[3]->IsSolidAt(x + subchunk_size, y, z);
	}

	if (x >= subchunk_size)
	{
		if (!neighbours[2]) return false;
		return neighbours[2]->IsSolidAt(x - subchunk_size, y, z);
	}

	if (z < 0)
	{
		if (!neighbours[1]) return false;
		return neighbours[1]->IsSolidAt(x, y, z + subchunk_size);
	}

	if (z >= subchunk_size)
	{
		if (!neighbours[0]) return false;
		return neighbours[0]->IsSolidAt(x, y, z - subchunk_size);
	}

	return subchunks[y / subchunk_size].blocks[x][y % subchunk_size][z].type != BLOCK_AIR;
}

float Chunk::GetAO(int x, int y, int z, const int n[3], const int u[3], const int v[3], int su, int sv)
{
	const int fx = x + n[0];
	const int fy = y + n[1];
	const int fz = z + n[2];

	const bool side1 = IsSolidAt(fx + u[0] * su, fy + u[1] * su, fz + u[2] * su);
	const bool side2 = IsSolidAt(fx + v[0] * sv, fy + v[1] * sv, fz + v[2] * sv);
	const bool corner = IsSolidAt(fx + u[0] * su + v[0] * sv,
		fy + u[1] * su + v[1] * sv,
		fz + u[2] * su + v[2] * sv);

	int level;
	if (side1 && side2) level = 3;
	else level = (int)side1 + (int)side2 + (int)corner;

	constexpr float table[4] = { 1.0f, 0.8f, 0.65f, 0.5f };
	return table[level];
}
