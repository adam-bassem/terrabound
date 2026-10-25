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
#include "raymath.h"
#include "block.hpp"
#include "resourcemanager.hpp"
#include <algorithm>
#include <iostream>

namespace
{
	Texture2D empty_heart;
	Texture2D half_heart;
	Texture2D full_heart;


	Texture2D itemSelectionBar;
	Texture2D itemSelectionHighlighter;
	Texture2D playerInventory;

	//Texture2D block3d;

	double lastRecordedTime = 0.0f;
	double lmbDuration = 0.0f;
	double rmbDuration = 0.0f;

	bool isCreative = true;
	constexpr int max_health = 20;
	int health = max_health;
	block_type currentBlock;

	//Texture2D Load3dBlock() {
	//	Image img = LoadImage("tiles_atlas.png");
	//	Texture2D tex = LoadTextureFromImage(img);
	//	UnloadImage(img);
	//	return tex;
	//}

	Texture2D LoadHeart(int id)
	{
		Resource r = Resources::LoadTextureFromResource(id);
		Image img = LoadImageFromMemory(".png", (const unsigned char*)r.Data, r.Size);
		Texture2D tex = LoadTextureFromImage(img);
		UnloadImage(img);
		return tex;
	}

	Texture2D LoadQuickItemSelectionBar(std::string resName) { //Idk how to do it your way Adam, so i'm doing it my way (If you want to fix this then you can do so freely). -Alex
		Image img = LoadImage(resName.c_str()); 
		Texture2D tex = LoadTextureFromImage(img);
		UnloadImage(img);
		return tex;
	}

	Texture2D LoadInventory(std::string resName) { //Idk how to do it your way so i'm doing it my way (If you want to fix this then you can do so freely). -Alex
		Image img = LoadImage(resName.c_str());
		Texture2D tex = LoadTextureFromImage(img);
		UnloadImage(img);
		return tex;
	}


	Texture2D LoadQuickItemSelectionHighlighter(std::string resName) {
		Image img = LoadImage(resName.c_str());  
		Texture2D tex = LoadTextureFromImage(img);
		UnloadImage(img);
		return tex;
	}
	Mesh mesh{};



	//void DrawHeart(Texture2D tex, int x, int y)
	//{
	//	DrawTextureEx(tex, Vector2{ (float)x, (float)y }, 0, 2, WHITE);
	//}

	//void DrawQuickItemSelectionBar(Texture2D tex, int x, int y) {
	//	DrawTextureEx(tex, Vector2{ (float)x, (float)y }, 0, 2, WHITE);
	//}
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

		const int w = GetScreenWidth();

		const int x = w - 400 + i * 33;
		const int y = 650;

		if (i < empty) DrawTextureEx(empty_heart, Vector2{ (float)x, (float)y }, 0, 2, WHITE);
		else if (i == empty && has_half) DrawTextureEx(half_heart, Vector2{ (float)x, (float)y }, 0, 2, WHITE);
		else DrawTextureEx(full_heart, Vector2{ (float)x, (float)y }, 0, 2, WHITE);
	}
}	
float i = 1;

void draw3dBlock() { //todo:draw a mesh that's a isometric block with the texture being from what is the current block.
//	const int x = 50;
	//const int y = 600;
//	const int sx = x + 4;
	//const int sy = y + 28;
	//Currently, Just draw the bar. -Alex
	//DrawMesh(mesh, Material(), Matrix());
//	DrawTextureEx(block3d, Vector2{ (float)x, (float)y }, 0, 4, WHITE);
	///ImageDrawRectanglePro(, Rectangle(x, y, 128, 128), Vector2(x, y), 45.0f, WHITE);
}

void drawQuickItemSelectionBars() {
	//Default 
		const int x = 50;
		const int y = 600;

	//Edited

		//const int x = 600;
		//const int y = 180;

		const int sx = x + 4;
		const int sy = y + 28;

		//Currently, Just draw the bar. -Alex
		DrawTextureEx(itemSelectionBar, Vector2{ (float)x, (float)y }, 0, 4, WHITE);
	

	

		DrawTextureEx(itemSelectionHighlighter, Vector2{ (float)sx * (i ), (float) sy}, 0, 4, WHITE);

}

void Player::drawInventory() { //Draw inventory -Alex
	const int x = 10;
	const int y = 2;

	const int armorTextX = 585;
	const int armorTextY = 85;

	DrawTextureEx(playerInventory, Vector2{ (float)x, (float) y -27 }, 0, 3, WHITE);
	DrawText("Inventory", x + 105, y + 35, 30 , DARKGRAY);
	DrawText("Inventory", x + 107, y + 37, 30 , WHITE);

	DrawText("Armor", x + 475, y + 35, 25, DARKGRAY);
	DrawText("Armor", x + 477, y + 37, 25, WHITE);

	DrawText("Helmet", x + armorTextX, y + armorTextY, 25, DARKGRAY);
	DrawText("Helmet", x + armorTextX + 2, y + armorTextY + 2, 25, WHITE);

	DrawText("Chestplate", x + (armorTextX - 10), y + (armorTextY + 50), 25, DARKGRAY);
	DrawText("Chestplate", x + (armorTextX - 10) + 2, y + (armorTextY + 50) + 2, 25, WHITE);

	DrawText("Leggings", x + armorTextX, y + (armorTextY + 105), 25, DARKGRAY);
	DrawText("Leggings", x + armorTextX + 2, y + (armorTextY + 105) + 2, 25, WHITE);

	DrawText("Greaves", x + armorTextX, y + (armorTextY + 165), 25, DARKGRAY);
	DrawText("Greaves", x + armorTextX + 2, y + (armorTextY + 165) + 2, 25, WHITE);

	DrawText("Shield", x + (armorTextX - 410), y + (armorTextY + 165), 25, DARKGRAY);
	DrawText("Shield", x + (armorTextX - 410) + 2, y + (armorTextY + 165) + 2, 25, WHITE);

	DrawText("Quiver", x + (armorTextX - 410), y + (armorTextY + 105), 25, DARKGRAY);
	DrawText("Quiver", x + (armorTextX - 410) + 2, y + (armorTextY + 105) + 2, 25, WHITE);
}


void Player::Initialise()
{
	empty_heart = LoadHeart(IDB_PNG1);
	half_heart = LoadHeart(IDB_PNG3);
	full_heart = LoadHeart(IDB_PNG2);


	itemSelectionBar = LoadQuickItemSelectionBar("quick_item_bar.png");
	itemSelectionHighlighter = LoadQuickItemSelectionHighlighter("selection_highlighter.png");

	playerInventory = LoadInventory("player_inventory.png");
	//block3d = Load3dBlock();
	lastRecordedTime = GetTime();


	//TODO: create a isometrical block that renders in a seperate perspectie. -Alex 
	//float vertices[] = {
	//-0.5f, -0.5f, -0.5f,
	//0.5f, -0.5f, -0.5f,
	//0.5f, 0.5f, -0.5f,
	//-0.5f, 0.5f, -0.5f,
	//};

	//std::vector<float> verticesVec;

	//for (int i = 0; i < 12; i++) {
	//	verticesVec.push_back(vertices[i]);
	//}


	//
	//mesh.vertexCount = 3;
	//mesh.vertices = (float*)MemAlloc(verticesVec.size() * sizeof(float));
	//UploadMesh(&mesh, false);
}

void Player::Shutdown()
{
	UnloadTexture(empty_heart);
	UnloadTexture(half_heart);
	UnloadTexture(full_heart);
	UnloadTexture(itemSelectionBar);
	UnloadTexture(itemSelectionHighlighter);
}

void Player::DrawUI()
{


	if (!isCreative) {
		DrawHearts();
	}

	drawQuickItemSelectionBars();
	draw3dBlock();
}

void Player::Update(PlayerCamera& camera)
{
	double currentTime = GetTime();
	double dt = currentTime - lastRecordedTime;
	lastRecordedTime = currentTime;

	RaycastResult raycast = DDARaycast::DoRaycast(camera.GetPosition(), camera.Forward());


	float t = 0;

	if (!isCreative) {
		t = 0.15f;
	}
	else {
		t = 0.002f;
	}


	const bool in_reach = raycast.Hit && raycast.Distance <= 6.0f;

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) lmbDuration += dt;
	
	else lmbDuration = 0.0;

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) rmbDuration += dt;
	else rmbDuration = 0.0;


	if (IsKeyPressed(KEY_ONE) ) {
		currentBlock = BLOCK_STONE;
		i = 1;
	}

	if (IsKeyPressed(KEY_TWO)) {
		currentBlock = BLOCK_DIRT;
		i = 2.34f;
	}

	if (IsKeyPressed(KEY_THREE)) {
		currentBlock = BLOCK_GRASS;
		i = 3.6f;
	}

	if (IsKeyPressed(KEY_FOUR) ) {
		currentBlock = BLOCK_WOOD_LOG;
		i = 4.73f;
	}

	if (IsKeyPressed(KEY_FIVE) ) {
		currentBlock = BLOCK_SHORT_GRASS;
		i = 6.04f;
	}

	if (IsKeyPressed(KEY_SIX) ) {
	//	currentBlock = BLOCK_SHORT_GRASS;
		i = 7.34f;
	}

	if (IsKeyPressed(KEY_SEVEN) ) {
		//	currentBlock = BLOCK_SHORT_GRASS;
		i = 8.6f;
	}


	if (IsKeyPressed(KEY_EIGHT)) {
		//	currentBlock = BLOCK_SHORT_GRASS;
		i = 9.84f;
	}


	if (IsKeyPressed(KEY_NINE) ) {
		//	currentBlock = BLOCK_SHORT_GRASS;
		i = 11.1f;
	}


	if (IsKeyPressed(KEY_ZERO)) {
		//	currentBlock = BLOCK_SHORT_GRASS;
		i = 12.4f;
	}

	std::string curBlockName;

	int a = currentBlock;
	switch (a) {
		case 1: curBlockName = "Grass Block"; break;
		case 2: curBlockName = "Dirt Block"; break;
		case 3: curBlockName = "Stone Block"; break;
		case 4: curBlockName = "Snow Block"; break;
		case 5: curBlockName = "Log Block"; break;
		case 6: curBlockName = "Leaf Block"; break;
		case 7: curBlockName = "Bush Block"; break;
		case 8: curBlockName = "Grass Plant"; break;
		default: curBlockName = "NULL"; break;
	}

	//Compass
	const Vector3 pos = camera.GetPosition();
	const Vector3 f = camera.Forward();
	float angle = std::atan2f(f.x, f.z) * RAD2DEG;
	if (angle < 0) angle += 360.0f;


	const char* compass = "N";

	if (angle >= 315.0f || angle < 45.0f) compass = "N";
	else if (angle < 135.0f) compass = "W";
	else if (angle < 225.0f) compass = "S";
	else compass = "E";



	DrawText(TextFormat("XYZ: %.2f / %.2f / %.2f", pos.x, pos.y, pos.z), 20, 20, 20, WHITE);
	DrawText(TextFormat("Facing: %s (%.1f deg)", compass, angle), 20, 40, 20, WHITE);

	bool isInCreative;// =
	std::string gamemode = isCreative ? "Creative" : "Survival";

	DrawText(TextFormat("Current Block is [%s]", curBlockName), 10, 70, 30, RED);
	DrawText(TextFormat("Current Gamemode is [%s]", gamemode), 10, 110, 30, BLUE);


	if (IsKeyPressed(KEY_C)) {
		isCreative = !isCreative;
	}

	// Break
	if (in_reach)
	{
		double break_time ;
		if (!isCreative) {
			break_time = std::max(t, (float)Block::GetHardness(*raycast.HitBlock));
		}
		else {
			break_time = std::max(t, 0.002f);
		}
		
		if (lmbDuration >= break_time)
		{
			World::BreakBlock(raycast.GlobalBlockPosition);
			lmbDuration = 0.0;
		}
	}

	// Place
	if (in_reach && rmbDuration >= t)
	{
		World::PlaceBlock(raycast.FirstVector3AlongNormal, currentBlock);
		rmbDuration = 0.0;
	}

	// FOR TESTING ONLY

	float tilt = 0.0f;

	if (IsKeyPressed(KEY_E))
	{
		health = std::max(0, health - 1);
		Matrix matrix = GetCameraMatrix(camera.GetNativeCamera());
	//	matrix = MatrixRotateZ();
	//	matrix.
		//GetCameraMatrix(camera.GetNativeCamera())
	}
}