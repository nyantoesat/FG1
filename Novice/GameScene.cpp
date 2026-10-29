
#include "GameScene.h"
#include "Input.h"
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

// 初期化
void GameScene::Initialize(int totalRounds) {
	map_.Initialize();

	totalRounds_ = totalRounds;

	circle_ = 0;
	lives_ = kStartLives;
	batsKilled_ = 0;

	cleared_ = false;
	returnToTitle_ = false;

	player_.RefillHearts();

	StartCircle();
}

// ラウンド開始
void GameScene::StartCircle() {
	wave_ = 0;

	waveTimer_ = kWaveFrames;
	breakTimer_ = kWaveBreakFrames;

	rockTimer_ = 30;
	arrowTimer_ = 60;
	batTimer_ = 120;

	cage_.y = kCageRestY;
	cage_.vy = 0.0f;

	ClearHazards();
	PutPlayerOnCage();

	state_ = State::Play;
	stateTimer_ = 0;
}

void GameScene::PutPlayerOnCage() { player_.Spawn(kCageX + kCageWidth * 0.5f - kPlayerWidth * 0.5f, cage_.y - kPlayerHeight); }

void GameScene::ClearHazards() {
	rocks_.clear();
	arrows_.clear();
	bats_.clear();
}

// 更新
void GameScene::Update() {
	stateTimer_++;

	switch (state_) {

	case State::Play:
		UpdatePlay();
		break;

	case State::LifeLost:
		if (stateTimer_ > 90) {
			if (lives_ > 0) {
				player_.RefillHearts();
				StartCircle();
			} else {
				state_ = State::GameOver;
				stateTimer_ = 0;
			}
		}
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
		if (stateTimer_ > 40 && Input::JumpTrigger()) {
			returnToTitle_ = true;
		}
		break;
	}
}

// プレイ中
void GameScene::UpdatePlay() {
	cage_.vy = 0.0f;

	player_.Update(map_, cage_, true);

	UpdateWaves();

	// ウェーブ終了直後は敵の更新をしない
	if (state_ != State::Play) {
		return;
	}

	for (Rock& r : rocks_) {
		r.Update(cage_);
	}

	for (Arrow& a : arrows_) {
		a.Update();
	}

	for (Bat& b : bats_) {
		b.Update(player_.GetCenter());
	}

	CheckHits();

	RemoveDead(rocks_);
	RemoveDead(arrows_);
	RemoveDead(bats_);

	if (player_.FellOut()) {
		player_.LoseHeart();
		PutPlayerOnCage();
	}

	if (player_.GetHearts() <= 0) {
		lives_--;

		state_ = State::LifeLost;
		stateTimer_ = 0;
	}
}

// ウェーブ管理
void GameScene::UpdateWaves() {
	if (breakTimer_ > 0) {
		breakTimer_--;
		return;
	}

	SpawnHazards();

	waveTimer_--;

	if (waveTimer_ <= 0) {
		// ウェーブ終了時に敵と障害物を消す
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

// 障害物の生成
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

// 落石
void GameScene::SpawnRock() {
	Rock r;

	float playerX = player_.GetCenter().x;

	r.x = playerX - Rock::kSize * 0.5f + RandomFloat(-40.0f, 40.0f);

	r.x = Clamp(r.x, kShaftLeft, kShaftRight - Rock::kSize);

	r.y = -Rock::kSize;
	r.vy = 0.0f;

	rocks_.push_back(r);
}

// 矢
void GameScene::SpawnArrow() {
	Arrow a;

	a.dir = RandomInt(0, 1) == 0 ? 1 : -1;

	a.x = a.dir > 0 ? kShaftLeft - Arrow::kLength : kShaftRight;

	a.y = Clamp(player_.GetCenter().y + RandomFloat(-80.0f, 80.0f), 40.0f, kCageRestY - 10.0f);

	arrows_.push_back(a);
}

// Bat
void GameScene::SpawnBat() {
	Bat b;

	b.pos = {RandomFloat(kShaftLeft, kShaftRight - Bat::kWidth), -Bat::kHeight};

	b.speed = BatSpeed(circle_);

	bats_.push_back(b);
}

// 当たり判定
void GameScene::CheckHits() {
	Rect p = player_.GetRect();

	for (Rock& r : rocks_) {
		if (r.IsDangerous() && Intersects(p, r.GetRect())) {
			player_.Damage();
		}
	}

	for (Arrow& a : arrows_) {
		if (a.IsDangerous() && Intersects(p, a.GetRect())) {

			if (player_.Damage()) {
				a.alive = false;
			}
		}
	}

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
			player_.Damage();
		}
	}
}

// 描画
void GameScene::Draw() {
	Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, 0x1C2033FF, kFillModeSolid);

	map_.Draw();
	cage_.Draw();

	for (const Rock& r : rocks_) {
		r.Draw();
	}

	for (const Arrow& a : arrows_) {
		a.Draw();
	}

	for (const Bat& b : bats_) {
		b.Draw();
	}

	if (state_ != State::LifeLost || (stateTimer_ / 6) % 2 == 0) {
		player_.Draw();
	}

	DrawHud();
}

// HUD
void GameScene::DrawHud() const {
	for (int i = 0; i < kMaxHearts; i++) {
		unsigned int color = i < player_.GetHearts() ? 0x8FDCC8FF : 0x3E4A5AFF;

		Novice::DrawBox(640 - kMaxHearts * 20 + i * 40 + 6, 20, 28, 24, 0.0f, color, kFillModeSolid);
	}

	Novice::ScreenPrintf(20, 20, "ROUND %d / %d", (std::min)(circle_ + 1, totalRounds_), totalRounds_);

	Novice::ScreenPrintf(20, 40, "LIVES %d", lives_);

	Novice::ScreenPrintf(20, 60, "BATS %d", batsKilled_);

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

	if (state_ == State::LifeLost) {
		Novice::ScreenPrintf(590, 300, "-1 LIFE (%d left)", lives_);
	}

	if (state_ == State::Descend || state_ == State::Arrive) {
		Novice::ScreenPrintf(570, 300, "GOING DOWN...");
	}

	if (state_ == State::GameOver) {
		Novice::ScreenPrintf(590, 300, "GAME OVER");

		Novice::ScreenPrintf(540, 330, "REACHED ROUND %d", circle_ + 1);

		Novice::ScreenPrintf(550, 360, "SPACE : TITLE");
	}
}

bool GameScene::IsCleared() const { return cleared_; }

bool GameScene::IsReturnToTitle() const { return returnToTitle_; }

int GameScene::GetTotalRounds() const { return totalRounds_; }
