/*
 * ============================================================================
 * File        : world.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     World contains 20x20 chunks (temporarily, later will expand to an infinite world)
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#pragma once

#include "playercamera.hpp"

namespace World
{
	void GenerateWorld();
	void Render(PlayerCamera& camera);

	unsigned long long getWorldSeed(); //I added this -Alex
}