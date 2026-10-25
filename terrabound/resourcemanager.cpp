/*
 * =============================================================================
 * File        : resourcemanager.cpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Resource manager
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#include <windows.h>
#undef CloseWindow
#include "resourcemanager.hpp"
#include <iostream>

Resource Resources::LoadTextureFromResource(int id)
{
	HRSRC res = FindResource(nullptr, MAKEINTRESOURCE(id), TEXT("PNG"));
	if (!res)
	{
		std::cerr << "Warning: Couldn't find resource ID " << id << std::endl;
		return {};
	}

	HGLOBAL mem = LoadResource(nullptr, res);
	const unsigned char* data = (const unsigned char*)LockResource(mem);
	DWORD size = SizeofResource(nullptr, res);

	Resource rsrc;
	rsrc.Data = (void*)data;
	rsrc.Size = size;

	return rsrc;
}