#include "character.hpp"
#include "raymath.h"

constexpr float fly_speed = 12.0f;
constexpr float fly_sprint_speed_multiplier = 3.0f;

Character::Character(PlayerCamera& cam)
	: camera(cam), velocity{ 0, 0, 0 }, position{ 0, 50 ,0 }
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
	bool up = IsKeyDown(KEY_SPACE);
	bool down = IsKeyDown(KEY_LEFT_SHIFT);
	sprint = IsKeyDown(KEY_LEFT_CONTROL);

	float dt = GetFrameTime();

	Vector3 f = camera.Forward();
	f.y = 0;
	Vector3 r = camera.Right();

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

	if (up)
	{
		input.y += 1.0f;
	}

	if (down)
	{
		input.y -= 1.0f;
	}

	if (Vector3Length(input) > 0.0001f)
	{
		input = Vector3Normalize(input);
	}

	float speed = fly_speed * (sprint ? fly_sprint_speed_multiplier : 1.0f);

	position = Vector3Add(position, Vector3Scale(input, speed * dt));
	camera.SetPosition(position);
}