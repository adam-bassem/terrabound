/*
 * =============================================================================
 * File        : playercamera.hpp
 * Author      : Adam Bassem
 * Created     : 23/10/2026
 *
 * Description :
 *     Player camera code
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#pragma once

#include "raylib.h"

class PlayerCamera
{
public:
	// mouse_sensitivity = 0.5 if 100%
	// (((mouse_sensitivity * 0.6) + 0.2)^3) * 0.06
	float camera_sensitivity = 0.0075f;

	PlayerCamera();
	~PlayerCamera();

	void Update();
	Vector3 Forward();
	Vector3 Right();
	Vector3 Up();
	void SetPosition(Vector3 position);
	Camera3D& GetNativeCamera();

private:
	Camera3D camera;
	float yaw;
	float pitch;
	Vector3 forward, up, right;
};
