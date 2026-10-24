/*
 * =============================================================================
 * File        : block.hpp
 * Author      : Adam Bassem
 *
 * Description :
 *     Block struct, used by chunk code
 *
 * Copyright © 2026 Adam Bassem & Alex. All rights reserved. Licensed under Apache 2.0.
 * =============================================================================
 */

#pragma once

enum block_type
{
	BLOCK_AIR,
	BLOCK_FILLED, // Temporary, later add actual blocks
};

// test states for filled block,
// later add actual states for actual blocks
enum block_state
{
	BLOCK_RED,
	BLOCK_GREEN,
	BLOCK_BLUE,
};

struct block_data
{
	block_type type;
	block_state state;
};

