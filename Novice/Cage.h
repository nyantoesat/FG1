
#pragma once

#include "Config.h"
#include "MathUtil.h"

struct Cage {
	float x = kCageX;
	float y = kCageRestY;
	float vy = 0.0f;

	float width = kCageWidth;
	bool breakEnabled = false;

	static constexpr float kBreakWidth = 100.0f;
	static constexpr int kBreakFrames = 60;
	static constexpr int kRestoreFrames = 180;

	enum class FloorState { Normal, Cracking, Broken };

	FloorState floorState = FloorState::Normal;
	int floorTimer = 0;

	float GetWidth() const { return width; }

	void SetWidth(float newWidth) {
		width = newWidth;

		// 元の足場の中心を維持
		x = kCageX + (kCageWidth - width) * 0.5f;
	}

	void SetBreakEnabled(bool enabled) {
		breakEnabled = enabled;

		if (!enabled) {
			ResetFloor();
		}
	}

	Rect GetRect() const { return {x, y, width, kCageHeight}; }

	Rect GetBreakRect() const { return {x + width - kBreakWidth - 30.0f, y, kBreakWidth, 8.0f}; }

	bool CanLand(float left, float right) const;
	bool IsOnBreakFloor(float left, float right) const;

	void UpdateFloor(bool playerStanding);
	void ResetFloor();
	void Draw() const;
};
