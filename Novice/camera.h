#pragma once
#include "Vector2.h"

// World -> screen conversion. Follows a target smoothly, mostly vertically.
class Camera {
public:
	void Initialize(float worldWidth, float worldHeight, const Vector2& target);
	void Update(const Vector2& target, float fallSpeed);

	// Use these for EVERY draw call in the world
	int ToScreenX(float worldX) const { return (int)(worldX - pos_.x); }
	int ToScreenY(float worldY) const { return (int)(worldY - pos_.y); }
	float GetY() const { return pos_.y; }

private:
	Vector2 Desired(const Vector2& target) const;
	void ClampToWorld();

	Vector2 pos_ = {0.0f, 0.0f}; // top-left of the screen in world coordinates
	float lookDown_ = 0.0f;      // extra offset while falling (see further below)
	float worldWidth_ = 0.0f;
	float worldHeight_ = 0.0f;
};