
#include "Hazards.h"
#include "Cage.h"
#include "Config.h"
#include <Novice.h>
#include <cmath>

// ===================== Rock =====================

void Rock::Update(const Cage& cage, float speedScale) {
	if (warnTimer > 0) {
		warnTimer--;
		return;
	}

	// 速度倍率は移動量に反映
	vy += 0.4f * speedScale;

	float maxSpeed = 10.0f * speedScale;

	if (vy > maxSpeed) {
		vy = maxSpeed;
	}

	y += vy;

	Rect c = cage.GetRect();

	if (x + kSize > c.x && x < c.x + c.w && y + kSize >= c.y && y < c.y + 20.0f) {
		alive = false;
	}

	if (y > kWindowHeight) {
		alive = false;
	}
}

void Rock::Draw() const {
	if (warnTimer > 0) {
		if ((warnTimer / 6) % 2 == 0) {
			Novice::DrawBox((int)x, 0, (int)kSize, kWindowHeight, 0.0f, 0xF0505030, kFillModeSolid);
		}

		Novice::DrawBox((int)x, 0, (int)kSize, 10, 0.0f, 0xF05050FF, kFillModeSolid);
		return;
	}

	Novice::DrawBox((int)x, (int)y, (int)kSize, (int)kSize, 0.0f, 0xF05050FF, kFillModeSolid);
}

// ===================== Arrow =====================

void Arrow::Update(float speedScale) {
	if (warnTimer > 0) {
		warnTimer--;
		return;
	}

	x += dir * kSpeed * speedScale;

	if (x > kShaftRight || x + kLength < kShaftLeft) {
		alive = false;
	}
}

void Arrow::Draw() const {
	if (warnTimer > 0) {
		if ((warnTimer / 5) % 2 == 0) {
			int wx = dir > 0 ? (int)kShaftLeft - 12 : (int)kShaftRight;

			Novice::DrawBox(wx, (int)y - 6, 12, (int)kThickness + 12, 0.0f, 0xFFD040FF, kFillModeSolid);
		}
		return;
	}

	Novice::DrawBox((int)x, (int)y, (int)kLength, (int)kThickness, 0.0f, 0xFFD040FF, kFillModeSolid);
}

// ===================== Bat =====================

void Bat::Update(const Vector2& target, float speedScale) {
	float dx = target.x - (pos.x + kWidth * 0.5f);
	float dy = target.y - (pos.y + kHeight * 0.5f);
	float len = std::sqrt(dx * dx + dy * dy);

	float actualSpeed = speed * speedScale;

	if (len > 1.0f) {
		vel.x += (dx / len * actualSpeed - vel.x) * 0.04f;

		vel.y += (dy / len * actualSpeed - vel.y) * 0.04f;
	}

	phase += 0.15f * speedScale;

	pos.x += vel.x;
	pos.y += vel.y + std::sin(phase) * 1.2f * speedScale;

	pos.x = Clamp(pos.x, kShaftLeft, kShaftRight - kWidth);
}

void Bat::Draw() const { Novice::DrawBox((int)pos.x, (int)pos.y, (int)kWidth, (int)kHeight, 0.0f, 0x9A60E0FF, kFillModeSolid); }

// ===================== Spike =====================

void Spike::Initialize(const Cage& cage) {
	x = cage.x + 30.0f;
	y = cage.y;

	state = State::Warning;
	timer = kWarnFrames;
}

void Spike::Update(const Cage& cage) {
	x = cage.x + 30.0f;
	y = cage.y;

	timer--;

	if (timer > 0) {
		return;
	}

	switch (state) {
	case State::Warning:
		state = State::Active;
		timer = kActiveFrames;
		break;

	case State::Active:
		state = State::Rest;
		timer = kRestFrames;
		break;

	case State::Rest:
		state = State::Warning;
		timer = kWarnFrames;
		break;
	}
}

void Spike::Draw() const {
	if (state == State::Rest) {
		return;
	}

	if (state == State::Warning) {
		if ((timer / 6) % 2 == 0) {
			Novice::DrawBox((int)x, (int)y - 5, (int)kWidth, 5, 0.0f, 0xF05050FF, kFillModeSolid);
		}
		return;
	}

	const int spikeCount = 5;
	const float spikeWidth = kWidth / spikeCount;

	for (int i = 0; i < spikeCount; i++) {
		float sx = x + i * spikeWidth;

		Novice::DrawTriangle((int)sx, (int)y, (int)(sx + spikeWidth * 0.5f), (int)(y - kHeight), (int)(sx + spikeWidth), (int)y, 0xF05050FF, kFillModeSolid);
	}
}
