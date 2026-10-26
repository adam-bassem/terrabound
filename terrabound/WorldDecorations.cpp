/*
 * =============================================================================
 * File        : WorldDecorations.cpp
 * Author      : Alex
 * Date		   : 26/10/26 18:29 GMT + 0000
 * 
 * Description :
 *     World Decorations Class, contains all feature decorations of the world (Like trees, bushes, flowers etc)
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */
#include "WorldDecorations.h"
//Doesn't work right now -Alex
void WorldDecorations::DecorateTree(Chunk* chunk, Vector3 base, std::mt19937_64& rng)
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
			chunk->SetBlock(Vector3{ (float)x, (float)y, (float)z }, Block::GetBlock(type));
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
		set(bx, by + dy, bz, BLOCK_WOOD_LOG);
	}
}

void WorldDecorations::DecoratePineTree(Chunk* chunk, Vector3 base, std::mt19937_64& rng) {
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
			chunk->SetBlock(Vector3{ (float)x, (float)y, (float)z }, Block::GetBlock(type));
		};

	//Upper 5x5 leaves
	for (int dy = h - 10; dy <= h; dy++) //so this seems to place leaves
	{
		for (int dx = -2; dx <= 2; dx++)
		{
			for (int dz = -2; dz <= 2; dz++)
			{
				set(bx + dx, by + dy, bz + dz, BLOCK_PINE_LEAVES);
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

				set((bx + dx), brh, (bz + dz), BLOCK_PINE_LEAVES);
			}

		}
	}



	// Trunk / Branches last, so that the leaves don't cover it
	for (int dy = 0; dy < h; dy++)
	{
		set(bx, by + dy, bz, BLOCK_PINE_LOG);


		//	set(bx + 1, by + dy, bz, BLOCK_PINE_LOG);
		//	set(bx + 1, by + dy, bz + 1, BLOCK_PINE_LOG);
			//set(bx, by + dy, bz + 1, BLOCK_PINE_LOG);


			 //North - South
			//for (int i = 0; i < br; i++) {
			//	set(bx, h - 5, bz + i, BLOCK_PINE_LOG);
			//
			//}
			//for (int i = 0; i < br; i++) {
			//	set(bx, h - 4, (bz + br) + i, BLOCK_PINE_LOG);
			//}

			//for (int i = 0; i > -br; i--) {
			//	set(bx, brh, bz + i, BLOCK_PINE_LOG);

			//}
			//for (int i = 0; i > -br; i--) {
			//	set(bx, brh, (bz - br) + i, BLOCK_PINE_LOG);
			//}



			////West - East

			//for (int i = 0; i < br; i++) {
			//	set(bx + i, h - 5, bz , BLOCK_PINE_LOG);

			//}
			//for (int i = 0; i < br; i++) {
			//	set((bx + br) + i, h - 4, bz , BLOCK_PINE_LOG);
			//}

			//for (int i = 0; i > nbr; i--) {
			//	set(bx + i, nbr, bz , BLOCK_PINE_LOG);

			//}

			//for (int i = 0; i > nbr; i--) {
			//	set((bx - nbr) + i, nbr, bz , BLOCK_PINE_LOG);
			//}



	}
}


void WorldDecorations::DecorateBush(Chunk* chunk, Vector3 base)
{
	const int bx = (int)base.x;
	const int by = (int)base.y;
	const int bz = (int)base.z;
	const int max_y = subchunk_size * chunk_height_subchunks;

	auto set = [&](int x, int y, int z, auto type)
		{
			if (x < 0 || x >= subchunk_size || z < 0 || z >= subchunk_size) return;
			if (y < 0 || y >= max_y) return;
			chunk->SetBlock(Vector3{ (float)x, (float)y, (float)z }, Block::GetBlock(type));
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
