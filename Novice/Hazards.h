
#pragma once

#include "MathUtil.h"

struct Cage;

// ===================== Rock =====================
struct Rock {
	static constexpr float kSize = 44.0f;
	static constexpr int kWarnFrames = 60;

	float x = 0.0f;
	float y = -kSize;
	float vy = 0.0f;
	int warnTimer = kWarnFrames;
	bool alive = true;

	void Update(const Cage& cage, float speedScale = 1.0f);
	void Draw() const;

	bool IsDangerous() const { return warnTimer == 0; }

	Rect GetRect() const { return {x, y, kSize, kSize}; }
};

// ===================== Arrow =====================
struct Arrow {
	static constexpr float kLength = 40.0f;
	static constexpr float kThickness = 8.0f;
	static constexpr float kSpeed = 8.0f;
	static constexpr int kWarnFrames = 50;

	float x = 0.0f;
	float y = 0.0f;
	int dir = 1;
	int warnTimer = kWarnFrames;
	bool alive = true;

	void Update(float speedScale = 1.0f);
	void Draw() const;

	bool IsDangerous() const { return warnTimer == 0; }

	Rect GetRect() const { return {x, y, kLength, kThickness}; }
};

// ===================== Bat =====================
struct Bat {
	static constexpr float kWidth = 30.0f;
	static constexpr float kHeight = 20.0f;

	Vector2 pos = {0.0f, 0.0f};
	Vector2 vel = {0.0f, 0.0f};

	float speed = 2.0f;
	float phase = 0.0f;
	bool alive = true;

	void Update(const Vector2& target, float speedScale = 1.0f);

	void Draw() const;

	Rect GetRect() const { return {pos.x, pos.y, kWidth, kHeight}; }
};

// ===================== Spike =====================
struct Spike {
	static constexpr float kWidth = 80.0f;
	static constexpr float kHeight = 24.0f;

	static constexpr int kWarnFrames = 60;
	static constexpr int kActiveFrames = 90;
	static constexpr int kRestFrames = 90;

	enum class State { Warning, Active, Rest };

	float x = 0.0f;
	float y = 0.0f;

	State state = State::Warning;
	int timer = kWarnFrames;

	void Initialize(const Cage& cage);
	void Update(const Cage& cage);
	void Draw() const;

	bool IsDangerous() const { return state == State::Active; }

	Rect GetRect() const { return {x, y - kHeight, kWidth, kHeight}; }
};
