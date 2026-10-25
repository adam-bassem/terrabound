/*
 * ============================================================================
 * File        : player.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Player code handles player input and player UI
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#include "player.hpp"
#include "world.hpp"
#include "ddaraycast.hpp"
#include "raylib.h"
#include "block.hpp"
#include "resourcemanager.hpp"
#include <algorithm>

namespace
{
	Texture2D empty_heart;
	Texture2D half_heart;
	Texture2D full_heart;

	double lastRecordedTime = 0.0f;
	double lmbDuration = 0.0f;
	double rmbDuration = 0.0f;

	constexpr int max_health = 20;
	int health = max_health;

	Texture2D LoadHeart(int id)
	{
		Resource r = Resources::LoadTextureFromResource(id);
		Image img = LoadImageFromMemory(".png", (const unsigned char*)r.Data, r.Size);
		Texture2D tex = LoadTextureFromImage(img);
		UnloadImage(img);
		return tex;
	}

	void DrawHeart(Texture2D tex, int x, int y)
	{
		DrawTextureEx(tex, Vector2{ (float)x, (float)y }, 0, 2, WHITE);
	}
}

void DrawHearts()
{
	// health is in half-hearts (20 means 10 hearts)
	constexpr int hearts = max_health / 2;
	const int full = health / 2;
	const bool has_half = health % 2 == 1;
	const int empty = hearts - full - (has_half ? 1 : 0);

	for (int i = 0; i < hearts; i++)
	{
		const int x = 20 + i * 33;
		const int y = 80;

		if (i < empty) DrawTextureEx(empty_heart, Vector2{ (float)x, (float)y }, 0, 2, WHITE);
		else if (i == empty && has_half) DrawTextureEx(half_heart, Vector2{ (float)x, (float)y }, 0, 2, WHITE);
		else DrawTextureEx(full_heart, Vector2{ (float)x, (float)y }, 0, 2, WHITE);
	}
}

void Player::Initialise()
{
	empty_heart = LoadHeart(IDB_PNG1);
	half_heart = LoadHeart(IDB_PNG3);
	full_heart = LoadHeart(IDB_PNG2);
	lastRecordedTime = GetTime();
}

void Player::Shutdown()
{
	UnloadTexture(empty_heart);
	UnloadTexture(half_heart);
	UnloadTexture(full_heart);
}

void Player::DrawUI()
{
	DrawHearts();
}

void Player::Update(PlayerCamera& camera)
{
	double currentTime = GetTime();
	double dt = currentTime - lastRecordedTime;
	lastRecordedTime = currentTime;

	RaycastResult raycast = DDARaycast::DoRaycast(camera.GetPosition(), camera.Forward());
	const bool in_reach = raycast.Hit && raycast.Distance <= 6.0f;

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) lmbDuration += dt;
	else lmbDuration = 0.0;

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) rmbDuration += dt;
	else rmbDuration = 0.0;

	// Break
	if (in_reach)
	{
		double break_time = std::max(0.25f, (float)Block::GetHardness(*raycast.HitBlock));
		if (lmbDuration >= break_time)
		{
			World::BreakBlock(raycast.GlobalBlockPosition);
			lmbDuration = 0.0;
		}
	}

	// Place
	if (in_reach && rmbDuration >= 0.25)
	{
		World::PlaceBlock(raycast.FirstVector3AlongNormal);
		rmbDuration = 0.0;
	}

	// FOR TESTING ONLY
	if (IsKeyPressed(KEY_E))
	{
		health = std::max(0, health - 1);
	}
}