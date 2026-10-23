/*
 * =============================================================================
 * File        : game.cpp
 * Author      : Adam Bassem
 * Created     : 23/10/2026
 *
 * Description :
 *     Entry point of the game
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#include "raylib.h"
#include "playercamera.hpp"

namespace
{
	// Player camera, handles camera rotation, and player
	// character updates it's position
	PlayerCamera player_camera;
	// Track if cursor is enabled or not
	bool cursor_enabled;
}

int main()
{
	// Enable anti-aliasing (MSAA 4x)
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	// Enable vertical sync
	SetConfigFlags(FLAG_VSYNC_HINT);
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

	player_camera.SetPosition({ 0, 15, 0 });

	// As long as the window is still open, keep looping
	while (!WindowShouldClose())
	{
		BeginDrawing();

		// Clear the frame to SKYBLUE colour
		ClearBackground(SKYBLUE);

		BeginMode3D(player_camera.GetNativeCamera());

		DrawGrid(100, 20);

		EndMode3D();

		// Update camera rotation
		player_camera.Update();

		// If escape is pressed, toggle cursor on/off
		if (IsKeyPressed(KEY_ESCAPE))
		{
			cursor_enabled ? DisableCursor() : EnableCursor();
		}

		EndDrawing();
	}

	// Finished program
	CloseWindow();
	return 0;
}
