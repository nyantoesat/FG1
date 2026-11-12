
#include "GameScene.h"
#include <Novice.h>
#include <algorithm>

namespace {

int RockInterval(int circle, int wave) { return (std::max)(25, 90 - circle * 7 - wave * 10); }

int ArrowInterval(int circle, int wave) { return (std::max)(45, 170 - circle * 12 - wave * 15); }

int BatInterval(int circle, int wave) { return (std::max)(150, 480 - circle * 35 - wave * 40); }

int MaxBats(int circle, int wave) { return 1 + circle / 2 + wave; }

float BatSpeed(int circle) { return 1.8f + circle * 0.15f; }

bool ArrowsEnabled(int circle) { return circle >= 1; }

template<typename T> void RemoveDead(std::vector<T>& list) {
	list.erase(std::remove_if(list.begin(), list.end(), [](const T& t) { return !t.alive; }), list.end());
}

} // namespace

// ===================== 初期化 =====================

void GameScene::Initialize(int totalRounds, int totalLives) {
	map_.Initialize();

	totalRounds_ = (std::max)(1, totalRounds);
	baseLives_ = (std::max)(1, totalLives);

	circle_ = 0;
	batsKilled_ = 0;
	cleared_ = false;

	shieldCharges_ = 0;
	enemySpeedScale_ = 1.0f;

	player_.SetMaxHearts(baseLives_);
	player_.SetMovementMultiplier(1.0f, 1.0f);

	cage_.SetWidth(kCageWidth);
	cage_.SetBreakEnabled(false);

	StartCircle();
}

// ===================== スキル =====================

void GameScene::ApplySkills(const SkillManager& skills) {
	int lifeLevel = skills.GetSkillLevel(SkillType::LifeUp);

	int speedLevel = skills.GetSkillLevel(SkillType::SpeedUp);

	int jumpLevel = skills.GetSkillLevel(SkillType::JumpUp);

	int shieldLevel = skills.GetSkillLevel(SkillType::Shield);

	int slowLevel = skills.GetSkillLevel(SkillType::SlowDown);

	int floorLevel = skills.GetSkillLevel(SkillType::FloorUp);

	// 最大ライフ増加
	player_.SetMaxHearts(baseLives_ + lifeLevel);

	// 移動速度・ジャンプ力
	player_.SetMovementMultiplier(1.0f + speedLevel * 0.15f, 1.0f + jumpLevel * 0.15f);

	// シールド
	shieldCharges_ = shieldLevel;

	// 敵の移動速度を低下
	enemySpeedScale_ = (std::max)(0.5f, 1.0f - slowLevel * 0.1f);

	// 足場を左右均等に拡張
	float newWidth = kCageWidth * (1.0f + floorLevel * 0.2f);

	float maxWidth = kShaftRight - kShaftLeft - 20.0f;

	cage_.SetWidth((std::min)(newWidth, maxWidth));
}

// 死亡したラウンドから再開
void GameScene::RestartCurrentRound(const SkillManager& skills) {
	ApplySkills(skills);

	// circle_は変更しない
	StartCircle();
}

// ===================== ラウンド =====================

void GameScene::StartCircle() {
	wave_ = 0;

	waveTimer_ = kWaveFrames;
	breakTimer_ = kWaveBreakFrames;

	rockTimer_ = 30;
	arrowTimer_ = 60;
	batTimer_ = 120;

	cage_.y = kCageRestY;
	cage_.vy = 0.0f;

	// ラウンド4から崩れる床
	cage_.SetBreakEnabled(circle_ >= 3);
	cage_.ResetFloor();

	// トゲ床
	spike_.Initialize(cage_);

	ClearHazards();
	PutPlayerOnCage();

	state_ = State::Play;
	stateTimer_ = 0;
}

// プレイヤーを足場中央に配置
void GameScene::PutPlayerOnCage() { player_.Spawn(cage_.x + cage_.GetWidth() * 0.5f - kPlayerWidth * 0.5f, cage_.y - kPlayerHeight); }

void GameScene::ClearHazards() {
	rocks_.clear();
	arrows_.clear();
	bats_.clear();
}

// ===================== ダメージ =====================

// ダメージが通った場合はtrue
bool GameScene::TakeDamage() {
	int oldHearts = player_.GetHearts();

	// Player::Damageで無敵時間を管理
	if (!player_.Damage()) {
		return false;
	}

	if (shieldCharges_ > 0) {
		shieldCharges_--;

		// シールドがライフ減少を打ち消す
		player_.RestoreOneHeart();
	}

	return player_.GetHearts() < oldHearts || shieldCharges_ >= 0;
}

// ===================== 更新 =====================

void GameScene::Update() {
	stateTimer_++;

	switch (state_) {

	case State::Play:
		UpdatePlay();
		break;

	case State::Descend:
		cage_.vy = 5.0f;
		cage_.y += cage_.vy;

		PutPlayerOnCage();

		if (cage_.y > kWindowHeight + 40.0f) {
			circle_++;

			if (circle_ >= totalRounds_) {
				cleared_ = true;
			} else {
				cage_.y = -kCageHeight;
				state_ = State::Arrive;
				stateTimer_ = 0;
			}
		}
		break;

	case State::Arrive:
		cage_.vy = 6.0f;
		cage_.y += cage_.vy;

		PutPlayerOnCage();

		if (cage_.y >= kCageRestY) {
			StartCircle();
		}
		break;

	case State::GameOver:
		break;
	}
}

// ===================== プレイ中 =====================

void GameScene::UpdatePlay() {
	cage_.vy = 0.0f;

	player_.Update(map_, cage_, true);

	Rect p = player_.GetRect();

	// ラウンド4から崩れる床
	if (circle_ >= 3) {
		bool onBreakFloor = player_.IsOnGround() && cage_.IsOnBreakFloor(p.x, p.x + p.w);

		cage_.UpdateFloor(onBreakFloor);
	}

	// ラウンド3からトゲ床
	if (circle_ >= 2) {
		spike_.Update(cage_);

		if (spike_.IsDangerous() && Intersects(player_.GetRect(), spike_.GetRect())) {
			TakeDamage();
		}
	}

	// 落下時の処理
	if (player_.FellOut()) {
		player_.LoseHeart();
		PutPlayerOnCage();
	}

	if (player_.GetHearts() <= 0) {
		ClearHazards();
		state_ = State::GameOver;
		stateTimer_ = 0;
		return;
	}

	UpdateWaves();

	if (state_ != State::Play) {
		return;
	}

	// 落石
	for (Rock& r : rocks_) {
		r.Update(cage_, enemySpeedScale_);
	}

	// 矢
	for (Arrow& a : arrows_) {
		a.Update(enemySpeedScale_);
	}

	// コウモリ
	for (Bat& b : bats_) {
		b.Update(player_.GetCenter(), enemySpeedScale_);
	}

	CheckHits();

	RemoveDead(rocks_);
	RemoveDead(arrows_);
	RemoveDead(bats_);

	if (player_.GetHearts() <= 0) {
		ClearHazards();
		state_ = State::GameOver;
		stateTimer_ = 0;
	}
}

// ===================== ウェーブ =====================

void GameScene::UpdateWaves() {
	if (breakTimer_ > 0) {
		breakTimer_--;
		return;
	}

	SpawnHazards();
	waveTimer_--;

	if (waveTimer_ <= 0) {
		ClearHazards();
		wave_++;

		if (wave_ >= kWavesPerCircle) {
			state_ = State::Descend;
			stateTimer_ = 0;
		} else {
			waveTimer_ = kWaveFrames;
			breakTimer_ = kWaveBreakFrames;

			rockTimer_ = 30;
			arrowTimer_ = 60;
			batTimer_ = 120;
		}
	}
}

// ===================== 障害物生成 =====================

void GameScene::SpawnHazards() {
	if (--rockTimer_ <= 0) {
		SpawnRock();
		rockTimer_ = RockInterval(circle_, wave_);
	}

	if (ArrowsEnabled(circle_) && --arrowTimer_ <= 0) {
		SpawnArrow();
		arrowTimer_ = ArrowInterval(circle_, wave_);
	}

	if (--batTimer_ <= 0) {
		if ((int)bats_.size() < MaxBats(circle_, wave_)) {
			SpawnBat();
		}

		batTimer_ = BatInterval(circle_, wave_);
	}
}

void GameScene::SpawnRock() {
	Rock r;

	r.x = player_.GetCenter().x - Rock::kSize * 0.5f + RandomFloat(-40.0f, 40.0f);

	r.x = Clamp(r.x, kShaftLeft, kShaftRight - Rock::kSize);

	r.y = -Rock::kSize;
	r.vy = 0.0f;

	rocks_.push_back(r);
}

void GameScene::SpawnArrow() {
	Arrow a;

	a.dir = RandomInt(0, 1) == 0 ? 1 : -1;

	a.x = a.dir > 0 ? kShaftLeft - Arrow::kLength : kShaftRight;

	a.y = Clamp(player_.GetCenter().y + RandomFloat(-80.0f, 80.0f), 40.0f, kCageRestY - 10.0f);

	arrows_.push_back(a);
}

void GameScene::SpawnBat() {
	Bat b;

	b.pos = {RandomFloat(kShaftLeft, kShaftRight - Bat::kWidth), -Bat::kHeight};

	b.speed = BatSpeed(circle_);

	bats_.push_back(b);
}

// ===================== 当たり判定 =====================

void GameScene::CheckHits() {
	Rect p = player_.GetRect();

	// 落石
	for (Rock& r : rocks_) {
		if (r.IsDangerous() && Intersects(p, r.GetRect())) {
			TakeDamage();
		}
	}

	// 矢
	for (Arrow& a : arrows_) {
		if (a.IsDangerous() && Intersects(p, a.GetRect())) {

			if (TakeDamage()) {
				a.alive = false;
			}
		}
	}

	// コウモリ
	for (Bat& b : bats_) {
		Rect br = b.GetRect();

		if (!Intersects(p, br)) {
			continue;
		}

		bool stomp = player_.GetVelocity().y > 0.0f && player_.GetPrevBottom() <= br.y + 10.0f;

		if (stomp) {
			b.alive = false;
			batsKilled_++;
			player_.Bounce();
		} else {
			TakeDamage();
		}
	}
}

// ===================== 描画 =====================

void GameScene::Draw() {
	Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, 0x1C2033FF, kFillModeSolid);

	map_.Draw();
	cage_.Draw();

	// ラウンド3から表示
	if (state_ == State::Play && circle_ >= 2) {
		spike_.Draw();
	}

	for (const Rock& r : rocks_) {
		r.Draw();
	}

	for (const Arrow& a : arrows_) {
		a.Draw();
	}

	for (const Bat& b : bats_) {
		b.Draw();
	}

	player_.Draw();
	DrawHud();
}

// ===================== HUD =====================

void GameScene::DrawHud() const {
	int hearts = player_.GetHearts();
	int maxHearts = player_.GetMaxHearts();

	for (int i = 0; i < maxHearts; i++) {
		unsigned int color = i < hearts ? 0x8FDCC8FF : 0x3E4A5AFF;

		Novice::DrawBox(640 - maxHearts * 20 + i * 40 + 6, 20, 28, 24, 0.0f, color, kFillModeSolid);
	}

	Novice::ScreenPrintf(20, 20, "ROUND %d / %d", (std::min)(circle_ + 1, totalRounds_), totalRounds_);

	Novice::ScreenPrintf(20, 40, "LIVES %d", hearts);

	Novice::ScreenPrintf(20, 60, "BATS %d", batsKilled_);

	Novice::ScreenPrintf(20, 80, "SHIELD %d", shieldCharges_);

	Novice::ScreenPrintf(1000, 20, "WAVE %d / %d", (std::min)(wave_ + 1, kWavesPerCircle), kWavesPerCircle);

	if (state_ == State::Play) {
		float t = breakTimer_ > 0 ? 1.0f : (float)waveTimer_ / kWaveFrames;

		Novice::DrawBox(1000, 44, 200, 8, 0.0f, 0x3E4A5AFF, kFillModeSolid);

		Novice::DrawBox(1000, 44, (int)(200 * t), 8, 0.0f, 0x8FDCC8FF, kFillModeSolid);

		if (breakTimer_ > 0) {
			Novice::ScreenPrintf(590, 300, "ROUND %d", circle_ + 1);

			Novice::ScreenPrintf(595, 320, "WAVE %d", wave_ + 1);
		}
	}

	if (state_ == State::Descend || state_ == State::Arrive) {
		Novice::ScreenPrintf(570, 300, "GOING DOWN...");
	}
}

// ===================== 状態取得 =====================

bool GameScene::IsCleared() const { return cleared_; }

bool GameScene::IsGameOver() const { return state_ == State::GameOver; }

int GameScene::GetTotalRounds() const { return totalRounds_; }

int GameScene::GetReachedRound() const { return (std::min)(circle_ + 1, totalRounds_); }
