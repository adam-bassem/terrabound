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

#include "postprocessing.hpp"

namespace
{
	const char* terrain_vs = R"(
		#version 330

		in vec3 vertexPosition;
		in vec4 vertexColor;

		uniform mat4 mvp;

		out vec3 fragColor;
		out float fragAO;

		void main()
		{
			fragColor = vertexColor.rgb;
			fragAO = vertexColor.a;
			gl_Position = mvp * vec4(vertexPosition, 1.0);
		}
		)";

	const char* terrain_fs = R"(
		#version 330

		in vec3 fragColor;
		in float fragAO;

		out vec4 finalColor;

		void main()
		{
			finalColor = vec4(fragColor * fragAO, 1.0);
		}
		)";

	Shader terrain_shader;
}

void PostProcessing::InitialiseTerrain()
{
	terrain_shader = LoadShaderFromMemory(terrain_vs, terrain_fs);
}

Shader& PostProcessing::GetTerrainShader()
{
	return terrain_shader;
}