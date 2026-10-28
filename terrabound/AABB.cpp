/*
 * ============================================================================
 * File        : AABB.cpp
 * Author      : Alex
 *
 * Description :
 *     AABB Collision Code
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * ============================================================================
 */

#include "AABB.h"

//Implement this correctly in the future -Alex
AABB::AABB(float sx, float sy, float sz, float w, float h, float d) {
	this->x0 = sx; 
	this->y0 = sy;
	this->z0 = sz;
	this->w = w;
	this->h = h;
	this->d = d;
	this->x1 = x0 + w; //The Max x is the min x + the width
	this->y1 = y0 + h; //The Max y is the min y + the height
	this->z1 = z0 + d; //The Max z is the min z + the depth
}

bool AABB::intersects(AABB a, AABB b) {
	return a.x0 <= b.x1 && a.x1 >= b.x0 &&
		   a.y0 <= b.y1 && a.y1 >= b.y0 &&
		   a.z0 <= b.z1 && a.z1 >= b.z0;
}

bool AABB::intersectsV(Vector3 a, Vector3 b) {
	float aMinX = a.x;
	float aMinY = a.y;
	float aMinZ = a.z;

	float aMaxX = a.x + w; 
	float aMaxY = a.y + h;
	float aMaxZ = a.z + d;

	float bMinX = b.x;
	float bMinY = b.y;
	float bMinZ = b.z;

	float bMaxX = b.x + w;
	float bMaxY = b.y + h;
	float bMaxZ = b.z + d;


	return
		aMinX <= bMaxX &&
		aMaxX >= bMinX &&
		aMinY <= bMaxY &&
		aMaxY >= bMinY &&
		aMinZ <= bMaxZ &&
		aMaxZ >= bMinZ;
}