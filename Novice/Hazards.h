#pragma once
#include "MathUtil.h"

struct Cage;

// ===================== Falling rock =====================
// Shows a warning line first, then drops from the top of the screen.
struct Rock {
	static constexpr float kSize = 44.0f;
	static constexpr int kWarnFrames = 60;

	float x = 0.0f;
	float y = -kSize;
	float vy = 0.0f;
	int warnTimer = kWarnFrames;
	bool alive = true;

	void Update(const Cage& cage);
	void Draw() const;
	bool IsDangerous() const { return warnTimer == 0; }
	Rect GetRect() const { return {x, y, kSize, kSize}; }
};

// ===================== Arrow =====================
// Shows a warning on the wall first, then flies across the shaft.
struct Arrow {
	static constexpr float kLength = 40.0f;
	static constexpr float kThickness = 8.0f;
	static constexpr float kSpeed = 8.0f;
	static constexpr int kWarnFrames = 50;

	float x = 0.0f;
	float y = 0.0f;
	int dir = 1; // +1 flies to the right, -1 to the left
	int warnTimer = kWarnFrames;
	bool alive = true;

	void Update();
	void Draw() const;
	bool IsDangerous() const { return warnTimer == 0; }
	Rect GetRect() const { return {x, y, kLength, kThickness}; }
};

// ===================== Bat =====================
// Flies toward the player. Jump on it to kill it.
struct Bat {
	static constexpr float kWidth = 30.0f;
	static constexpr float kHeight = 20.0f;

	Vector2 pos = {0.0f, 0.0f};
	Vector2 vel = {0.0f, 0.0f};
	float speed = 2.0f;
	float phase = 0.0f; // for the wavy flight
	bool alive = true;

	void Update(const Vector2& target);
	void Draw() const;
	Rect GetRect() const { return {pos.x, pos.y, kWidth, kHeight}; }
};