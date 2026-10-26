/*
 * ============================================================================
 * File        : world.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     World code
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#pragma once

#include "playercamera.hpp"
#include "block.hpp"
#include "raylib.h"

namespace World
{
	void GenerateWorld();
	void Render(PlayerCamera& camera);
	block_data* GetBlock(Vector3 Global);
	void SetBlock(Vector3 Global, block_data& data);
	void BreakBlock(Vector3 Global);
	void PlaceBlock(Vector3 Global);
}