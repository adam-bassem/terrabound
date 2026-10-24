/*
 * ============================================================================
 * File        : ddaraycast.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     DDA raycasts
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#include "ddaraycast.hpp"
#include "world.hpp"
#include <cmath>
#include <limits>
#include "raymath.h"

RaycastResult DDARaycast::DoRaycast(Vector3 Origin, Vector3 Forward)
{
	Forward = Vector3Normalize(Forward);

	int x = (int)std::floor(Origin.x);
	int y = (int)std::floor(Origin.y);
	int z = (int)std::floor(Origin.z);

	int sx = (Forward.x > 0) ? 1 : (Forward.x < 0 ? -1 : 0);
	int sy = (Forward.y > 0) ? 1 : (Forward.y < 0 ? -1 : 0);
	int sz = (Forward.z > 0) ? 1 : (Forward.z < 0 ? -1 : 0);

	float dx = (Forward.x != 0) ? std::abs(1.0f / Forward.x) : INFINITY;
	float dy = (Forward.y != 0) ? std::abs(1.0f / Forward.y) : INFINITY;
	float dz = (Forward.z != 0) ? std::abs(1.0f / Forward.z) : INFINITY;

	float tx = (Forward.x > 0)
		? (x + 1.0f - Origin.x) / Forward.x
		: (Forward.x < 0)
		? (Origin.x - x) / -Forward.x : INFINITY;

	float ty = (Forward.y > 0)
		? (y + 1.0f - Origin.y) / Forward.y
		: (Forward.y < 0)
		? (Origin.y - y) / -Forward.y : INFINITY;

	float tz = (Forward.z > 0)
		? (z + 1.0f - Origin.z) / Forward.z
		: (Forward.z < 0)
		? (Origin.z - z) / -Forward.z : INFINITY;

	Vector3 normal{ 0, 0, 0 };

	float distance = 0.0f;

	const float max_distance = 200.0f;

	while (true)
	{
		if (distance > max_distance)
		{
			return { false, Vector3{ (float)x, (float)y, (float)z }, nullptr, normal, Vector3{ 0, 0, 0 }, nullptr };
		}

		block_data* current = World::GetBlock(
			Vector3{ (float)x, (float)y, (float)z }
		);

		// Treat out-of-world/null as a miss.
		if (!current)
		{
			return {
				false,
				Vector3{ (float)x, (float)y, (float)z },
				nullptr,
				normal,
				Vector3{ 0, 0, 0 },
				nullptr
			};
		}

		if (current->type != BLOCK_AIR)
		{
			Vector3 adjacentPos{
				(float)(x + (int)normal.x),
				(float)(y + (int)normal.y),
				(float)(z + (int)normal.z)
			};

			block_data* adjacent = World::GetBlock(adjacentPos);

			return {
				true,
				Vector3{ (float)x, (float)y, (float)z },
				current,
				normal,
				adjacentPos,
				adjacent,
				distance
			};
		}

		if (tx <= ty && tx <= tz)
		{
			distance = tx;
			x += sx;
			tx += dx;
			normal = Vector3{ (float)-sx, 0, 0 };
		}
		else if (ty <= tz)
		{
			distance = ty;
			y += sy;
			ty += dy;
			normal = Vector3{ 0, (float)-sy, 0 };
		}
		else
		{
			distance = tz;
			z += sz;
			tz += dz;
			normal = Vector3{ 0, 0, (float)-sz };
		}
	}
}