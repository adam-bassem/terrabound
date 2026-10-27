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
#include <iostream>

namespace
{
	Texture2D empty_heart;
	Texture2D half_heart;
	Texture2D full_heart;

	Texture2D itemSelectionBar;
	Texture2D itemSelectionHighlighter;
	Texture2D playerInventory;
	Texture2D playerCreativeInventory; //test

	Texture2D block2d;

	bool isCreative = true;

	double lastRecordedTime = 0.0f;
	double lmbDuration = 0.0f;
	double rmbDuration = 0.0f;

	constexpr int max_health = 20;
	int health = max_health;
	block_data currentBlock;

Texture2D Load2dBlock() {
	Image img = LoadImage("res/tiles_atlas.png");
	Texture2D tex = LoadTextureFromImage(img);
	UnloadImage(img);
	return tex;
}

	Texture2D LoadHeart(int id)
	{
		Resource r = Resources::LoadTextureFromResource(id);
		Image img = LoadImageFromMemory(".png", (const unsigned char*)r.Data, r.Size);
		Texture2D tex = LoadTextureFromImage(img);
		UnloadImage(img);
		return tex;
	}

	Texture2D LoadQuickItemSelectionBar(int id) { //std::string resName Idk how to do it your way Adam, so i'm doing it my way (If you want to fix this then you can do so freely). -Alex
		//Image img = LoadImage(resName.c_str());
		Resource r = Resources::LoadTextureFromResource(id);
		Image img = LoadImageFromMemory(".png", (const unsigned char*)r.Data, r.Size);
		Texture2D tex = LoadTextureFromImage(img);
		UnloadImage(img);
		return tex;
	}

	Texture2D LoadInventory(int id) { //std::string resName Idk how to do it your way so i'm doing it my way (If you want to fix this then you can do so freely). -Alex
	//	Image img = LoadImage(resName.c_str());
		Resource r = Resources::LoadTextureFromResource(id);
		Image img = LoadImageFromMemory(".png", (const unsigned char*)r.Data, r.Size);
		Texture2D tex = LoadTextureFromImage(img);
		UnloadImage(img);
		return tex;
	}

	Texture2D LoadCreativeInventory(std::string resName) { //std::string resName Idk how to do it your way so i'm doing it my way (If you want to fix this then you can do so freely). -Alex
		//	Image img = LoadImage(resName.c_str());
		//Resource r = Resources::LoadTextureFromResource(id);
		Image img = LoadImage(resName.c_str());// LoadImageFromMemory(".png", (const unsigned char*)r.Data, r.Size);
		Texture2D tex = LoadTextureFromImage(img);
		UnloadImage(img);
		return tex;
	}


	Texture2D LoadQuickItemSelectionHighlighter(int id) { //std::string resName
	//	Image img = LoadImage(resName.c_str());
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
static Mesh itemMesh;
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

	DrawMesh(itemMesh, Material(), Matrix());


	DrawTextureEx(itemSelectionHighlighter, Vector2{ (float)sx * (i), (float)sy }, 0, 4, WHITE);

}

void Player::createItemMesh() {
	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<int> indices;
	//botttom left
	vertices.push_back(-0.5f);
	vertices.push_back(-0.5f);
	vertices.push_back(0.0f);
	//bottom right
	vertices.push_back(0.5f);
	vertices.push_back(-0.5f);
	vertices.push_back(0.0f);

	//upper right
	vertices.push_back(0.5f);
	vertices.push_back(0.5f);
	vertices.push_back(0.0f);

	//upper left
	vertices.push_back(-0.5f);
	vertices.push_back(0.5f);
	vertices.push_back(0.0f);
		
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	texCoords.push_back(0);
	texCoords.push_back(0);

	texCoords.push_back(1);
	texCoords.push_back(0);

	texCoords.push_back(1);
	texCoords.push_back(1);

	texCoords.push_back(0);
	texCoords.push_back(1);

	itemMesh = Mesh{};
	itemMesh.vertexCount = (int)indices.size();
	itemMesh.triangleCount = (int)indices.size() / 3;
	itemMesh.vertices = (float*)MemAlloc(indices.size() * 3 * sizeof(float));
	itemMesh.texcoords = (float*)MemAlloc(indices.size() * 2 * sizeof(float));
	itemMesh.indices = nullptr;

	for (size_t i = 0; i < indices.size(); i++)
	{
		const unsigned int k = indices[i];
		for (int c = 0; c < 3; c++)
		{
			itemMesh.vertices[i * 3 + c] = vertices[k * 3 + c];
		}
		for (int c = 0; c < 2; c++)
		{
			itemMesh.texcoords[i * 2 + c] = texCoords[k * 2 + c];
		}
	}
	UploadMesh(&itemMesh, false);
}

void Player::drawInventory() { //Draw inventory -Alex
	const int x = 10;
	const int y = 2;

	const int armorTextX = 585;
	const int armorTextY = 85;

	//World::GetBlock(Vector3(0, 90, 0));

	if (!isCreative) {
		DrawTextureEx(playerInventory, Vector2{ (float)x, (float)y - 27 }, 0, 3, WHITE);
		DrawText("Inventory", x + 105, y + 35, 30, DARKGRAY);
		DrawText("Inventory", x + 107, y + 37, 30, WHITE);

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
	else {
		DrawTextureEx(playerCreativeInventory, Vector2{ (float)x, (float)y - 27 }, 0, 3, WHITE);
		DrawText("Select Block", x + 105, y + 15, 30, DARKGRAY);
		DrawText("Select Block", x + 107, y + 17, 30, WHITE);
		

		

	//	block2d.width = 16;
	//	block2d.height = 16;
		
		//Basic way of drawing textures but no functionallity besides textures 
		for (int i = 0; i < 10; i++) {
			for (int l = 0; l < 2; l++) {
				int j = i + 1;
				DrawTexturePro(block2d, Rectangle(16 * j, 16 * l, 16, 16), Rectangle(128 + (i * 61), 54 + (l * 71), 48, 48), Vector2(0, 0), 5, WHITE);
			}
		}

	
		//DrawTextureRec(block2d, Rectangle(32, 0, 16, 16), Vector2(128, 54), WHITE);
		
		
		//DrawRectangle(128, 54, 48, 48, WHITE)
		

		//DrawCube(Vector3(20, 20, -1), 16, 16, 16, RED);
		
		//DrawCircle(20, 20, 20, RED);
		//DrawCube()
	}

}


void Player::Initialise()
{
	empty_heart = LoadHeart(IDB_PNG1);
	half_heart = LoadHeart(IDB_PNG3);
	full_heart = LoadHeart(IDB_PNG2);

	itemSelectionBar = LoadQuickItemSelectionBar(IDB_PNG6); //"res/gui/quick_item_bar.png"
	itemSelectionHighlighter = LoadQuickItemSelectionHighlighter(IDB_PNG7); //"res/gui/selection_highlighter.png"

	playerInventory = LoadInventory(IDB_PNG8); //"res/gui/player_inventory.png"
	playerCreativeInventory = LoadCreativeInventory("res/gui/player_creative_inventory.png"); //"res/gui/player_inventory.png"

	block2d = Load2dBlock();
	createItemMesh();
	lastRecordedTime = GetTime();
}

//bool Player::intersects(Vector3 a, Vector3 b) {
//	float aMinX = a.x;
//	float aMinY = a.y;
//	float aMinZ = a.z;
//
//	float aMaxX = a.x + 16; //Some hard coded values
//	float aMaxY = a.y + 16;
//	float aMaxZ = a.z + 16;
//
//	float bMinX = b.x;
//	float bMinY = b.y;
//	float bMinZ = b.z;
//				  
//	float bMaxX = b.x + 16; //Some hard coded values
//	float bMaxY = b.y + 16;
//	float bMaxZ = b.z + 16;
//
//
//	return 
//		aMinX <= bMaxX &&
//		aMaxX >= bMinX &&
//		aMinY <= bMaxY &&
//		aMaxY >= bMinY &&
//		aMinZ <= bMaxZ &&
//		aMaxZ >= bMinZ;
//}

void Player::Shutdown()
{
	UnloadTexture(empty_heart);
	UnloadTexture(half_heart);
	UnloadTexture(full_heart);	
	UnloadTexture(itemSelectionBar);
	UnloadTexture(itemSelectionHighlighter);
	UnloadTexture(playerInventory);
	UnloadTexture(playerCreativeInventory);
	UnloadTexture(block2d);
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

	if (IsKeyPressed(KEY_ONE)) {
		currentBlock.type = BLOCK_STONE;
		i = 1;
	}

	if (IsKeyPressed(KEY_TWO)) {
		currentBlock.type = BLOCK_DIRT;
		i = 2.34f;
	}

	if (IsKeyPressed(KEY_THREE)) {
		currentBlock.type = BLOCK_GRASS;
		i = 3.6f;
	}

	if (IsKeyPressed(KEY_FOUR)) {
		currentBlock.type = BLOCK_WOOD_LOG;
		i = 4.73f;
	}

	if (IsKeyPressed(KEY_FIVE)) {
		currentBlock.type = BLOCK_SHORT_GRASS;
		i = 6.04f;
	}

	if (IsKeyPressed(KEY_SIX)) {
		//	currentBlock = BLOCK_SHORT_GRASS;
		i = 7.34f;
	}

	if (IsKeyPressed(KEY_SEVEN)) {
		//	currentBlock = BLOCK_SHORT_GRASS;
		i = 8.6f;
	}


	if (IsKeyPressed(KEY_EIGHT)) {
		//	currentBlock = BLOCK_SHORT_GRASS;
		i = 9.84f;
	}


	if (IsKeyPressed(KEY_NINE)) {
		//	currentBlock = BLOCK_SHORT_GRASS;
		i = 11.1f;
	}


	if (IsKeyPressed(KEY_ZERO)) {
		//	currentBlock = BLOCK_SHORT_GRASS;
		i = 12.4f;
	}

	if (IsKeyPressed(KEY_C)) {
		isCreative = !isCreative;
	}

	std::string curBlockName;
	bool hasDied;

	int a = currentBlock.type;
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

	const bool in_reach = raycast.Hit && raycast.Distance <= 6.0f;

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) lmbDuration += dt;
	else lmbDuration = 0.0;

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) rmbDuration += dt;
	else rmbDuration = 0.0;


	std::string gamemode = isCreative ? "Creative" : "Survival";
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

	DrawText(TextFormat("Current Block is [%s]", curBlockName), 10, 70, 30, RED);
	DrawText(TextFormat("Current Gamemode is [%s]", gamemode), 10, 110, 30, BLUE);

	// Break
	if (in_reach)
	{
		double break_time;
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
	if (IsKeyPressed(KEY_E))
	{
		hasDied = false;
		health = std::max(0, health - 1);
	}
}

int Player::getHealth() {
	return health;
}

void Player::setHealth(int amount) {
	health = amount;
}