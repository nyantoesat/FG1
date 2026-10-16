#include "Camera.h"
#include "Config.h"

void Camera::Initialize(float worldWidth, float worldHeight, const Vector2& target) {
	worldWidth_ = worldWidth;
	worldHeight_ = worldHeight;
	lookDown_ = 0.0f;
	pos_ = Desired(target); // start already on the target (no slide at the start)
	ClampToWorld();
}

Vector2 Camera::Desired(const Vector2& target) const {
	// Keep the player a bit ABOVE the screen center -> you see more of what is below you
	return {target.x - kWindowWidth * 0.5f, target.y + lookDown_ - kWindowHeight * 0.4f};
}

void Camera::Update(const Vector2& target, float fallSpeed) {
	// Falling fast -> slowly shift the view further down
	float wantLookDown = fallSpeed > 4.0f ? fallSpeed * 12.0f : 0.0f;
	lookDown_ = Lerp(lookDown_, wantLookDown, 0.05f);

	// Smooth follow: move 15% of the remaining distance each frame
	Vector2 desired = Desired(target);
	pos_.x = Lerp(pos_.x, desired.x, 0.15f);
	pos_.y = Lerp(pos_.y, desired.y, 0.15f);
	ClampToWorld();
}

void Camera::ClampToWorld() {
	// X: world narrower than the screen -> center it
	float maxX = worldWidth_ - kWindowWidth;
	pos_.x = (maxX < 0.0f) ? maxX * 0.5f : Clamp(pos_.x, 0.0f, maxX);

	// Y: never show above the top or below the bottom of the map
	float maxY = worldHeight_ - kWindowHeight;
	pos_.y = (maxY < 0.0f) ? maxY * 0.5f : Clamp(pos_.y, 0.0f, maxY);
}