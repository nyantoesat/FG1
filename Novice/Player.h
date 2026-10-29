
#pragma once

#include "Config.h"
#include "MathUtil.h"

class Map;
struct Cage;

class Player {
public:
	// 初期位置を設定
	void Spawn(float x, float y);

	// 更新・描画
	void Update(const Map& map, const Cage& cage, bool controllable);
	void Draw() const;

	// ダメージ
	bool Damage();
	void LoseHeart();

	// 最大ハート数を設定
	void SetMaxHearts(int hearts) {
		maxHearts_ = hearts;
		hearts_ = maxHearts_;
		invincibleTimer_ = 0;
	}

	// ハートを全回復
	void RefillHearts() {
		hearts_ = maxHearts_;
		invincibleTimer_ = 0;
	}

	// 現在のハート数
	int GetHearts() const { return hearts_; }

	// 最大ハート数
	int GetMaxHearts() const { return maxHearts_; }

	// 敵を踏んだときの跳ね返り
	void Bounce();

	// 当たり判定
	Rect GetRect() const { return {pos_.x, pos_.y, kPlayerWidth, kPlayerHeight}; }

	// プレイヤーの中心座標
	Vector2 GetCenter() const { return {pos_.x + kPlayerWidth * 0.5f, pos_.y + kPlayerHeight * 0.5f}; }

	// 移動速度
	const Vector2& GetVelocity() const { return vel_; }

	// 前フレームの足元
	float GetPrevBottom() const { return prevBottom_; }

	// 画面外に落下したか
	bool FellOut() const { return pos_.y > kWindowHeight + 40.0f; }

private:
	// 移動処理
	void HandleHorizontal(bool controllable);
	void HandleJump(bool controllable);
	void ApplyGravity(bool controllable);

	void MoveX(const Map& map);
	void MoveY(const Map& map, const Cage& cage);

	// 座標と速度
	Vector2 pos_ = {0.0f, 0.0f};
	Vector2 vel_ = {0.0f, 0.0f};

	float prevBottom_ = 0.0f;
	int facing_ = 1;

	bool onGround_ = false;

	// ジャンプ補助
	int coyoteTimer_ = 0;
	int jumpBufferTimer_ = 0;

	// 最大ハート数
	int maxHearts_ = kMaxHearts;

	// 現在のハート数
	int hearts_ = kMaxHearts;

	// 無敵時間
	int invincibleTimer_ = 0;
};
