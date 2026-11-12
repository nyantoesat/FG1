
#include "Cage.h"
#include <Novice.h>

// 崩れる床の上にいるか
bool Cage::IsOnBreakFloor(float left, float right) const {
	Rect r = GetBreakRect();
	float center = (left + right) * 0.5f;

	return center >= r.x && center < r.x + r.w;
}

// 着地可能か
bool Cage::CanLand(float left, float right) const {
	if (right <= x || left >= x + width) {
		return false;
	}

	if (breakEnabled && floorState == FloorState::Broken && IsOnBreakFloor(left, right)) {
		return false;
	}

	return true;
}

// 崩れる床の更新
void Cage::UpdateFloor(bool playerStanding) {
	if (!breakEnabled) {
		return;
	}

	switch (floorState) {
	case FloorState::Normal:
		if (playerStanding) {
			floorState = FloorState::Cracking;
			floorTimer = kBreakFrames;
		}
		break;

	case FloorState::Cracking:
		floorTimer--;

		if (floorTimer <= 0) {
			floorState = FloorState::Broken;
			floorTimer = kRestoreFrames;
		}
		break;

	case FloorState::Broken:
		floorTimer--;

		if (floorTimer <= 0) {
			floorState = FloorState::Normal;
			floorTimer = 0;
		}
		break;
	}
}

void Cage::ResetFloor() {
	floorState = FloorState::Normal;
	floorTimer = 0;
}

void Cage::Draw() const {
	// 足場本体
	Novice::DrawBox((int)x, (int)y, (int)width, (int)kCageHeight, 0.0f, 0x2E5A6AFF, kFillModeSolid);

	// 屋根
	Novice::DrawBox((int)x, (int)y, (int)width, 8, 0.0f, 0x8FDCC8FF, kFillModeSolid);

	if (breakEnabled) {
		Rect r = GetBreakRect();

		if (floorState == FloorState::Broken) {
			// 崩れた部分を背景色で表示
			Novice::DrawBox((int)r.x, (int)r.y, (int)r.w, (int)kCageHeight, 0.0f, 0x1C2033FF, kFillModeSolid);
		} else {
			unsigned int color = floorState == FloorState::Cracking ? 0xF0A050FF : 0xB8A078FF;

			Novice::DrawBox((int)r.x, (int)r.y, (int)r.w, 8, 0.0f, color, kFillModeSolid);

			if (floorState == FloorState::Cracking) {
				for (int i = 1; i <= 3; i++) {
					int crackX = (int)r.x + i * 24;

					Novice::DrawLine(crackX, (int)r.y, crackX + 8, (int)r.y + 8, 0x502A20FF);
				}
			}
		}
	}

	// ワイヤー
	Novice::DrawBox((int)(x + width * 0.5f) - 2, (int)y - 2000, 4, 2000, 0.0f, 0x2E5A6AFF, kFillModeSolid);
}
