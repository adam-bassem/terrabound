/*
 * =============================================================================
 * File        : playercamera.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Player camera code
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#include "playercamera.hpp"
#include <raymath.h>
#include <cmath>
#include <algorithm>

// Initialise the camera
PlayerCamera::PlayerCamera()
{
	// Initialise raylib's camera
	camera.fovy = 90.0f;
	camera.position = { 0, 0, 0 };
	camera.projection = CAMERA_PERSPECTIVE;
	camera.target = { 0, 0, 1 };
	camera.up = { 0, 1, 0 };

	// Initialise yaw and pitch values
	yaw = pitch = 0.0f;
}

// Destructor
PlayerCamera::~PlayerCamera()
{
}

// Run every frame, updates pitch and yaw only
void PlayerCamera::Update()
{
	// Get mouse delta
	Vector2 mouse_delta = GetMouseDelta();

	// Update pitch
	pitch -= mouse_delta.y * camera_sensitivity * DEG2RAD;
	// Update yaw
	yaw -= mouse_delta.x * camera_sensitivity * DEG2RAD;

	// clamp pitch
	const float max_pitch = 89.0f * DEG2RAD;
	pitch = std::clamp(pitch, -max_pitch, max_pitch);

	// F_x = cos(pitch) * sin(yaw)
	// F_y = sin(pitch)
	// F_z = cos(pitch) * cos(yaw)
	forward.x = std::cosf(pitch) * std::sinf(yaw);
	forward.y = std::sinf(pitch);
	forward.z = std::cosf(pitch) * std::cosf(yaw);

	// R_x = -cos(yaw)
	// R_y = 0
	// R_z = sin(yaw)
	right.x = -std::cosf(yaw);
	right.y = 0;
	right.z = std::sinf(yaw);

	// U_x = -sin(pitch) * sin(yaw)
	// U_y = cos(pitch)
	// U_z = -sin(pitch) * cos(yaw)
	up.x = -std::sinf(pitch) * std::sinf(yaw);
	up.y = std::cosf(pitch);
	up.z = -std::sinf(pitch) * std::cosf(yaw);

	// Camera target = Camera position + forward
	camera.target = Vector3Add(camera.position, forward);
	// Camera up = up
	camera.up = up;
}

// Return calculated forward vector from pitch and yaw
Vector3 PlayerCamera::Forward()
{
	return forward;
}

// Return calculated right vector from pitch and yaw
Vector3 PlayerCamera::Right()
{
	return right;
}

// Return calculated up vector from pitch and yaw
Vector3 PlayerCamera::Up()
{
	return up;
}

// Update camera's position, usually by character
void PlayerCamera::SetPosition(Vector3 position)
{
	camera.position = position;
	camera.target = Vector3Add(position, forward);
}

// Returns camera position
Vector3 PlayerCamera::GetPosition()
{
	return camera.position;
}

int PlayerCamera::GetRenderDistanceChunks()
{
	return 4;
}

// Returns the native camera (Camera3D)
Camera3D& PlayerCamera::GetNativeCamera()
{
	return camera;
}
