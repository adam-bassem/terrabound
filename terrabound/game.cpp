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
#include "player.hpp"
#include "rlgl.h"

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
	// Skybox
	Texture2D skybox_texture;
}

static void toggleCursor();

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
	// Set trace log to no logs for performance
	SetTraceLogLevel(LOG_NONE);

	// Initialise player, loads resources
	Player::Initialise();

	// Create terrain shader
	PostProcessing::InitialiseTerrain();

	// Create the world
	World::GenerateWorld();

	Mesh skybox_mesh = GenMeshCube(2.0f, 2.0f, 2.0f);
	Model skybox = LoadModelFromMesh(skybox_mesh);
	Shader skybox_shader = PostProcessing::GetSkyShader();
	int sky_horizon_loc = GetShaderLocation(skybox_shader, "horizonColor");
	int sky_zenith_loc = GetShaderLocation(skybox_shader, "zenithColor");
	int sky_sun_loc = GetShaderLocation(skybox_shader, "sunDir");
	skybox.materials[0].shader = skybox_shader;

	Vector3 horizon = { 0.53f, 0.81f, 0.92f };
	Vector3 zenith = { 0.25f, 0.45f, 0.85f };
	Vector3 sun = Vector3Normalize({ -0.4f, -1.0f, -0.3f });
	float sundir = 0.0f; // horizon
	bool isInInventory = false;
	// As long as the window is still open, keep looping
	while (!WindowShouldClose())
	{
		BeginDrawing();

		RaycastResult result = DDARaycast::DoRaycast(player_camera.GetPosition(), player_camera.Forward());
		const Vector3 pos = player_camera.GetPosition();
		const Vector3 f = player_camera.Forward();

		// Clear the frame
		ClearBackground(BLACK);

		BeginMode3D(player_camera.GetNativeCamera());

		rlDisableBackfaceCulling();

		DrawModel(skybox, pos, 100.0f, WHITE);
		rlEnableBackfaceCulling();

		World::Render(player_camera);

		if (result.Hit && result.Distance <= 6)
		{
			Vector3 centre = { result.GlobalBlockPosition.x + 0.5f,
							   result.GlobalBlockPosition.y + 0.5f,
							   result.GlobalBlockPosition.z + 0.5f };
			DrawCubeWires(centre, 1.01f, 1.01f, 1.01f, BLACK);
		}

		// axis gizmo, always on top
		rlDisableDepthTest();

		const Vector3 gizmo_origin = Vector3Add(player_camera.GetPosition(), player_camera.Forward());
		DrawLine3D(gizmo_origin, Vector3Add(gizmo_origin, Vector3 { 0.15f, 0.0f, 0.0f }), RED);
		DrawLine3D(gizmo_origin, Vector3Add(gizmo_origin, Vector3 { 0.0f, 0.15f, 0.0f }), GREEN);
		DrawLine3D(gizmo_origin, Vector3Add(gizmo_origin, Vector3 { 0.0f, 0.0f, 0.15f }), BLUE);

		rlDrawRenderBatchActive();
		rlEnableDepthTest();

		EndMode3D();

		// Update camera rotation and player movement,
		// only if cursor disabled (hidden)
		if (!cursor_enabled)
		{
			player_camera.Update();
			character.Update();
		}
		Player::Update(player_camera);

		// If escape is pressed, toggle cursor on/off
		if (IsKeyPressed(KEY_ESCAPE))
		{
			toggleCursor();
		}

		
	
		if (isInInventory) {
			Player::drawInventory();
		}

		if (IsKeyPressed(KEY_I)) { //Toggle between in inventory or not
			isInInventory = !isInInventory;
			toggleCursor();
		}

		// If escape is pressed, toggle cursor on/off
		if (IsKeyPressed(KEY_ESCAPE))
		{
			toggleCursor();
		}
		
		DrawFPS(20, 60);

		Player::DrawUI();

		// S_x = -cos(sundir)
		// S_y = 0
		// S_z = sin(sundir)
		sun.x = std::cosf(sundir);
		sun.y = -std::sinf(sundir);
		sun.z = 0;
		// 360 / (24 * 60) * dt
		// 360 / (mins * min2sec) * dt
		sundir += 0.25f * GetFrameTime() * DEG2RAD; // 24-minutes to fully rotate around the world

		SetShaderValue(skybox_shader, sky_horizon_loc, &horizon, SHADER_UNIFORM_VEC3);
		SetShaderValue(skybox_shader, sky_zenith_loc, &zenith, SHADER_UNIFORM_VEC3);
		SetShaderValue(skybox_shader, sky_sun_loc, &sun, SHADER_UNIFORM_VEC3);

		EndDrawing();
	}

	// Free resources
	Player::Shutdown();

	UnloadTexture(skybox_texture);

	// Finished program
	CloseWindow();
	return 0;
}


static void toggleCursor() {

	cursor_enabled ? DisableCursor() : EnableCursor();
	cursor_enabled = !cursor_enabled;
}