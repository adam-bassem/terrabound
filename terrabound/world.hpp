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

namespace World
{
	void GenerateWorld();
	void Render(PlayerCamera& camera);
}