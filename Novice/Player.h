
#pragma once
#include "Config.h"
#include "MathUtil.h"

class Map;
struct Cage;

class Player {
public:
	void Spawn(float x, float y);
	void Update(const Map& map, const Cage& cage, bool controllable);
	void Draw() const;

	bool Damage();
	void LoseHeart();

	void RefillHearts() { hearts_ = kMaxHearts; }

	int GetHearts() const { return hearts_; }

	void Bounce();

	Rect GetRect() const { return {pos_.x, pos_.y, kPlayerWidth, kPlayerHeight}; }

	Vector2 GetCenter() const { return {pos_.x + kPlayerWidth * 0.5f, pos_.y + kPlayerHeight * 0.5f}; }

	const Vector2& GetVelocity() const { return vel_; }

	float GetPrevBottom() const { return prevBottom_; }

	bool FellOut() const { return pos_.y > kWindowHeight + 40.0f; }

private:
	void HandleHorizontal(bool controllable);
	void HandleJump(bool controllable);
	void ApplyGravity(bool controllable);

	void MoveX(const Map& map);
	void MoveY(const Map& map, const Cage& cage);

	Vector2 pos_ = {0.0f, 0.0f};
	Vector2 vel_ = {0.0f, 0.0f};

	float prevBottom_ = 0.0f;
	int facing_ = 1;

	bool onGround_ = false;

	int coyoteTimer_ = 0;
	int jumpBufferTimer_ = 0;

	int hearts_ = kMaxHearts;
	int invincibleTimer_ = 0;
};
