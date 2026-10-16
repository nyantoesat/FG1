#pragma once
#include "Config.h"
#include "MathUtil.h"

// The hellvator. You stand on its roof. It carries you down to the next circle.
struct Cage {
	float x = kCageX;
	float y = kCageRestY; // top of the roof
	float vy = 0.0f;      // how much it moved this frame (to carry the player)

	Rect GetRect() const { return {x, y, kCageWidth, kCageHeight}; }
	void Draw() const;
};