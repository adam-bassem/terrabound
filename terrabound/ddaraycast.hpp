/*
 * ============================================================================
 * File        : ddaraycast.hpp
 * Author      : Adam Bassem
 *
 * Description :
 *     DDA raycasts
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#pragma once

#include "raylib.h"
#include "block.hpp"

struct RaycastResult
{
	bool Hit;
	Vector3 GlobalBlockPosition;
	block_data* HitBlock;
	Vector3 Normal;
	Vector3 FirstVector3AlongNormal;
	block_data* FirstBlockAlongNormal;
	float Distance;
};

namespace DDARaycast
{
	RaycastResult DoRaycast(Vector3 Origin, Vector3 Forward);
}