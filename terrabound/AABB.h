/*
 * ============================================================================
 * File        : AABB.h
 * Author      : Alex
 *
 * Description :
 *     AABB Collision Code
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */
#pragma once
#include "raymath.h"

class AABB {
	public :
		float x0, y0, z0;
		float w, h, d;
		float x1, y1, z1;

		AABB(float sx, float sy, float sz, float w, float h, float d);

		bool intersects(AABB a, AABB b); //For Entities

		bool intersectsV(Vector3 a, Vector3 b); //For Entity to other
};