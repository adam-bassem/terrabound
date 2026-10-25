/*
 * =============================================================================
 * File        : resourcemanager.hpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Resource manager
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#pragma once

#include "resource.h"

struct Resource
{
	void* Data;
	size_t Size;
};

namespace Resources
{
	Resource LoadTextureFromResource(int id);
}