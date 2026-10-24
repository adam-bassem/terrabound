/*
 * ============================================================================
 * File        : postprocessing.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Shaders and post processing stuff
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#include "raylib.h"

namespace PostProcessing
{
	void InitialiseTerrain();
	Shader& GetTerrainShader();
}