/*
 * =============================================================================
 * File        : WorldDecorations.h
 * Author      : Alex 
 * Date		   : 26/10/26 18:29 GMT + 0000
 * 
 * Description :
 *     World Decorations Class, contains all feature decorations of the world (Like trees, bushes, flowers etc)
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */
#pragma once

#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <random>
#include "chunk.hpp"
#include "block.hpp"

namespace WorldDecoration {
	void DecorateTree(Chunk* chunk, Vector3 base, std::mt19937_64& rng);

	void DecoratePineTree(Chunk* chunk, Vector3 base, std::mt19937_64& rng);

	void DecorateBush(Chunk* chunk, Vector3 base);

	void DecorateFeatureSecondPass(Chunk* chunk, uint64_t seed);

	static bool generatedFeature = false;
}


class WorldDecorations {
	public :
		
	//void DecorateTree(Chunk* chunk, Vector3 base, std::mt19937_64& rng);

	//void DecoratePineTree(Chunk* chunk, Vector3 base, std::mt19937_64& rng);

	//void DecorateBush(Chunk* chunk, Vector3 base);

	//void DecorateSecondPass();
};
