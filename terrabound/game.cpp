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
#include "rlgl.h"
#include <iostream>
#include <sstream>

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


	float rx = 0.0f;
	float gx = 0.0f;
	float bx = 0.0f;

	bool hasRx = true;
	bool hasGx = true;
	bool hasBx = true;
}

void animateSky();

int main()
{

	SetTraceLogLevel(LOG_ERROR);
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


	
	World::GenerateWorld();



	//Get the Working Directory
	std::string workDir = GetApplicationDirectory();
	//Get the Main Game Directory
	std::string gameDir = "Terrabound";
	//Full Game Directory Path
	std::string combinedDir = workDir + gameDir;

	std::cout << "Dir:" << combinedDir.c_str() << "\n";

	//If this thing doesn't exist then we create it!
	if (!DirectoryExists(gameDir.c_str())) {
		MakeDirectory("Terrabound\0");
		std::cout << "Created Directory: Terrabound!" << "\n";
	}

	Texture2D tex = LoadTextureFromImage(LoadImage("tiles_atlas.png"));
	GenTextureMipmaps(&tex);

	// As long as the window is still open, keep looping
	while (!WindowShouldClose())
	{
		BeginDrawing();
			
	

		animateSky();

		Color color = Color(rx, gx, bx, 255);

		ClearBackground(color);
		
		//rlEnableBackfaceCulling();
		BeginMode3D(player_camera.GetNativeCamera());
		World::Render(player_camera);

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

		Vector3 f = player_camera.Forward();



		float basedScale = 30.0f;

		float baseX = 20.0f;
		float baseY = 20.0f;

		float addition = 30.0f;

		float angle = atan2f(f.x, f.z) * RAD2DEG;
		if (angle < 0) angle += 360.0f;
		if (angle >= 315.0f || angle < 45.0f)
		{
			DrawText("Facing north", baseX, baseY, basedScale, BLUE);
		}
		else if (angle >= 45.0f && angle < 135.0f)
		{
			DrawText("Facing east", baseX, baseY, basedScale, RED);
		}
		else if (angle >= 135.0f && angle < 225.0f)
		{
			DrawText("Facing south", baseX, baseY, basedScale, BLUE);
		}
		else if (angle >= 225.0f && angle < 315.0f)
		{
			DrawText("Facing west", baseX, baseY, basedScale, RED);
		}
		DrawFPS(20, baseY + addition);

		DrawText(TextFormat("Player XYZ %f, %f, %f", player_camera.GetPosition().x, player_camera.GetPosition().y, player_camera.GetPosition().z), baseX + 2, (baseY * 4) + 2/*62*/, basedScale, DARKGREEN);
		DrawText(TextFormat("Player XYZ %f, %f, %f", player_camera.GetPosition().x, player_camera.GetPosition().y, player_camera.GetPosition().z), baseX, baseY * 4, basedScale, GREEN);
		DrawText(TextFormat("World Seed: %i", World::getWorldSeed()), baseX, baseY * 6, basedScale, PURPLE); //Display the World Seed

		EndDrawing();
	}


	//Get the world seed and then write it to a file
	std::string seed = TextFormat("%i", World::getWorldSeed());

	if (!FileExists("seed.txt")) {
		SaveFileText("seed.txt", seed.c_str());
	}
	else {
		SaveFileText("seed.txt", seed.c_str());
	}

	// Finished program
	CloseWindow();
	return 0;
}

//Code for the animated sky
static void animateSky() {
	if (rx > 0.0f && hasRx) {
		std::sinf(rx -= 1.0f);
	}
	else {
		hasRx = false;
	}


	if (rx < 128.0f && !hasRx) {
		std::sinf(rx += 1.0f);

	}
	else {
		hasRx = true;
	}

	if (gx > 0.0f && hasGx) {
		std::sinf(gx -= 1.0f);
	}
	else {
		hasGx = false;
	}


	if (gx < 128.0f && !hasGx) {
		std::sinf(gx += 1.0f);

	}
	else {
		hasGx = true;
	}

	if (bx > 0.0f && hasBx) {
		std::sinf(bx -= 2.0f);
	}
	else {
		hasBx = false;
	}


	if (bx < 254.0f && !hasBx) {
		std::sinf(bx += 2.0f);
	}
	else {
		hasBx = true;
	}
}