
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
}

void Player::Update(const Map& map, const Cage& cage, bool controllable) {
	prevBottom_ = pos_.y + kPlayerHeight;

	if (onGround_) {
		pos_.y += cage.vy;
		prevBottom_ += cage.vy;
	}

	HandleHorizontal(controllable);
	HandleJump(controllable);
	ApplyGravity(controllable);

	MoveX(map);
	MoveY(map, cage);

	if (invincibleTimer_ > 0) {
		invincibleTimer_--;
	}
}

void Player::HandleHorizontal(bool controllable) {
	int dir = 0;

	if (controllable) {
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
	} else {
		vel_.x *= onGround_ ? kPlayerGroundFriction : kPlayerAirFriction;

		if (std::fabs(vel_.x) < 0.1f) {
			vel_.x = 0.0f;
		}
	}
}

void Player::HandleJump(bool controllable) {
	// ジャンプ入力を一定時間保存
	if (controllable && Input::JumpTrigger()) {
		jumpBufferTimer_ = kJumpBufferFrames;
	} else if (jumpBufferTimer_ > 0) {
		jumpBufferTimer_--;
	}

	// 足場にいるときだけジャンプ可能時間を更新
	if (onGround_) {
		coyoteTimer_ = kCoyoteFrames;
	} else if (coyoteTimer_ > 0) {
		coyoteTimer_--;
	}

	if (jumpBufferTimer_ == 0) {
		return;
	}

	// 地面にいる、または足場から離れた直後だけジャンプ
	if (coyoteTimer_ > 0) {
		vel_.y = -kJumpPower;
	} else {
		return;
	}

	jumpBufferTimer_ = 0;
	coyoteTimer_ = 0;
	onGround_ = false;
}

void Player::ApplyGravity(bool controllable) {
	float gravity = kGravity;

	if (vel_.y < 0.0f && !(controllable && Input::JumpPress())) {
		gravity *= 2.2f;
	}

	vel_.y += gravity;

	if (vel_.y > kMaxFallSpeed) {
		vel_.y = kMaxFallSpeed;
	}
}

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
	(void)map;

	pos_.y += vel_.y;
	onGround_ = false;

	Rect c = cage.GetRect();

	bool overlapX = pos_.x + kPlayerWidth > c.x && pos_.x < c.x + c.w;

	float bottom = pos_.y + kPlayerHeight;

	// 足場の上に着地
	if (overlapX && vel_.y >= 0.0f && prevBottom_ <= c.y + 1.0f && bottom >= c.y) {

		pos_.y = c.y - kPlayerHeight;
		vel_.y = 0.0f;
		onGround_ = true;
	}

	// 画面上部への移動を制限
	if (pos_.y < 0.0f) {
		pos_.y = 0.0f;

		if (vel_.y < 0.0f) {
			vel_.y = 0.0f;
		}
	}
}

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
}

void Player::Draw() const {
	if (invincibleTimer_ > 0 && (invincibleTimer_ / 4) % 2 == 0) {
		return;
	}

	Novice::DrawBox((int)pos_.x, (int)pos_.y, (int)kPlayerWidth, (int)kPlayerHeight, 0.0f, 0xE8F8F0FF, kFillModeSolid);

	int eyeX = facing_ > 0 ? (int)(pos_.x + kPlayerWidth) - 8 : (int)pos_.x + 4;

	Novice::DrawBox(eyeX, (int)pos_.y + 8, 4, 6, 0.0f, 0x1C2033FF, kFillModeSolid);
}
