
#include "GameScene.h"

#include <Novice.h>
#include <algorithm>

namespace {

int RockInterval(int c, int w) { return (std::max)(25, 90 - 7 * c - 10 * w); }

int ArrowInterval(int c, int w) { return (std::max)(45, 170 - 12 * c - 15 * w); }

int BatInterval(int c, int w) { return (std::max)(150, 480 - 35 * c - 40 * w); }

int MaxBats(int c, int w) { return 1 + c / 2 + w; }

float BatSpeed(int c) { return 1.8f + c * 0.15f; }

template<typename T> void RemoveDead(std::vector<T>& list) {
	list.erase(std::remove_if(list.begin(), list.end(), [](const T& item) { return !item.alive; }), list.end());
}

} // namespace

// ==================== 初期化 ====================

void GameScene::Initialize(int totalRounds, int totalLives) {
	map_.Initialize();

	totalRounds_ = (std::max)(1, totalRounds);

	circle_ = 0;
	batsKilled_ = 0;
	cleared_ = false;

	shieldCharges_ = 0;
	appliedHealLevel_ = 0;
	enemySpeedScale_ = 1.0f;
	damageTakenThisFrame_ = false;

	// タイトル画面で設定したHPを反映
	player_.SetMaxHearts((std::max)(1, totalLives));
	player_.SetMovementMultiplier(1.0f, 1.0f);

	cage_.SetWidth(kCageWidth);
	cage_.SetBreakEnabled(false);

	StartCircle();
}

// ==================== スキル適用 ====================

void GameScene::ApplySkills(const SkillManager& skills) {
	const int speedLevel = skills.GetSkillLevel(SkillType::SpeedUp);

	const int jumpLevel = skills.GetSkillLevel(SkillType::JumpUp);

	const int shieldLevel = skills.GetSkillLevel(SkillType::Shield);

	const int slowLevel = skills.GetSkillLevel(SkillType::SlowDown);

	const int floorLevel = skills.GetSkillLevel(SkillType::FloorUp);

	const int healLevel = skills.GetSkillLevel(SkillType::Heal);

	player_.SetMovementMultiplier(1.0f + speedLevel * 0.15f, 1.0f + jumpLevel * 0.15f);

	// シールド数
	shieldCharges_ = shieldLevel;

	// 敵の速度低下
	enemySpeedScale_ = (std::max)(0.5f, 1.0f - slowLevel * 0.1f);

	// 足場の横幅拡張
	const float newWidth = kCageWidth * (1.0f + floorLevel * 0.2f);

	const float maxWidth = kShaftRight - kShaftLeft - 20.0f;

	cage_.SetWidth((std::min)(newWidth, maxWidth));

	// HEALを新しく獲得した分だけHP回復
	if (healLevel > appliedHealLevel_) {
		const int newHealCount = healLevel - appliedHealLevel_;

		for (int i = 0; i < newHealCount; i++) {
			player_.RestoreOneHeart();
		}

		appliedHealLevel_ = healLevel;
	}
}

// ==================== スキル選択後 ====================

void GameScene::ResumeAfterSkill(const SkillManager& skills) {
	if (state_ != State::SkillPending) {
		return;
	}

	// 獲得したスキルを反映
	ApplySkills(skills);

	// 敵・落石・矢をすべて削除
	ClearHazards();

	// トゲを初期状態に戻す
	spike_.Initialize(cage_);

	// 崩れる床を元に戻す
	cage_.ResetFloor();

	// プレイヤーを足場の初期位置に戻す
	PutPlayerOnCage();

	// ラウンド・WAVE・残り時間・HPは維持
	damageTakenThisFrame_ = false;

	// 1秒間停止してから再開
	state_ = State::SkillReady;
	stateTimer_ = 0;
}

// ==================== ラウンド開始 ====================

void GameScene::StartCircle() {
	wave_ = 0;

	waveTimer_ = kWaveFrames;
	breakTimer_ = kWaveBreakFrames;

	rockTimer_ = 30;
	arrowTimer_ = 60;
	batTimer_ = 120;

	cage_.y = kCageRestY;
	cage_.vy = 0.0f;

	cage_.SetBreakEnabled(circle_ >= 3);
	cage_.ResetFloor();

	spike_.Initialize(cage_);

	ClearHazards();
	PutPlayerOnCage();

	// ラウンドが進んでもHPは回復しない
	state_ = State::Play;
	stateTimer_ = 0;
	damageTakenThisFrame_ = false;
}

void GameScene::PutPlayerOnCage() { player_.Spawn(cage_.x + cage_.GetWidth() * 0.5f - kPlayerWidth * 0.5f, cage_.y - kPlayerHeight); }

// ==================== 障害物削除 ====================

void GameScene::ClearHazards() {
	rocks_.clear();
	arrows_.clear();
	bats_.clear();
}

// ==================== ダメージ ====================

bool GameScene::TakeDamage() {
	if (damageTakenThisFrame_) {
		return false;
	}

	const int before = player_.GetHearts();

	if (!player_.Damage()) {
		return false;
	}

	// シールドでダメージを防ぐ
	if (shieldCharges_ > 0) {
		shieldCharges_--;
		player_.RestoreOneHeart();
		return true;
	}

	if (player_.GetHearts() < before) {
		damageTakenThisFrame_ = true;
	}

	return true;
}

// ==================== 被弾後の状態 ====================

void GameScene::HandleDamageResult() {
	if (!damageTakenThisFrame_) {
		return;
	}

	if (player_.GetHearts() <= 0) {
		// HP0ならゲームオーバー
		state_ = State::GameOver;
		ClearHazards();
	} else {
		// HPが残っていればスキル選択
		state_ = State::SkillPending;
	}

	stateTimer_ = 0;
}

// ==================== 更新 ====================

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

	case State::SkillReady:
		// 1秒間はプレイヤー・敵・WAVEを停止
		if (stateTimer_ >= kSkillReadyFrames) {
			state_ = State::Play;
			stateTimer_ = 0;
		}
		break;

	case State::SkillPending:
	case State::GameOver:
		break;
	}
}

// ==================== プレイ中 ====================

void GameScene::UpdatePlay() {
	damageTakenThisFrame_ = false;

	cage_.vy = 0.0f;
	player_.Update(map_, cage_, true);

	Rect p = player_.GetRect();

	// 崩れる床
	if (circle_ >= 3) {
		const bool onBreakFloor = player_.IsOnGround() && cage_.IsOnBreakFloor(p.x, p.x + p.w);

		cage_.UpdateFloor(onBreakFloor);
	}

	// トゲ
	if (circle_ >= 2) {
		spike_.Update(cage_);

		if (spike_.IsDangerous() && Intersects(player_.GetRect(), spike_.GetRect())) {
			TakeDamage();
		}
	}

	// 落下ダメージ
	if (player_.FellOut()) {
		const int before = player_.GetHearts();

		player_.LoseHeart();

		if (player_.GetHearts() < before) {
			damageTakenThisFrame_ = true;
		}

		PutPlayerOnCage();
	}

	if (damageTakenThisFrame_) {
		HandleDamageResult();
		return;
	}

	UpdateWaves();

	if (state_ != State::Play) {
		return;
	}

	for (Rock& r : rocks_) {
		r.Update(cage_, enemySpeedScale_);
	}

	for (Arrow& a : arrows_) {
		a.Update(enemySpeedScale_);
	}

	for (Bat& b : bats_) {
		b.Update(player_.GetCenter(), enemySpeedScale_);
	}

	CheckHits();

	RemoveDead(rocks_);
	RemoveDead(arrows_);
	RemoveDead(bats_);

	if (damageTakenThisFrame_) {
		HandleDamageResult();
	}
}

// ==================== ウェーブ ====================

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

// ==================== 障害物生成 ====================

void GameScene::SpawnHazards() {
	if (--rockTimer_ <= 0) {
		SpawnRock();
		rockTimer_ = RockInterval(circle_, wave_);
	}

	if (circle_ >= 1 && --arrowTimer_ <= 0) {
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

// ==================== 当たり判定 ====================

void GameScene::CheckHits() {
	Rect p = player_.GetRect();

	for (Rock& r : rocks_) {
		if (damageTakenThisFrame_) {
			break;
		}

		if (r.IsDangerous() && Intersects(p, r.GetRect())) {
			TakeDamage();
		}
	}

	for (Arrow& a : arrows_) {
		if (damageTakenThisFrame_) {
			break;
		}

		if (a.IsDangerous() && Intersects(p, a.GetRect())) {
			if (TakeDamage()) {
				a.alive = false;
			}
		}
	}

	for (Bat& b : bats_) {
		if (damageTakenThisFrame_) {
			break;
		}

		Rect br = b.GetRect();

		if (!Intersects(p, br)) {
			continue;
		}

		const bool stomp = player_.GetVelocity().y > 0.0f && player_.GetPrevBottom() <= br.y + 10.0f;

		if (stomp) {
			b.alive = false;
			batsKilled_++;
			player_.Bounce();
		} else {
			TakeDamage();
		}
	}
}

// ==================== 描画 ====================

void GameScene::Draw() {
	Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, 0x1C2033FF, kFillModeSolid);

	map_.Draw();
	cage_.Draw();

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

	// READY・GOの表示なし
}

// ==================== HUD ====================

void GameScene::DrawHud() const {
	const int hearts = player_.GetHearts();
	const int maxHearts = player_.GetMaxHearts();

	Novice::ScreenPrintf(20, 20, "ROUND %d / %d", (std::min)(circle_ + 1, totalRounds_), totalRounds_);

	Novice::ScreenPrintf(20, 45, "HP %d / %d", hearts, maxHearts);

	for (int i = 0; i < maxHearts; i++) {
		const unsigned int color = i < hearts ? 0xFF5555FF : 0x444455FF;

		Novice::DrawBox(20 + i * 38, 70, 28, 24, 0.0f, color, kFillModeSolid);
	}

	Novice::ScreenPrintf(20, 110, "SHIELD %d", shieldCharges_);

	Novice::ScreenPrintf(20, 135, "BATS %d", batsKilled_);

	Novice::ScreenPrintf(1000, 20, "WAVE %d / %d", (std::min)(wave_ + 1, kWavesPerCircle), kWavesPerCircle);

	if (state_ == State::Play) {
		const float t = breakTimer_ > 0 ? 1.0f : (float)waveTimer_ / kWaveFrames;

		Novice::DrawBox(1000, 44, 200, 8, 0.0f, 0x3E4A5AFF, kFillModeSolid);

		Novice::DrawBox(1000, 44, (int)(200.0f * t), 8, 0.0f, 0x8FDCC8FF, kFillModeSolid);

		if (breakTimer_ > 0) {
			Novice::ScreenPrintf(590, 300, "ROUND %d", circle_ + 1);

			Novice::ScreenPrintf(595, 320, "WAVE %d", wave_ + 1);
		}
	}

	if (state_ == State::Descend || state_ == State::Arrive) {
		Novice::ScreenPrintf(570, 300, "GOING DOWN...");
	}
}

// ==================== 状態取得 ====================

bool GameScene::IsCleared() const { return cleared_; }

bool GameScene::IsGameOver() const { return state_ == State::GameOver; }

bool GameScene::NeedsSkillSelection() const { return state_ == State::SkillPending; }

int GameScene::GetTotalRounds() const { return totalRounds_; }

int GameScene::GetReachedRound() const { return (std::min)(circle_ + 1, totalRounds_); }
