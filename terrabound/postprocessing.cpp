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
		in vec2 vertexTexCoord;
		in vec4 vertexColor;

		uniform mat4 mvp;

		out vec2 fragTexCoord;
		out vec3 fragColor;
		out float fragAO;

		void main()
		{
			fragTexCoord = vertexTexCoord;
			fragColor = vertexColor.rgb;
			fragAO = vertexColor.a;
			gl_Position = mvp * vec4(vertexPosition, 1.0);
		}
		)";

	const char* terrain_fs = R"(
		#version 330

		in vec2 fragTexCoord;
		in vec3 fragColor;
		in float fragAO;

		uniform sampler2D texture0;

		out vec4 finalColor;

		void main()
		{
			vec4 texel = texture(texture0, fragTexCoord);
			if (texel.a < 0.5) discard;
			finalColor = vec4(texel.rgb * fragColor * fragAO, 1.0);
		}
		)";

	Shader terrain_shader;

	const char* sky_vs = R"(
	#version 330

	in vec3 vertexPosition;

	uniform mat4 mvp;

	out vec3 viewDir;

	void main()
	{
		viewDir = vertexPosition;
		gl_Position = (mvp * vec4(vertexPosition, 1.0)).xyww;
	}
	)";

	const char* sky_fs = R"(
	#version 330

	in vec3 viewDir;

	uniform vec3 horizonColor;
	uniform vec3 zenithColor;
	uniform vec3 sunDir;

	out vec4 finalColor;

	void main()
	{
		vec3 d = normalize(viewDir);

		float h = clamp(d.y, 0.0, 1.0);
		vec3 c = mix(horizonColor, zenithColor, pow(h, 0.5));

		float sun = smoothstep(0.9995, 0.9999, dot(d, normalize(sunDir)));
		c += vec3(1.0, 0.95, 0.8) * sun;

		finalColor = vec4(c, 1.0);
	}
	)";

	Shader sky_shader;
}

void PostProcessing::InitialiseTerrain()
{
	terrain_shader = LoadShaderFromMemory(terrain_vs, terrain_fs);
	sky_shader = LoadShaderFromMemory(sky_vs, sky_fs);
}

Shader& PostProcessing::GetTerrainShader()
{
	return terrain_shader;
}

Shader& PostProcessing::GetSkyShader()
{
	return sky_shader;
}