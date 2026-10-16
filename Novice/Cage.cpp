#include "Cage.h"
#include <Novice.h>

void Cage::Draw() const {
	
	Novice::DrawBox((int)x, (int)y, (int)kCageWidth, (int)kCageHeight, 0.0f, 0x2E5A6AFF, kFillModeSolid);
	Novice::DrawBox((int)x, (int)y, (int)kCageWidth, 8, 0.0f, 0x8FDCC8FF, kFillModeSolid); // roof = where you stand
	
	Novice::DrawBox((int)(x + kCageWidth * 0.5f) - 2, (int)y - 2000, 4, 2000, 0.0f, 0x2E5A6AFF, kFillModeSolid);
}