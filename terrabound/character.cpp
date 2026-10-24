/*
 * =============================================================================
 * File        : character.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Character code (for player character only), handles movement of player
 *     with velocity and friction (basic physics)
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#include "character.hpp"
#include "raymath.h"

constexpr float player_horizontal_speed = 4.6f;
constexpr float player_horizontal_sprint_speed_multiplier = 1.8f;
constexpr float player_vertical_speed = 4.6f;
constexpr float air_friction = 2.8f;
constexpr Vector3 player_camera_offset = { 0,1.6f,0 };

Character::Character(PlayerCamera& cam)
	: camera(cam), velocity{ 0, 0, 0 }, position{ 0,0,0 }
{
}

Character::~Character()
{

}

void Character::Update()
{
	// Get inputs
	bool W = IsKeyDown(KEY_W);
	bool A = IsKeyDown(KEY_A);
	bool S = IsKeyDown(KEY_S);
	bool D = IsKeyDown(KEY_D);
	sprint = IsKeyDown(KEY_LEFT_CONTROL) || (sprint && W);
	bool crouch = IsKeyDown(KEY_LEFT_SHIFT);
	bool jump = IsKeyDown(KEY_SPACE);

	float dt = GetFrameTime();

	Vector3 f = camera.Forward();
	Vector3 r = camera.Right();

	f.y = 0.0f;
	r.y = 0.0f;

	Vector3 input = { 0.0f, 0.0f, 0.0f };

	if (W)
	{
		input = Vector3Add(input, f);
	}

	if (S)
	{
		input = Vector3Subtract(input, f);
	}

	if (A)
	{
		input = Vector3Subtract(input, r);
	}

	if (D)
	{
		input = Vector3Add(input, r);
	}

	if (Vector3Length(input) > 0.0001f)
	{
		input = Vector3Normalize(input);
	}

	float speed = player_horizontal_speed *
		(sprint ? player_horizontal_sprint_speed_multiplier : 1.0f);

	Vector3 target_velocity = Vector3Scale(input, speed);
	float acceleration = Vector3Length(input) > 0.0001f ? 20.0f : air_friction;

	velocity.x += (target_velocity.x - velocity.x) * acceleration * dt;
	velocity.z += (target_velocity.z - velocity.z) * acceleration * dt;

	// Check Y axis movement
	if (jump)
	{
		velocity.y += player_vertical_speed * dt;
	}
	else if (crouch)
	{
		velocity.y -= player_vertical_speed * dt;
	}
	else
	{
		if (velocity.y > 0.0f)
		{
			velocity.y -= air_friction * dt;
		}
		else
		{
			velocity.y += air_friction * dt;
		}
	}

	position = Vector3Add(position, Vector3Scale(velocity, dt));
	camera.SetPosition(Vector3Add(position, player_camera_offset));
}
