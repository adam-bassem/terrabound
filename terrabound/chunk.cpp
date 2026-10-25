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
#include <iostream>


Chunk::Chunk(Vector3 position, uint64_t _seed) : Position(position), seed(_seed)
{
	FastNoiseLite heightmap_noise;
	heightmap_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	heightmap_noise.SetSeed((int)seed);
	heightmap_noise.SetFrequency(0.005f);
	heightmap_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	heightmap_noise.SetFractalOctaves(8);
	heightmap_noise.SetFractalLacunarity(1.85f);
	heightmap_noise.SetFractalGain(0.37f);
	heightmap_noise.SetFractalWeightedStrength(0.11f);
	heightmap_noise.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
	heightmap_noise.SetDomainWarpAmp(22.0f);

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

	
	//constexpr float terrain_base = 96.0f; //Old value
	//constexpr float terrain_base = 10.0f; //Currently my favorite


	//Not entirely sure what this does but from my testing,
	// the lower this number the more close each mountain is,
	// else each mountain is much further the higher this number is -Alex

	constexpr float terrain_base = 20.0f; 
	
	//constexpr float terrain_amp = 220.0f; //Higher number, Steeper terrain -Alex
	constexpr float terrain_amp = 105.0f; //Higher number, Steeper terrain, Lower, more flatter -Alex

	//constexpr float cave_threshold = 0.18f; //Old value

	///Lower number, less and smaller caves. Higher Number,  more and bigger caves. -Alex
	constexpr float cave_threshold = 0.15f; 

	// fill layers and carve caves
	for (int x = 0; x < subchunk_size; x++)
	{
		for (int z = 0; z < subchunk_size; z++)
		{
			float n = heightmap_noise.GetNoise(position.x + x, position.z + z);
			float h = std::pow(std::tanh(n * 2.0f) * 0.5f + 0.5f, 1.5f);
			heightmap[x][z] = (int)(terrain_base + h * terrain_amp);

			const int top = heightmap[x][z];
			const float wx = position.x + x;
			const float wz = position.z + z;

			for (int y = 0; y < top; y++)
			{
				block_data& b = subchunks[y / subchunk_size].blocks[x][y % subchunk_size][z];

				if (y == top - 1) b = Block::GetBlock(BLOCK_GRASS);
				else if (y >= top - 4) b = Block::GetBlock(BLOCK_DIRT);
				else b = Block::GetBlock(BLOCK_STONE);

				uint32_t h = (uint32_t)(position.x + x) * 0x27d4eb2dU //Is this hex even needed??? -Alex
					^ (uint32_t)y * 0x165667b1U
					^ (uint32_t)(position.z + z) * 0x9e3779b9U
					^ (uint32_t)seed;
				h ^= h >> 15;
				h *= 0x2c1b3c6dU;
				h ^= h >> 12;
				b.state = (block_state)(h % 3);

				if (y < 1 || y > top - 5) continue;

				const float fy = (float)y;
				if (fabsf(cave_a_spag.GetNoise(wx, fy, wz)) >= cave_threshold) continue;
				if (fabsf(cave_b_spag.GetNoise(wx, fy, wz)) >= cave_threshold) continue;

				b.type = BLOCK_AIR;
			}
		}
	}
}

// TODO: chunk_decorate.cpp
void Chunk::DecorateTree(Vector3 base, std::mt19937_64& rng)
{
	const int bx = (int)base.x;
	const int by = (int)base.y;
	const int bz = (int)base.z;
	const int max_y = subchunk_size * chunk_height_subchunks;

	std::uniform_int_distribution<int> height_roll(4, 6);
	const int h = height_roll(rng);

	auto set = [&](int x, int y, int z, auto type)
	{
		if (x < 0 || x >= subchunk_size || z < 0 || z >= subchunk_size) return;
		if (y < 0 || y >= max_y) return;
		GetBlock(Vector3 { (float)x, (float)y, (float)z }) = Block::GetBlock(type);
	};

	for (int dy = h - 1; dy <= h; dy++)
	{
		for (int dx = -2; dx <= 2; dx++)
		{
			for (int dz = -2; dz <= 2; dz++)
			{
				set(bx + dx, by + dy, bz + dz, BLOCK_TREE_LEAVES);
			}
		}
	}

	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dz = -1; dz <= 1; dz++)
		{
			set(bx + dx, by + h + 1, bz + dz, BLOCK_TREE_LEAVES);
		}
	}

	// Trunk last, so the leaves don't cover it
	for (int dy = 0; dy < h; dy++)
	{
		set(bx, by + dy, bz, BLOCK_WOOD_LOG);
	}
}

void Chunk::DecorateBush(Vector3 base)
{
	const int bx = (int)base.x;
	const int by = (int)base.y;
	const int bz = (int)base.z;
	const int max_y = subchunk_size * chunk_height_subchunks;

	auto set = [&](int x, int y, int z, auto type)
	{
		if (x < 0 || x >= subchunk_size || z < 0 || z >= subchunk_size) return;
		if (y < 0 || y >= max_y) return;
		GetBlock(Vector3 { (float)x, (float)y, (float)z }) = Block::GetBlock(type);
	};

	// 3x3 at the surface level, one block on top
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dz = -1; dz <= 1; dz++)
		{
			set(bx + dx, by, bz + dz, BLOCK_BUSH_LEAVES);
		}
	}

	set(bx, by + 1, bz, BLOCK_BUSH_LEAVES);
}

void Chunk::DecorateSecondPass()
{
	if (decorated) return;
	decorated = true;

	std::mt19937_64 rng((uint64_t)seed
		^ ((uint64_t)(int64_t)Position.x * 0x9E3779B97F4A7C15ull)
		^ ((uint64_t)(int64_t)Position.z * 0xC2B2AE3D27D4EB4Full));
	std::uniform_int_distribution<int> roll(0, 999);

	for (int x = 0; x < subchunk_size; x++)
	{
		for (int z = 0; z < subchunk_size; z++)
		{
			const int r = roll(rng);

			if (r < 8) DecorateTree(Vector3 { (float)x, heightmap[x][z], (float)z }, rng); // 0.8% tree
			else if (r < 32) DecorateBush(Vector3 { (float)x, heightmap[x][z], (float)z }); // 3.2% bush
			else if (r < 60) GetBlock(Vector3 { (float)x, heightmap[x][z], (float)z }) = Block::GetBlock(BLOCK_SHORT_GRASS); // 6% short grass
		}
	}
}

static void ReleaseSubchunk(Subchunk& subchunk)
{
	if (subchunk.subchunk_model.meshes) UnloadModel(subchunk.subchunk_model);
	subchunk.subchunk_model = Model { };

	if (subchunk.subchunk_mesh.vertices) UnloadMesh(subchunk.subchunk_mesh);
	subchunk.subchunk_mesh = Mesh { };
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
		std::vector<float> normals;
		std::vector<float> texCoords;
		std::vector<unsigned char> colours;
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

					const int _y = y + (subchunk_size * subchunk_i);
			
					// colour comes from the block, not the mesh
					const uint32_t colour = Block::GetColour(this_block);
					const uint8_t r = (colour >> 16) & 0xFF;
					const uint8_t g = (colour >> 8) & 0xFF;
					const uint8_t b = colour & 0xFF;


				
					Block::getTileTextures(this_block);
					uint8_t cull = 0;
					if (IsSolidAt(x - 1, _y, z)) cull |= CULL_WEST;
					if (IsSolidAt(x + 1, _y, z)) cull |= CULL_EAST;
					if (IsSolidAt(x, _y - 1, z)) cull |= CULL_DOWN;
					if (IsSolidAt(x, _y + 1, z)) cull |= CULL_UP;
					if (IsSolidAt(x, _y, z - 1)) cull |= CULL_SOUTH;
					if (IsSolidAt(x, _y, z + 1)) cull |= CULL_NORTH;

					
					const block_mesh &mesh = Block::GetBlockMesh(this_block);
				
					for (size_t q = 0; q < mesh.cull.size(); q++)
					{
						if (cull & mesh.cull[q]) continue;

						const float* nrm = &mesh.normals[q * 12];
						const int n[3] = { (int)nrm[0], (int)nrm[1], (int)nrm[2] };

						// the two directions across the face, the normal doesn't point along them
						int u[3], v[3];
						if (n[0] != 0)
						{
							u[0] = 0; u[1] = 1; u[2] = 0;
							v[0] = 0; v[1] = 0; v[2] = 1;
						}
						else if (n[1] != 0)
						{
							u[0] = 1; u[1] = 0; u[2] = 0;
							v[0] = 0; v[1] = 0; v[2] = 1;
						}
						else
						{
							u[0] = 1; u[1] = 0; u[2] = 0;
							v[0] = 0; v[1] = 1; v[2] = 0;
						}

						const unsigned int first = (unsigned int)(vertices.size() / 3);

						for (int k = 0; k < 4; k++)
						{
							const size_t vi = (q * 4 + k) * 3;
							const size_t vi2 = (q * 4 + k) * 2;

							const float lx = mesh.vertices[vi];
							const float ly = mesh.vertices[vi + 1];
							const float lz = mesh.vertices[vi + 2];

					
							float u2 = mesh.texCoords[vi2];
							float v2 = mesh.texCoords[vi2 + 1];

						//	std::cout << u2 << "\n";

							vertices.push_back(x + lx);
							vertices.push_back(_y + ly);
							vertices.push_back(z + lz);

							normals.push_back(nrm[0]);
							normals.push_back(nrm[1]);
							normals.push_back(nrm[2]);

							texCoords.push_back(u2);
							texCoords.push_back(v2);

							

							// which end of each in-plane axis this corner sits on
							const int su = (lx * u[0] + ly * u[1] + lz * u[2]) >= 0.5f ? 1 : -1;
							const int sv = (lx * v[0] + ly * v[1] + lz * v[2]) >= 0.5f ? 1 : -1;

							const float ao = mesh.allow_ao ? GetAO(x, _y, z, n, u, v, su, sv) : 1.0f;

							colours.push_back(r); //absolutely horrible solution to the textures not changing but ehh works for now
							colours.push_back(g);
							colours.push_back(b);
							colours.push_back((unsigned char)(ao * 255.0f));
							

						}	
						// six indices per quad, rebased onto this quad's first vertex

					
						for (int i = 0; i < 6; i++)
						{
							indices.push_back(first + (mesh.indices[q * 6 + i] - (unsigned int)(q * 4)));
						}
					
					}// delete[] uvs; //If i don't call this then an addition 2gb of ram will be taken, don't ask me how lol -Alex;
				}
					
			}
			
		}

		// expand indices into plain triangles, no index is limited to 16 bits
		Mesh mesh { };
		mesh.vertexCount = (int)indices.size();
		mesh.triangleCount = (int)indices.size() / 3;
		mesh.vertices = (float*)MemAlloc(indices.size() * 3 * sizeof(float));
		mesh.texcoords = (float*)MemAlloc(texCoords.size() * 2 * sizeof(float));
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

			for (int t = 0; t < 2; t++) { //This works perfectly fine, do not touch this when trying to debug texture issues! - Alex
				mesh.texcoords[i * 2 + t] = texCoords[k * 2 + t];
			}
		
		}
		//texCoords.clear();
		subchunk.subchunk_mesh = mesh;
		UploadMesh(&subchunk.subchunk_mesh, false);
		//texCoords.clear();
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
		subchunk.subchunk_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("tiles_atlas.png");
		subchunk.subchunk_mesh = Mesh { };
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

	block_data& b = subchunks[y / subchunk_size].blocks[x][y % subchunk_size][z];
	return b.type != BLOCK_AIR && !b.transparent;
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
