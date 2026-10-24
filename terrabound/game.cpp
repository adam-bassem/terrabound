/*
 * =============================================================================
 * File        : game.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Entry point of the game
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#include "raylib.h"
#include "playercamera.hpp"
#include "character.hpp"
#include "chunk.hpp"
#include "raymath.h"
#include "world.hpp"
#include "postprocessing.hpp"
#include "ddaraycast.hpp"

namespace
{
	// Player camera, handles camera rotation, and player
	// character updates it's position
	PlayerCamera player_camera;
	// Player character, handles movement and connects
	// to player camera to update it's position
	Character character(player_camera);
	// Track if cursor is enabled or not
	bool cursor_enabled;
}

int main()
{
	// Enable anti-aliasing (MSAA 4x)
	// and enable vertical sync
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	// Create a 1280x720 window titled "terrabound"
	InitWindow(1280, 720, "terrabound");
	// Limit FPS to current monitor's refresh rate
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
	// Disable cursor so it doesn't exit outside window
	DisableCursor();
	cursor_enabled = false;
	// Set exit key to nothing so the game doesn't exit unless you
	// quit from X button (TODO: make a quit menu button later)
	SetExitKey(0);

	// Create terrain shader
	PostProcessing::InitialiseTerrain();

	// Create the world
	World::GenerateWorld();

	// As long as the window is still open, keep looping
	while (!WindowShouldClose())
	{
		BeginDrawing();

		RaycastResult result = DDARaycast::DoRaycast(player_camera.GetPosition(), player_camera.Forward());

		// Clear the frame to SKYBLUE colour
		ClearBackground(SKYBLUE);

		BeginMode3D(player_camera.GetNativeCamera());

		World::Render(player_camera);

		if (result.Hit && result.Distance <= 6)
		{
			Vector3 centre = { result.GlobalBlockPosition.x + 0.5f,
							   result.GlobalBlockPosition.y + 0.5f,
							   result.GlobalBlockPosition.z + 0.5f };
			DrawCubeWires(centre, 1.01f, 1.01f, 1.01f, BLACK);
		}

		EndMode3D();

		// Update camera rotation and player movement,
		// only if cursor disabled (hidden)
		if (!cursor_enabled)
		{
			player_camera.Update();
			character.Update();
		}

		// If escape is pressed, toggle cursor on/off
		if (IsKeyPressed(KEY_ESCAPE))
		{
			cursor_enabled ? DisableCursor() : EnableCursor();
			cursor_enabled = !cursor_enabled;
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			if (result.Hit && result.Distance <= 6)
			{
				World::BreakBlock(result.GlobalBlockPosition);
			}
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			if (result.Hit && result.Distance <= 6)
			{
				World::PlaceBlock(result.FirstVector3AlongNormal);
			}
		}

		Vector3 f = player_camera.Forward();

		float angle = atan2f(f.x, f.z) * RAD2DEG;
		if (angle < 0) angle += 360.0f;
		if (angle >= 315.0f || angle < 45.0f)
		{
			DrawText("Facing north", 20, 20, 20, BLUE);
		}
		else if (angle >= 45.0f && angle < 135.0f)
		{
			DrawText("Facing east", 20, 20, 20, RED);
		}
		else if (angle >= 135.0f && angle < 225.0f)
		{
			DrawText("Facing south", 20, 20, 20, BLUE);
		}
		else if (angle >= 225.0f && angle < 315.0f)
		{
			DrawText("Facing west", 20, 20, 20, RED);
		}
		DrawFPS(20, 40);

		EndDrawing();
	}

	// Finished program
	CloseWindow();
	return 0;
}
