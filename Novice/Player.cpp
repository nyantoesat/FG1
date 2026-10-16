#include "Player.h"
#include "Cage.h"
#include "Input.h"
#include "Map.h"
#include <Novice.h>
#include <cmath>

namespace {
int ToTile(float pixel) { return (int)std::floor(pixel / kTileSize); }
} // namespace

void Player::Spawn(float x, float y) {
	pos_ = {x, y};
	vel_ = {0.0f, 0.0f};
	prevBottom_ = y + kPlayerHeight;
	onGround_ = false;
	coyoteTimer_ = 0;
	jumpBufferTimer_ = 0;
	wallSide_ = 0;
	wallJumpLock_ = 0;
}

void Player::Update(const Map& map, const Cage& cage, bool controllable) {
	prevBottom_ = pos_.y + kPlayerHeight;

	// Ride the cage: if we stood on it last frame, move with it
	if (onGround_) {
		pos_.y += cage.vy;
		prevBottom_ += cage.vy;
	}

	HandleHorizontal(controllable);
	UpdateWallHang(map, controllable);
	HandleJump(controllable);
	ApplyGravity(controllable);

	MoveX(map);
	MoveY(map, cage);

	if (invincibleTimer_ > 0) {
		invincibleTimer_--;
	}
	if (wallJumpLock_ > 0) {
		wallJumpLock_--;
	}
}

// ---------------------------------------------------------------- movement

void Player::HandleHorizontal(bool controllable) {
	int dir = 0;
	if (controllable && wallJumpLock_ == 0) {
		if (Input::Left()) {
			dir -= 1;
		}
		if (Input::Right()) {
			dir += 1;
		}
	}

	if (dir != 0) {
		float accel = onGround_ ? kPlayerGroundAccel : kPlayerAirAccel;
		vel_.x = Clamp(vel_.x + dir * accel, -kPlayerMaxSpeed, kPlayerMaxSpeed);
		facing_ = dir;
	} else if (wallJumpLock_ == 0) {
		vel_.x *= onGround_ ? kPlayerGroundFriction : kPlayerAirFriction;
		if (std::fabs(vel_.x) < 0.1f) {
			vel_.x = 0.0f;
		}
	}
}

void Player::UpdateWallHang(const Map& map, bool controllable) {
	// Hang on a wall when in the air, touching it, and pressing toward it (or up / down)
	wallSide_ = 0;
	if (!controllable || onGround_ || wallJumpLock_ > 0) {
		return;
	}
	float top = pos_.y + 4.0f;
	float bottom = pos_.y + kPlayerHeight - 4.0f;
	bool touchLeft = map.IsSolidAtPixel(pos_.x - 1.0f, top) || map.IsSolidAtPixel(pos_.x - 1.0f, bottom);
	bool touchRight = map.IsSolidAtPixel(pos_.x + kPlayerWidth + 1.0f, top) || map.IsSolidAtPixel(pos_.x + kPlayerWidth + 1.0f, bottom);
	bool climbKey = Input::Up() || Input::Down();

	if (touchRight && (Input::Right() || climbKey)) {
		wallSide_ = 1;
	} else if (touchLeft && (Input::Left() || climbKey)) {
		wallSide_ = -1;
	}
	if (wallSide_ != 0) {
		facing_ = wallSide_;
	}
}

void Player::HandleJump(bool controllable) {
	// Jump buffer: remember the press for a few frames
	if (controllable && Input::JumpTrigger()) {
		jumpBufferTimer_ = kJumpBufferFrames;
	} else if (jumpBufferTimer_ > 0) {
		jumpBufferTimer_--;
	}

	// Coyote time: still allowed to jump just after walking off the cage
	if (onGround_) {
		coyoteTimer_ = kCoyoteFrames;
	} else if (coyoteTimer_ > 0) {
		coyoteTimer_--;
	}

	if (jumpBufferTimer_ == 0) {
		return;
	}
	if (coyoteTimer_ > 0) {
		// normal jump
		vel_.y = -kJumpPower;
	} else if (wallSide_ != 0) {
		// wall jump: kick away from the wall
		vel_.x = -wallSide_ * kWallJumpX;
		vel_.y = -kWallJumpY;
		facing_ = -wallSide_;
		wallJumpLock_ = kWallJumpLockFrames;
		wallSide_ = 0;
	} else {
		return; // can't jump now, keep the buffer
	}
	jumpBufferTimer_ = 0;
	coyoteTimer_ = 0;
	onGround_ = false;
}

void Player::ApplyGravity(bool controllable) {
	if (wallSide_ != 0) {
		// Hanging: no gravity. W / S climbs, otherwise slide slowly.
		if (Input::Up()) {
			vel_.y = -kClimbUpSpeed;
		} else if (Input::Down()) {
			vel_.y = kClimbDownSpeed;
		} else {
			vel_.y = kWallSlideSpeed;
		}
		vel_.x = 0.0f;
		return;
	}

	float gravity = kGravity;
	if (vel_.y < 0.0f && !(controllable && Input::JumpPress())) {
		gravity *= 2.2f; // released jump early -> short hop
	}
	vel_.y += gravity;
	if (vel_.y > kMaxFallSpeed) {
		vel_.y = kMaxFallSpeed;
	}
}

// ---------------------------------------------------------------- collision

void Player::MoveX(const Map& map) {
	pos_.x += vel_.x;
	int top = ToTile(pos_.y);
	int bottom = ToTile(pos_.y + kPlayerHeight - 0.01f);

	if (vel_.x > 0.0f) {
		int tx = ToTile(pos_.x + kPlayerWidth - 0.01f);
		for (int ty = top; ty <= bottom; ty++) {
			if (map.IsSolid(tx, ty)) {
				pos_.x = tx * (float)kTileSize - kPlayerWidth;
				vel_.x = 0.0f;
				break;
			}
		}
	} else if (vel_.x < 0.0f) {
		int tx = ToTile(pos_.x);
		for (int ty = top; ty <= bottom; ty++) {
			if (map.IsSolid(tx, ty)) {
				pos_.x = (tx + 1) * (float)kTileSize;
				vel_.x = 0.0f;
				break;
			}
		}
	}
}

void Player::MoveY(const Map& map, const Cage& cage) {
	(void)map; // walls are vertical only, so Y only collides with the cage roof
	pos_.y += vel_.y;
	onGround_ = false;

	// Land on the cage roof (only from above, like a one-way platform)
	Rect c = cage.GetRect();
	bool overlapX = pos_.x + kPlayerWidth > c.x && pos_.x < c.x + c.w;
	float bottom = pos_.y + kPlayerHeight;
	if (overlapX && vel_.y >= 0.0f && prevBottom_ <= c.y + 1.0f && bottom >= c.y) {
		pos_.y = c.y - kPlayerHeight;
		vel_.y = 0.0f;
		onGround_ = true;
		wallSide_ = 0;
	}

	// Don't climb out of the top of the screen
	if (pos_.y < 0.0f) {
		pos_.y = 0.0f;
		if (vel_.y < 0.0f) {
			vel_.y = 0.0f;
		}
	}
}

// ---------------------------------------------------------------- health

bool Player::Damage() {
	if (invincibleTimer_ > 0) {
		return false;
	}
	hearts_--;
	invincibleTimer_ = kInvincibleFrames;
	return true;
}

void Player::LoseHeart() {
	hearts_--;
	invincibleTimer_ = kInvincibleFrames;
}

void Player::Bounce() {
	vel_.y = -10.0f;
	onGround_ = false;
	wallSide_ = 0;
}

// ---------------------------------------------------------------- draw

void Player::Draw() const {
	// Blink while invincible
	if (invincibleTimer_ > 0 && (invincibleTimer_ / 4) % 2 == 0) {
		return;
	}
	// PLACEHOLDER ART: replace with your cat sprite later (facing_ tells you which way to flip it)
	Novice::DrawBox((int)pos_.x, (int)pos_.y, (int)kPlayerWidth, (int)kPlayerHeight, 0.0f, 0xE8F8F0FF, kFillModeSolid);
	// small mark showing the facing direction
	int eyeX = facing_ > 0 ? (int)(pos_.x + kPlayerWidth) - 8 : (int)pos_.x + 4;
	Novice::DrawBox(eyeX, (int)pos_.y + 8, 4, 6, 0.0f, 0x1C2033FF, kFillModeSolid);
}