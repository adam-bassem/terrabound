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

private:
	// reference to player camera
	PlayerCamera& camera;
	// is sprinting?
	bool sprint = false;
	// player velocity
	Vector3 velocity;
	// player position
	Vector3 position;
};