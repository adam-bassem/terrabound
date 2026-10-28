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
#include "resourcemanager.hpp"

namespace
{
	Texture2D world_texture;
	bool world_texture_loaded = false;
	//WorldDecorations featureDecorations = WorldDecorations();
}



Chunk::Chunk(Vector3 position, uint64_t _seed) : Position(position), seed(_seed)
{
	if (!world_texture_loaded)
	{
	//	Resource rsrc = Resources::LoadTextureFromResource(IDB_PNG9);
		world_texture = LoadTextureFromImage(LoadImage("res/tiles_atlas.png"));
	//	LoadTextureFromImage(LoadImageFromMemory(".png", (const unsigned char*)rsrc.Data, rsrc.Size));
		world_texture.mipmaps = 4;
		GenTextureMipmaps(&world_texture);
		SetTextureFilter(world_texture, TEXTURE_FILTER_POINT);
		SetTextureWrap(world_texture, TEXTURE_WRAP_CLAMP);
		world_texture_loaded = true;
	}

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

	FastNoiseLite coal_ore_noise;
	coal_ore_noise.SetNoiseType(FastNoiseLite::NoiseType_Value);
	coal_ore_noise.SetSeed((int)seed + 3247);
	coal_ore_noise.SetFrequency(0.005f);
	coal_ore_noise.SetFractalType(FastNoiseLite::FractalType_FBm);


	FastNoiseLite iron_ore_noise;
	iron_ore_noise.SetNoiseType(FastNoiseLite::NoiseType_Value);
	iron_ore_noise.SetSeed((int)seed + 2747);
	iron_ore_noise.SetFrequency(0.003f);
	iron_ore_noise.SetFractalType(FastNoiseLite::FractalType_FBm);

	FastNoiseLite diamond_ore_noise;
	iron_ore_noise.SetNoiseType(FastNoiseLite::NoiseType_Value);
	iron_ore_noise.SetSeed((int)seed + 934);
	iron_ore_noise.SetFrequency(0.003f);
	iron_ore_noise.SetFractalType(FastNoiseLite::FractalType_FBm);


	constexpr float terrain_base = 96.0f;
	constexpr float terrain_amp = 120.0f;

	//constexpr float terrain_base = 12.0f;
	//constexpr float terrain_amp = 12.0f;

	constexpr float cave_threshold = 0.18f;
	constexpr float cave_ore_threshold = 0.18f;

	

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
				else if (y <= 2) b = Block::GetBlock(BLOCK_BEDROCK);

	

				else b = Block::GetBlock(BLOCK_STONE);

				if (y < 1 || y > top - 5) continue;
				const float fy = (float)y;
				if (fabsf(cave_a_spag.GetNoise(wx, fy, wz)) >= cave_threshold) continue;
				if (fabsf(cave_b_spag.GetNoise(wx, fy, wz)) >= cave_threshold) continue;
			
				if (coal_ore_noise.GetNoise(wx, (float)y, wz) >= cave_ore_threshold) {
					b.type = BLOCK_COAL_ORE;
					continue;
				}
				if (y < 40) {
					if (iron_ore_noise.GetNoise(wx, (float)y, wz) >= 0.02) {
					b.type = BLOCK_GOLD_ORE;
					continue;
					}
				}
				else {
					if (iron_ore_noise.GetNoise(wx, (float)y, wz) >= 0.02) {
						b.type = BLOCK_IRON_ORE;
						continue;
					}
				}

				//Diamond ore
				if (y < 30 && y > 15) {
					if (diamond_ore_noise.GetNoise(wx, (float)y, wz) >= 0.02) {
						b.type = BLOCK_DIAMOND_ORE;
						continue;
					}
				}
				//Titanium Ore
				else if( y <20) {
					if (diamond_ore_noise.GetNoise(wx, (float)y, wz) >= 0.02) {
						if (b.type == BLOCK_DIAMOND_ORE || b.type == BLOCK_STONE) {
							b.type = BLOCK_TITANIUM_ORE;
						}
					
						continue;
					}
				}

				
			
		
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
		SetBlock(Vector3 { (float)x, (float)y, (float)z }, Block::GetBlock(type));
	};

	for (int dy = h - 1; dy <= h; dy++) //so this seems to place leaves
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
		set(bx, by + dy, bz, BLOCK_WOOD_LOG); //BLOCK_WOOD_LOG
	}
}

void Chunk::decorateSkyIsland(Vector3 base, std::mt19937_64& rng) {
	const int bx = (int)base.x;
	const int by = (int)base.y;
	const int bz = (int)base.z;
	const int max_y = subchunk_size * chunk_height_subchunks;

	std::uniform_int_distribution<int> heightRoll(100, 250);

	const int h = heightRoll(rng);

	std::uniform_int_distribution<int> mossyRockRoll(0, 100);
	std::uniform_int_distribution<int> airTileRoll(0, 50);

	block_type type;

	auto set = [&](int x, int y, int z, auto type)
		{
			if (x < 0 || x >= subchunk_size || z < 0 || z >= subchunk_size) return;
			if (y < 0 || y >= max_y) return;
			SetBlock(Vector3{ (float)x, (float)y, (float)z }, Block::GetBlock(type));
		};

	//Generate the Snow blocks or "cloud like blocks"
	//lower snow

	int baseHeight = heightmap[bx][bz] + h; //Fix for the sky islands generating in the ground


	for (int dx = -2; dx <= 3; dx++) {
			for (int dz = -3; dz <= 2; dz++)
			{
				set(bx + dx, baseHeight, bz + dz, BLOCK_SNOW);
			}
	}
	

	//lower snow
	//for (int dy = h - 10; dy <= h; dy++) //so this seems to place leaves
	//{
		for (int dx = -5; dx <= 5; dx++)
		{
			for (int dz = -5; dz <= 5; dz++)
			{
				set(bx + dx, baseHeight + 1, bz + dz, BLOCK_SNOW);
			}
		}
	//}

	//lower snow

	for (int dx = -9; dx <= 7; dx++) {
		for (int dz = -10; dz <= 9; dz++) {
			set(bx + dx, baseHeight + 2, bz + dz, BLOCK_SNOW);
		}
	}

	//Generate the dirt layers 
	for (int dy = baseHeight + 3; dy <= baseHeight + 7; dy++) { //so this seems to place leaves 
		for (int dx = -11; dx <= 9; dx++) {
			for (int dz = -12; dz <= 11; dz++) {
				set(bx + dx, dy, bz + dz, BLOCK_DIRT);
			}
		}
	}
	//Generate the grass platform
	for (int dy = baseHeight + 3; dy <= baseHeight + 6; dy++) //so this seems to place leaves
	{
		for (int dx = -9; dx <= 7; dx++)
		{
			for (int dz = -10; dz <= 9; dz++)
			{
				set(bx + dx, baseHeight + 8, bz + dz, BLOCK_GRASS);
			}
		}
	}

	//Generate the House
	for (int dy = (baseHeight + 9); dy <= (baseHeight + 13); dy++) {
		for (int dz = 0; dz <= 10; dz++) {
			for (int dx = 0; dx <= 10; dx++) {

				//Box Creation
				// Saving this for other box like structures like houses or underground dungeons
				//set(0 + dx, 60, 0 + dz, BLOCK_WOOD_PLANK);

				//set(0 + dx ,1 + dy, 0, BLOCK_ROCK);
				//set(10 ,1 + dy, 0 + dz, BLOCK_ROCK);
				//set(10 - dx ,1 + dy, 10, BLOCK_ROCK);
				//set(0 ,1 + dy, 10 - dz, BLOCK_ROCK);

				//set(0 + dx, 65, 0 + dz, BLOCK_ROCK);	

				int mossyRoll = mossyRockRoll(rng);
				int airRoll = airTileRoll(rng);

				//If it rolls at 5, 8, 63 or 47 then we set the tile to mossy rock
				if (mossyRoll == 5 || mossyRoll == 8 || mossyRoll == 63 || mossyRoll == 47) {
					type = BLOCK_MOSSY_ROCK;
				}
				//If this Rolls at 5, 10, 90 or 27 then we do not place a block / set tile to air
				else if (airRoll == 5 || airRoll == 10 || airRoll == 90 || airRoll == 27) {
					type = BLOCK_AIR;
				}
				//else none of that has rolled so we just place a rock tile
				else {
					type = BLOCK_ROCK;
				}

				//This can be used for random variations of this
				//if (mossyRoll % 5 == 0) {
				//	type = BLOCK_MOSSY_ROCK;
				//}
				//else if (mossyRoll % 60 == 0) {
				//	type = BLOCK_AIR;
				//}
				//else {
				//	type = BLOCK_ROCK;
				//}

				const int obx = bx;
				const int obz = bz;
				set(obx + dx, baseHeight + 8, obz + dz, BLOCK_WOOD_PLANK);

				set(obx + dx, dy, obz, type);
				set(obx + 10, dy, obz + dz, type);
				set((obx + 10) - dx, dy, obz + 10, type);
				set(obx, dy, (obz + 10) - dz, type);

				set(obx + dx, baseHeight + 14, obz + dz, type);
			}
		}
	}

}

void Chunk::DecoratePineTree(Vector3 base, std::mt19937_64& rng) {
	const int bx = (int)base.x;
	const int by = (int)base.y;
	const int bz = (int)base.z;
	const int max_y = subchunk_size * chunk_height_subchunks;

	std::uniform_int_distribution<int> height_roll(10, 30);
//	std::uniform_int_distribution<int> branchRoll(2, 6);

//	std::uniform_int_distribution<int> negBranchRoll(2, 6);


	const int h = height_roll(rng);

	std::uniform_int_distribution<int> branchHeightRoll(h - (h - 15), h - (h - 5));

	//const int br = branchRoll(rng);
	//const int nbr = negBranchRoll(rng);

	const int brh = branchHeightRoll(rng);



	auto set = [&](int x, int y, int z, auto type)
		{
			if (x < 0 || x >= subchunk_size || z < 0 || z >= subchunk_size) return;
			if (y < 0 || y >= max_y) return;
			SetBlock(Vector3{ (float)x, (float)y, (float)z }, Block::GetBlock(type));
		};

	//Upper 5x5 leaves
	for (int dy = h - 10; dy <= h; dy++) //so this seems to place leaves
	{
		for (int dx = -2; dx <= 2; dx++)
		{
			for (int dz = -2; dz <= 2; dz++)
			{
				set(bx + dx, by + dy , bz + dz, BLOCK_PINE_LEAVES);
			}
		}
	}

	//Top 3x3 leaves
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dz = -1; dz <= 1; dz++)
		{
			for (int dy = h; dy < h + 5; dy++) {
				set(bx + dx, by + dy, bz + dz, BLOCK_PINE_LEAVES);
			}
		
		}
	}


	//Lower 7x7 leaves
	for (int dx = -3; dx <= 3; dx++)
	{
		for (int dz = -3; dz <= 3; dz++)
		{
			int bdy = by + h + 1;
			for (int idx = 8; idx > 0; idx--) {
				set(bx + dx, (bdy - 5) - idx, bz + dz, BLOCK_PINE_LEAVES);
			}
		
		}
	}

	for (int dx = -7; dx <= 7; dx++)
	{
		for (int dz = -7; dz <= 7; dz++)
		{
			int bdy = by + h + 1;
			for (int idx = 8; idx > 0; idx--) {

				set((bx + dx), brh, (bz + dz ), BLOCK_PINE_LEAVES);
			}

		}
	}



	// Trunk / Branches last, so that the leaves don't cover it
	for (int dy = 0; dy < h; dy++) {
		set(bx, by + dy, bz, BLOCK_PINE_LOG);
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
		SetBlock(Vector3 { (float)x, (float)y, (float)z }, Block::GetBlock(type));
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
	std::uniform_int_distribution<int> roll2(-1000.000f, 1000.000f);
	std::uniform_int_distribution<int> roll3(-50.000f, 70.000f);
	const float r2 = roll2(rng);


	for (int x = 0; x < subchunk_size; x++) {
		for (int z = 0; z < subchunk_size; z++)	{
			const float r = roll(rng);
			const float r3 = roll3(rng);

			if (r < 7.0f) 
				DecorateTree(Vector3 { (float)x, (float)heightmap[x][z], (float)z }, rng); // 0.8% tree
			else if (r < 7.2f) 
				DecoratePineTree(Vector3{ (float)x, (float)heightmap[x][z], (float)z }, rng); // 0.2% pine
			else if (r < 13.0f) 
				DecorateBush(Vector3 { (float)x, (float)heightmap[x][z], (float)z }); // 1.6% bush
			else if (r < 86.0f) 
				SetBlock(Vector3 { (float)x, (float)heightmap[x][z], (float)z }, Block::GetBlock(BLOCK_SHORT_GRASS)); // 6% short grass
			else if (r < 120.0f)
				SetBlock(Vector3{ (float)x, (float)heightmap[x][z], (float)z }, Block::GetBlock(BLOCK_RED_FLOWER)); // 10.5% red flower
			else if (r < 130.0f) 
				SetBlock(Vector3{ (float)x, (float)heightmap[x][z], (float)z }, Block::GetBlock(BLOCK_OXEYE_DAISY)); // 12.5% oxeye daisy flower
			
			if (r2 == r3)
				decorateSkyIsland(Vector3{ (float)x, (float)heightmap[x][z], (float)z }, rng); // 1.6% bush
		}
	}
}

static void ReleaseSubchunk(Subchunk& subchunk)
{
	if (subchunk.subchunk_model.meshes)
	{
		// the atlas is shared between chunks, so detach it before UnloadModel frees the material textures
		if (subchunk.subchunk_model.materials)
		{
			subchunk.subchunk_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Texture2D { };
		}
		UnloadModel(subchunk.subchunk_model);
	}
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

	// world_texture is shared by every chunk, so it is not unloaded here
}

#define CULL_NORTH 0b00000001
#define CULL_SOUTH 0b00000010
#define CULL_EAST 0b00000100
#define CULL_WEST 0b00001000
#define CULL_UP 0b00010000
#define CULL_DOWN 0b00100000

void Chunk::GenerateMesh() {
	for (int subchunk_i = 0; subchunk_i < chunk_height_subchunks; subchunk_i++) {
		Subchunk& subchunk = subchunks[subchunk_i];

		if (!subchunk.is_dirty) continue;
		subchunk.is_dirty = false;

		ReleaseSubchunk(subchunk);
		has_model = false;

		std::vector<float> vertices;
		std::vector<float> normals;
		std::vector<float> texcoords;
		std::vector<unsigned char> colours;
		std::vector<unsigned int> indices;

		for (int x = 0; x < subchunk_size; x++) {
			for (int y = 0; y < subchunk_size; y++) {
				for (int z = 0; z < subchunk_size; z++) {
					block_data& this_block = subchunk.blocks[x][y][z];



					if (this_block.type == BLOCK_AIR) continue;

					const int _y = y + (subchunk_size * subchunk_i);

					uint8_t cull = 0;
					if (IsSolidAt(x - 1, _y, z)) cull |= CULL_WEST;
					if (IsSolidAt(x + 1, _y, z)) cull |= CULL_EAST;
					if (IsSolidAt(x, _y - 1, z)) cull |= CULL_DOWN;
					if (IsSolidAt(x, _y + 1, z)) cull |= CULL_UP;
					if (IsSolidAt(x, _y, z - 1)) cull |= CULL_SOUTH;
					if (IsSolidAt(x, _y, z + 1)) cull |= CULL_NORTH;

					const block_mesh& mesh = Block::GetBlockMesh(this_block);

					for (size_t q = 0; q < mesh.cull.size(); q++) {
						if (cull & mesh.cull[q]) continue;

						const float* nrm = &mesh.normals[q * 12];
						const int n[3] = { (int)nrm[0], (int)nrm[1], (int)nrm[2] };

						int u[3], v[3];
						if (n[0] != 0) {
							u[0] = 0; u[1] = 1; u[2] = 0;
							v[0] = 0; v[1] = 0; v[2] = 1;
						}
						else if (n[1] != 0) {
							u[0] = 1; u[1] = 0; u[2] = 0;
							v[0] = 0; v[1] = 0; v[2] = 1;
						} else {
							u[0] = 1; u[1] = 0; u[2] = 0;
							v[0] = 0; v[1] = 1; v[2] = 0;
						}

						const unsigned int first = (unsigned int)(vertices.size() / 3);

						for (int k = 0; k < 4; k++) {
							const size_t vi = (q * 4 + k) * 3;
							const float lx = mesh.vertices[vi];
							const float ly = mesh.vertices[vi + 1];
							const float lz = mesh.vertices[vi + 2];

							vertices.push_back(x + lx);
							vertices.push_back(_y + ly);
							vertices.push_back(z + lz);

							normals.push_back(nrm[0]);
							normals.push_back(nrm[1]);
							normals.push_back(nrm[2]);

							const UV& uv = mesh.texcoords[q * 4 + k];
							texcoords.push_back(uv.x1);
							texcoords.push_back(uv.y1);

							const int su = (lx * u[0] + ly * u[1] + lz * u[2]) >= 0.5f ? 1 : -1;
							const int sv = (lx * v[0] + ly * v[1] + lz * v[2]) >= 0.5f ? 1 : -1;

							const float ao = mesh.allow_ao ? GetAO(x, _y, z, n, u, v, su, sv) : 1.0f;

							// colour comes from the atlas, so rgb is white and alpha carries AO
							colours.push_back(255);
							colours.push_back(255);
							colours.push_back(255);
							colours.push_back((unsigned char)(ao * 255.0f));
						}

						for (int i = 0; i < 6; i++) {
							indices.push_back(first + (mesh.indices[q * 6 + i] - (unsigned int)(q * 4)));
						}
					}
				}
			}
		}

		Mesh mesh { };
		mesh.vertexCount = (int)indices.size();
		mesh.triangleCount = (int)indices.size() / 3;
		mesh.vertices = (float*)MemAlloc(indices.size() * 3 * sizeof(float));
		mesh.normals = (float*)MemAlloc(indices.size() * 3 * sizeof(float));
		mesh.texcoords = (float*)MemAlloc(indices.size() * 2 * sizeof(float));
		mesh.colors = (unsigned char*)MemAlloc(indices.size() * 4 * sizeof(unsigned char));
		mesh.indices = nullptr;

		for (size_t i = 0; i < indices.size(); i++){
			const unsigned int k = indices[i];
			for (int c = 0; c < 3; c++) {
				mesh.vertices[i * 3 + c] = vertices[k * 3 + c];
				mesh.normals[i * 3 + c] = normals[k * 3 + c];
			}
			for (int c = 0; c < 2; c++) {
				mesh.texcoords[i * 2 + c] = texcoords[k * 2 + c];
			}
			for (int c = 0; c < 4; c++) {
				mesh.colors[i * 4 + c] = colours[k * 4 + c];
			}
		}

		subchunk.subchunk_mesh = mesh;
		UploadMesh(&subchunk.subchunk_mesh, false);
	}

	has_mesh = true;
}

void Chunk::GenerateModel()
{
	for (int i = 0; i < chunk_height_subchunks; i++) {
		Subchunk& subchunk = subchunks[i];

		if (subchunk.subchunk_mesh.vertexCount == 0) continue;

		subchunk.subchunk_model = LoadModelFromMesh(subchunk.subchunk_mesh);
		subchunk.subchunk_mesh = Mesh { };
		subchunk.subchunk_model.materials[0].shader = PostProcessing::GetTerrainShader();
		subchunk.subchunk_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = world_texture;
	}

	has_model = true;
}

Model* Chunk::GetModel(int subchunk)
{
	if (subchunk < 0 || subchunk >= chunk_height_subchunks) return nullptr;
	if (!subchunks[subchunk].subchunk_model.meshes) return nullptr;
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

void Chunk::SetBlock(Vector3 Relative, block_data data)
{
	const int x = (int)Relative.x;
	const int y = (int)Relative.y;
	const int z = (int)Relative.z;

	if (x < 0 || x >= subchunk_size || z < 0 || z >= subchunk_size) return;
	if (y < 0 || y >= subchunk_size * chunk_height_subchunks) return;

	subchunks[y / subchunk_size].blocks[x][y % subchunk_size][z] = data;

	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			for (int dz = -1; dz <= 1; dz++)
			{
				MarkDirtyAt(x + dx, y + dy, z + dz);
			}
		}
	}
}

void Chunk::MarkDirtyAt(int x, int y, int z)
{
	if (y < 0 || y >= subchunk_size * chunk_height_subchunks) return;

	if (x < 0)
	{
		if (neighbours[3]) neighbours[3]->MarkDirtyAt(x + subchunk_size, y, z);
		return;
	}

	if (x >= subchunk_size)
	{
		if (neighbours[2]) neighbours[2]->MarkDirtyAt(x - subchunk_size, y, z);
		return;
	}

	if (z < 0)
	{
		if (neighbours[1]) neighbours[1]->MarkDirtyAt(x, y, z + subchunk_size);
		return;
	}

	if (z >= subchunk_size)
	{
		if (neighbours[0]) neighbours[0]->MarkDirtyAt(x, y, z - subchunk_size);
		return;
	}

	subchunks[y / subchunk_size].is_dirty = true;
}

void Chunk::SetNeighbour(int x, int y, Chunk* c) {
	if (x == 0 && y == 1) neighbours[0] = c; // N (+z)
	else if (x == 0 && y == -1) neighbours[1] = c; // S (-z)
	else if (x == 1 && y == 0) neighbours[2] = c; // E (+x)
	else if (x == -1 && y == 0) neighbours[3] = c; // W (-x)

	for (Subchunk& s : subchunks) s.is_dirty = true;
}

bool Chunk::IsSolidAt(int x, int y, int z, bool transparent_is_solid) {
	if (y < 0 || y >= subchunk_size * chunk_height_subchunks) return false;

	if (x < 0) {
		if (!neighbours[3]) return false;
		return neighbours[3]->IsSolidAt(x + subchunk_size, y, z, transparent_is_solid);
	}

	if (x >= subchunk_size)	{
		if (!neighbours[2]) return false;
		return neighbours[2]->IsSolidAt(x - subchunk_size, y, z, transparent_is_solid);
	}

	if (z < 0) {
		if (!neighbours[1]) return false;
		return neighbours[1]->IsSolidAt(x, y, z + subchunk_size, transparent_is_solid);
	}

	if (z >= subchunk_size)	{
		if (!neighbours[0]) return false;
		return neighbours[0]->IsSolidAt(x, y, z - subchunk_size, transparent_is_solid);
	}

	block_data& b = subchunks[y / subchunk_size].blocks[x][y % subchunk_size][z];
	return b.type != BLOCK_AIR && (transparent_is_solid || !b.transparent);
}

float Chunk::GetAO(int x, int y, int z, const int n[3], const int u[3], const int v[3], int su, int sv) {
	const int fx = x + n[0];
	const int fy = y + n[1];
	const int fz = z + n[2];

	const bool side1 = IsSolidAt(fx + u[0] * su, fy + u[1] * su, fz + u[2] * su, true);
	const bool side2 = IsSolidAt(fx + v[0] * sv, fy + v[1] * sv, fz + v[2] * sv, true);
	const bool corner = IsSolidAt(fx + u[0] * su + v[0] * sv,
		fy + u[1] * su + v[1] * sv,
		fz + u[2] * su + v[2] * sv, true);

	int level;
	if (side1 && side2) level = 3;
	else level = (int)side1 + (int)side2 + (int)corner;

	constexpr float table[4] = { 1.0f, 0.8f, 0.65f, 0.5f };
	return table[level];
}