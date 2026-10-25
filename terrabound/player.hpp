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

#pragma once

#include "playercamera.hpp"

namespace Player
{
	void Initialise();
	void Shutdown();
	void DrawUI();
	void Update(PlayerCamera& camera);
}