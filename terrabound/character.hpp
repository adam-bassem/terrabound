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

#pragma once

#include "playercamera.hpp"
#include "raylib.h"

class Character
{
public:
	Character(PlayerCamera& cam);
	~Character();

	// called per frame
	void Update();
	// player position
	Vector3 position;
private:
	// reference to player camera
	PlayerCamera& camera;
	// is sprinting?
	bool sprint = false;
	// player velocity
	Vector3 velocity;

};