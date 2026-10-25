/*
 * ============================================================================
 * File        : chunk.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Chunk class, contains block data for full chunk, and mesh data and generation
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#include "chunk.hpp"
#include <vector>
#include "FastNoiseLite.hpp"
#include <algorithm>

Chunk::Chunk(Vector3 position, uint64_t _seed) : Position(position), seed(_seed)
{
	FastNoiseLite heightmap;
	heightmap.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	heightmap.SetSeed(seed);
	heightmap.SetFrequency(0.005f);
	heightmap.SetFractalType(FastNoiseLite::FractalType_FBm);
	heightmap.SetFractalOctaves(8);
	heightmap.SetFractalLacunarity(1.85f);
	heightmap.SetFractalGain(0.37f);
	heightmap.SetFractalWeightedStrength(0.11f);
	heightmap.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
	heightmap.SetDomainWarpAmp(22.0f);



	
	for (int x = 0; x < subchunk_size; x++)
		for (int z = 0; z < subchunk_size; z++)
		{
			constexpr float terrain_base = 96.0f;
			constexpr float terrain_amp = 420.0f;

			float n = heightmap.GetNoise(position.x + x, position.z + z);
			float h = std::tanh(n * 2.0f) * 0.5f + 0.5f;
			h = std::pow(h, 1.5f);
			int height = (int)(terrain_base + h * terrain_amp);


			int cx = x;
			int cz = z;

			for (int y = 0; y < height; y++)
			{
				block_data& b = GetBlock({ (float)x, (float)y, (float)z });
				b.type = BLOCK_FILLED;


				if (y >= 260) {
					b.state = BLOCK_SNOW;
				}
				else if (y > 150 && y < 260) {
					b.state = BLOCK_STONE;
				}

				else if (y > 140 && y <= 150) {
					b.state = BLOCK_GRASS;
				}

				else if (y > 135 && y <= 140) {
					b.state = BLOCK_WATER;
				}
				

				else if (y == 135 ) {
					b.state = BLOCK_BEDROCK;
				}

				else if (y > 80 && y < 135) {
					b.state = BLOCK_DIRT;
				}

				else {
					b.type = BLOCK_FILLED;
					b.state = BLOCK_STONE;
				}

				//Random Air pockets
				if (std::rand() * 3 == x || std::rand() * 3 == z) {
					//y--;
					cx--;
					cz++;


					if (b.state == BLOCK_STONE) {
						b.type = BLOCK_AIR;
					}

					if (cz > 3) {
						b.type = BLOCK_AIR;
					}
					

					
				}
				else {
					b.type = BLOCK_FILLED;
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


//Culling Directions
#define CULL_NORTH 0b00000001
#define CULL_SOUTH 0b00000010
#define CULL_EAST  0b00000100
#define CULL_WEST  0b00001000
#define CULL_UP    0b00010000
#define CULL_DOWN  0b00100000

//Generate the chunk meshes
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
		std::vector<float> texCoords;

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

					uint8_t r = 0xFF, g = 0xFF, b = 0xFF, a = 0xFF;

					float u[24];
					float v[24];


					float u0 = 0, v0 = 0, u1 = 1, v1 = 0, u2 = 1, v2 = 1, u3 = 0, v3 = 1; //Testing


					//float u4 = 0, v4 = 0, u5 = 1, v5 = 0, u6 = 1, v6 = 1, u7 = 0, v7 = 1;
					//float u8 = 0, v8 = 0, u9 = 1, v9 = 0, u10 = 1, v10 = 1, u11 = 0, v11 = 1;
					//float u12 = 0, v12 = 0, u13 = 1, v13 = 0, u14 = 1, v14 = 1, u15 = 0, v15 = 1;

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

					case BLOCK_GRASS : {
						//bottom left corner
					//	u0 = 0.00390625f;
					//	v0 = 0.00390625f;

						//bottom right corner
				//		u1 = 0.0625;
				//		v1 = 0.00390625f;

						
						//top right corner
					//	u2 = 0.0625;
				//		v2 = 0.0625;
						//top left corner
				//		u3 = 0.00390625f;
				//		v3 = 0.0625;

						//New
						// 0.00390625
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

						break;
					}

					case BLOCK_WATER : {

					 //REMEMBER to start from 0.00390625f, (pixel 0, 0) then multiply this value by how far your texture starts and ends in pixels
						//Old
						////bottom left corner
						//u0 = 0.75f;
						//v0 = 0.0625f;

						////bottom right corner
						//u1 = 0.8125f;
						//v1 = 0.0625f; 

						////top right corner
						//u2 = 0.8125f;
						//v2 = 0.00390625f;

						////top left corner
						//u3 = 0.75f;
						//v3 = 0.00390625f;

						//New
						// 
						//North
						u[0] = 0.75f;
						v[0] = 0.0625f;

						u[1] = 0.8125f;
						v[1] = 0.0625f;

						u[2] = 0.8125f;
						v[2] = 0.00390625f;

						u[3] = 0.75f;
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


						break;
					}

					case BLOCK_BEDROCK : {
						//REMEMBER to start from 0.00390625f, (pixel 0, 0) then multiply this value by how far your texture starts and ends in pixels

						//bottom left corner
						//u0 = 0.6875;
						//v0 = 0.0625f;

						//bottom right corner
					//	u1 = 0.75f;
					//	v1 = 0.0625f;


						//top right corner
					//	u2 = 0.75f;
					//	v2 = 0.00390625f;

						//top left corner
					//	u3 = 0.6875;
					//	v3 = 0.00390625f;

						//New
					
						//North
						//bottom left corner
						u[0] = 0.6875;
						v[0] = 0.0625f;
						//bottom right corner
						u[1] = 0.75f;
						v[1] = 0.0625f;
						//top right corner
						u[2] = 0.75f;
						v[2] = 0.00390625f;
						//top left corner
						u[3] = 0.6875;
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
						break;
					}

					case BLOCK_SNOW : {

						//REMEMBER to start from 0.00390625f, (pixel 0, 0) then multiply this value by how far your texture starts and ends in pixels

						//bottom left corner
					//	u0 = 0.625f;
						//v0 = 0.0625f;

						//bottom right corner
					//	u1 = 0.6875f;
					//	v1 = 0.0625f;


						//top right corner
					//	u2 = 0.6875f;
					//	v2 = 0.00390625f;

						//top left corner
					//	u3 = 0.625f;
						//v3 = 0.00390625f;

						//New

						//North
						//bottom left corner
						u[0] = 0.625f;
						v[0] = 0.0625f;
						//bottom right corner
						u[1] = 0.6875f;
						v[1] = 0.0625f;
						//top right corner
						u[2] = 0.6875f;
						v[2] = 0.00390625f;
						//top left corner
						u[3] = 0.625f;
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

						break;
					}

					case BLOCK_STONE : {

					   //REMEMBER to start from 0.00390625f, (pixel 0, 0) then multiply this value by how far your texture starts and ends in pixels

					//	//bottom left corner
					//	u0 = 0.1875;
						//v0 = 0.0625f;

						//bottom right corner
					//	u1 = 0.25;
					//	v1 = 0.0625f;


						//top right corner
					//	u2 = 0.25;
					//	v2 = 0.00390625f;

						//top left corner
					//	u3 = 0.1875;
					//	v3 = 0.00390625f;

						//New

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

						break;
					}

					case BLOCK_DIRT : {
						//REMEMBER to start from 0.00390625f, (pixel 0, 0) then multiply this value by how far your texture starts and ends in pixels

						//bottom left corner
					//	u0 = 0.125f;
					//	v0 = 0.0625;

						//bottom right corner
					//	u1 = 0.1875;
					//	v1 = 0.0625;

						//top right corner
					///	u2 = 0.1875;
					//	v2 = 0.00390625f;
						//top left corner
					//	u3 = 0.125f;
						//v3 = 0.00390625f;

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
						break;
					}

					default:
					{
						break;
					}
					}

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


						//North face UV's

						//Old
						
						//texCoords.push_back(u0);
						//texCoords.push_back(v0);

						//texCoords.push_back(u1);
						//texCoords.push_back(v1);

						//texCoords.push_back(u2);
						//texCoords.push_back(v2);

						//texCoords.push_back(u3);
						//texCoords.push_back(v3);


						//New

						texCoords.push_back(u[0]);
						texCoords.push_back(v[0]);

						texCoords.push_back(u[1]);
						texCoords.push_back(v[1]);

						texCoords.push_back(u[2]);
						texCoords.push_back(v[2]);

						texCoords.push_back(u[3]);
						texCoords.push_back(v[3]);

						//North face UVs


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




						///South face UVs

						//Old
						//texCoords.push_back(u0);
						//texCoords.push_back(v0);

						//texCoords.push_back(u1);
						//texCoords.push_back(v1);

						//texCoords.push_back(u2);
						//texCoords.push_back(v2);

						//texCoords.push_back(u3);
						//texCoords.push_back(v3);
						

						//New
						texCoords.push_back(u[4]);
						texCoords.push_back(v[4]);

						texCoords.push_back(u[5]);
						texCoords.push_back(v[5]);

						texCoords.push_back(u[6]);
						texCoords.push_back(v[6]);

						texCoords.push_back(u[7]);
						texCoords.push_back(v[7]);

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

						//East Face UVs
						//Old
						//texCoords.push_back(u0);
						//texCoords.push_back(v0);

						//texCoords.push_back(u1);
						//texCoords.push_back(v1);

						//texCoords.push_back(u2);
						//texCoords.push_back(v2);

						//texCoords.push_back(u3);
						//texCoords.push_back(v3);
						//New
	

						texCoords.push_back(u[8]);
						texCoords.push_back(v[8]);

						texCoords.push_back(u[9]);
						texCoords.push_back(v[9]);

						texCoords.push_back(u[10]);
						texCoords.push_back(v[10]);

						texCoords.push_back(u[11]);
						texCoords.push_back(v[11]);




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


						//West fasce UVs
						//Old
						//texCoords.push_back(u0);
						//texCoords.push_back(v0);

						//texCoords.push_back(u1);
						//texCoords.push_back(v1);

						//texCoords.push_back(u2);
						//texCoords.push_back(v2);

						//texCoords.push_back(u3);
						//texCoords.push_back(v3);
						//New
						
						texCoords.push_back(u[12]);
						texCoords.push_back(v[12]);

						texCoords.push_back(u[13]);
						texCoords.push_back(v[13]);

						texCoords.push_back(u[14]);
						texCoords.push_back(v[14]);

						texCoords.push_back(u[15]);
						texCoords.push_back(v[15]);



						// 0,1,2,2,3,0
						indices.push_back(0 + indcs);
						indices.push_back(1 + indcs);
						indices.push_back(2 + indcs);
						indices.push_back(2 + indcs);
						indices.push_back(3 + indcs);
						indices.push_back(0 + indcs);

						for (int i = 0; i < 4; i++)	{
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

						//Up face UV's
						//Old
						//texCoords.push_back(u0);
						//texCoords.push_back(v0);

						//texCoords.push_back(u1);
						//texCoords.push_back(v1);

						//texCoords.push_back(u2);
						//texCoords.push_back(v2);

						//texCoords.push_back(u3);
						//texCoords.push_back(v3);
						//New
						texCoords.push_back(u[16]);
						texCoords.push_back(v[16]);

						texCoords.push_back(u[17]);
						texCoords.push_back(v[17]);

						texCoords.push_back(u[18]);
						texCoords.push_back(v[18]);

						texCoords.push_back(u[19]);
						texCoords.push_back(v[19]);


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


						//Down Faces UVs
						//Old
						//texCoords.push_back(u0);
						//texCoords.push_back(v0);

						//texCoords.push_back(u1);
						//texCoords.push_back(v1);

						//texCoords.push_back(u2);
						//texCoords.push_back(v2);

						//texCoords.push_back(u3);
						//texCoords.push_back(v3);
						//New
						texCoords.push_back(u[20]);
						texCoords.push_back(v[20]);

						texCoords.push_back(u[21]);
						texCoords.push_back(v[21]);

						texCoords.push_back(u[22]);
						texCoords.push_back(v[22]);

						texCoords.push_back(u[23]);
						texCoords.push_back(v[23]);


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
			}
		}

		Mesh mesh{};
		
		mesh.vertexCount = vertices.size() / 3;
		mesh.triangleCount = indices.size() / 3;
		mesh.vertices = (float*)MemAlloc(vertices.size() * sizeof(float));
		mesh.texcoords = (float*)MemAlloc(texCoords.size() * sizeof(float));
		mesh.normals = (float*)MemAlloc(normals.size() * sizeof(float));
		//mesh.colors = (unsigned char*)MemAlloc(colours.size() * sizeof(unsigned char));
		mesh.indices = (unsigned short*)MemAlloc(indices.size() * sizeof(unsigned short));


		std::copy(vertices.begin(), vertices.end(), mesh.vertices);
		std::copy(texCoords.begin(), texCoords.end(), mesh.texcoords);
		std::copy(normals.begin(), normals.end(), mesh.normals);
	//	std::copy(colours.begin(), colours.end(), mesh.colors);
		std::copy(indices.begin(), indices.end(), mesh.indices);

		vertices.clear();
		texCoords.clear();
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
		subchunk.subchunk_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("tiles_atlas.png");
		subchunk.subchunk_mesh = Mesh{};
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