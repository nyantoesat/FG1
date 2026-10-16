#include "Hazards.h"
#include "Cage.h"
#include "Config.h"
#include <Novice.h>
#include <cmath>

// ===================== Rock =====================

void Rock::Update(const Cage& cage) {
	if (warnTimer > 0) {
		warnTimer--;
		return;
	}
	vy += 0.4f;
	if (vy > 10.0f) {
		vy = 10.0f;
	}
	y += vy;

	// Breaks on the cage roof
	Rect c = cage.GetRect();
	if (x + kSize > c.x && x < c.x + c.w && y + kSize >= c.y && y < c.y + 20.0f) {
		alive = false;
	}
	if (y > kWindowHeight) {
		alive = false;
	}
}

void Rock::Draw() const {
	// PLACEHOLDER ART
	if (warnTimer > 0) {
		// blinking warning column + marker at the top
		if ((warnTimer / 6) % 2 == 0) {
			Novice::DrawBox((int)x, 0, (int)kSize, kWindowHeight, 0.0f, 0xF0505030, kFillModeSolid);
		}
		Novice::DrawBox((int)x, 0, (int)kSize, 10, 0.0f, 0xF05050FF, kFillModeSolid);
		return;
	}
	Novice::DrawBox((int)x, (int)y, (int)kSize, (int)kSize, 0.0f, 0xF05050FF, kFillModeSolid);
}

// ===================== Arrow =====================

void Arrow::Update() {
	if (warnTimer > 0) {
		warnTimer--;
		return;
	}
	x += dir * kSpeed;
	if (x > kShaftRight || x + kLength < kShaftLeft) {
		alive = false;
	}
}

void Arrow::Draw() const {
	// PLACEHOLDER ART
	if (warnTimer > 0) {
		// blinking marker on the wall the arrow comes from
		if ((warnTimer / 5) % 2 == 0) {
			int wx = dir > 0 ? (int)kShaftLeft - 12 : (int)kShaftRight;
			Novice::DrawBox(wx, (int)y - 6, 12, (int)kThickness + 12, 0.0f, 0xFFD040FF, kFillModeSolid);
		}
		return;
	}
	Novice::DrawBox((int)x, (int)y, (int)kLength, (int)kThickness, 0.0f, 0xFFD040FF, kFillModeSolid);
}

// ===================== Bat =====================

void Bat::Update(const Vector2& target) {
	// Steer toward the player (turns slowly, so you can dodge)
	float dx = target.x - (pos.x + kWidth * 0.5f);
	float dy = target.y - (pos.y + kHeight * 0.5f);
	float len = std::sqrt(dx * dx + dy * dy);
	if (len > 1.0f) {
		vel.x += (dx / len * speed - vel.x) * 0.04f;
		vel.y += (dy / len * speed - vel.y) * 0.04f;
	}
	phase += 0.15f;
	pos.x += vel.x;
	pos.y += vel.y + std::sin(phase) * 1.2f; // wavy flight

	// Stay inside the shaft
	pos.x = Clamp(pos.x, kShaftLeft, kShaftRight - kWidth);
}

void Bat::Draw() const {
	// PLACEHOLDER ART
	Novice::DrawBox((int)pos.x, (int)pos.y, (int)kWidth, (int)kHeight, 0.0f, 0x9A60E0FF, kFillModeSolid);
}