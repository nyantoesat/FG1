#include "Game.h"
#include "Input.h"
#include <Novice.h>
#include <algorithm>

namespace {

// ---- Difficulty: smaller interval = more spawns. Gets harder each circle and each wave. ----
int RockInterval(int circle, int wave) { return (std::max)(25, 90 - circle * 7 - wave * 10); }
int ArrowInterval(int circle, int wave) { return (std::max)(45, 170 - circle * 12 - wave * 15); }
int BatInterval(int circle, int wave) { return (std::max)(150, 480 - circle * 35 - wave * 40); }
int MaxBats(int circle, int wave) { return 1 + circle / 2 + wave; }
float BatSpeed(int circle) { return 1.8f + circle * 0.15f; }
bool ArrowsEnabled(int circle) { return circle >= 1; } // circle 1 = rocks and bats only

template<typename T> void RemoveDead(std::vector<T>& list) {
	list.erase(std::remove_if(list.begin(), list.end(), [](const T& t) { return !t.alive; }), list.end());
}

} // namespace

void Game::Initialize() {
	map_.Initialize();
	state_ = State::Title;
	PutPlayerOnCage();
}

// ============================================================ flow

void Game::StartRun() {
	circle_ = 0;
	lives_ = kStartLives;
	batsKilled_ = 0;
	player_.RefillHearts();
	StartCircle();
}

void Game::StartCircle() {
	wave_ = 0;
	waveTimer_ = kWaveFrames;
	breakTimer_ = kWaveBreakFrames; // short pause before wave 1 ("CIRCLE N")
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

void Game::PutPlayerOnCage() { player_.Spawn(kCageX + kCageWidth * 0.5f - kPlayerWidth * 0.5f, cage_.y - kPlayerHeight); }

void Game::ClearHazards() {
	rocks_.clear();
	arrows_.clear();
	bats_.clear();
}

void Game::Update() {
	stateTimer_++;

	switch (state_) {
	case State::Title:
		if (Input::JumpTrigger()) {
			StartRun();
		}
		break;

	case State::Play:
		UpdatePlay();
		break;

	case State::LifeLost:
		if (stateTimer_ > 90) {
			if (lives_ > 0) {
				player_.RefillHearts();
				StartCircle(); // same circle, from wave 1
			} else {
				state_ = State::GameOver;
				stateTimer_ = 0;
			}
		}
		break;

	case State::Descend:
		// Cage goes down with the player on it, out of the bottom of the screen
		cage_.vy = 5.0f;
		cage_.y += cage_.vy;
		PutPlayerOnCage();
		if (cage_.y > kWindowHeight + 40.0f) {
			circle_++;
			if (circle_ >= kCircleCount) {
				state_ = State::Win;
				stateTimer_ = 0;
			} else {
				cage_.y = -kCageHeight; // comes in from the top of the next circle
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
	case State::Win:
		if (stateTimer_ > 40 && Input::JumpTrigger()) {
			state_ = State::Title;
			cage_.y = kCageRestY;
			PutPlayerOnCage();
		}
		break;
	}
}

void Game::UpdatePlay() {
	cage_.vy = 0.0f;
	player_.Update(map_, cage_, true);

	UpdateWaves();

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

	// Fell into the void: lose a heart and go back on the cage
	if (player_.FellOut()) {
		player_.LoseHeart(); // always costs a heart, even while invincible
		PutPlayerOnCage();
	}

	if (player_.GetHearts() <= 0) {
		lives_--;
		state_ = State::LifeLost;
		stateTimer_ = 0;
	}
}

void Game::UpdateWaves() {
	if (breakTimer_ > 0) {
		breakTimer_--;
		return; // pause between waves: nothing new spawns
	}

	SpawnHazards();

	waveTimer_--;
	if (waveTimer_ <= 0) {
		wave_++;
		if (wave_ >= kWavesPerCircle) {
			// Circle cleared -> ride the cage down
			ClearHazards();
			state_ = State::Descend;
			stateTimer_ = 0;
		} else {
			waveTimer_ = kWaveFrames;
			breakTimer_ = kWaveBreakFrames;
		}
	}
}

// ============================================================ spawning

void Game::SpawnHazards() {
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

void Game::SpawnRock() {
	Rock r;
	// Half of the rocks aim near the player, so standing still is dangerous
	if (RandomInt(0, 1) == 0) {
		r.x = player_.GetCenter().x - Rock::kSize * 0.5f + RandomFloat(-60.0f, 60.0f);
	} else {
		r.x = RandomFloat(kShaftLeft, kShaftRight - Rock::kSize);
	}
	r.x = Clamp(r.x, kShaftLeft, kShaftRight - Rock::kSize);
	rocks_.push_back(r);
}

void Game::SpawnArrow() {
	Arrow a;
	a.dir = RandomInt(0, 1) == 0 ? 1 : -1;
	a.x = a.dir > 0 ? kShaftLeft - Arrow::kLength : kShaftRight;
	// Aim around the player's height
	a.y = Clamp(player_.GetCenter().y + RandomFloat(-80.0f, 80.0f), 40.0f, kCageRestY - 10.0f);
	arrows_.push_back(a);
}

void Game::SpawnBat() {
	Bat b;
	b.pos = {RandomFloat(kShaftLeft, kShaftRight - Bat::kWidth), -Bat::kHeight};
	b.speed = BatSpeed(circle_);
	bats_.push_back(b);
}

// ============================================================ hits

void Game::CheckHits() {
	Rect p = player_.GetRect();

	for (Rock& r : rocks_) {
		if (r.IsDangerous() && Intersects(p, r.GetRect())) {
			player_.Damage();
		}
	}
	for (Arrow& a : arrows_) {
		if (a.IsDangerous() && Intersects(p, a.GetRect())) {
			if (player_.Damage()) {
				a.alive = false; // arrow sticks in the player
			}
		}
	}
	for (Bat& b : bats_) {
		Rect br = b.GetRect();
		if (!Intersects(p, br)) {
			continue;
		}
		// Stomp: falling and our feet were above the bat last frame
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

// ============================================================ draw

void Game::Draw() {
	// PLACEHOLDER ART: background
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

void Game::DrawHud() const {
	// Hearts (top center)
	for (int i = 0; i < kMaxHearts; i++) {
		unsigned int color = i < player_.GetHearts() ? 0x8FDCC8FF : 0x3E4A5AFF;
		Novice::DrawBox(640 - kMaxHearts * 20 + i * 40 + 6, 20, 28, 24, 0.0f, color, kFillModeSolid);
	}

	// Text (debug font for now - replace with your own UI later)
	Novice::ScreenPrintf(20, 20, "CIRCLE %d / %d", circle_ + 1, kCircleCount);
	Novice::ScreenPrintf(20, 40, "LIVES  %d", lives_);
	Novice::ScreenPrintf(20, 60, "BATS   %d", batsKilled_);
	Novice::ScreenPrintf(1000, 20, "WAVE %d / %d", wave_ + 1, kWavesPerCircle);

	// Wave timer bar
	if (state_ == State::Play) {
		float t = breakTimer_ > 0 ? 1.0f : (float)waveTimer_ / kWaveFrames;
		Novice::DrawBox(1000, 44, 200, 8, 0.0f, 0x3E4A5AFF, kFillModeSolid);
		Novice::DrawBox(1000, 44, (int)(200 * t), 8, 0.0f, 0x8FDCC8FF, kFillModeSolid);
	}

	// Center messages
	switch (state_) {
	case State::Title:
		Novice::ScreenPrintf(560, 300, "MITTENS-LIKE (title)");
		Novice::ScreenPrintf(560, 330, "PRESS SPACE TO START");
		Novice::ScreenPrintf(470, 380, "A/D: move   SPACE: jump");
		Novice::ScreenPrintf(470, 400, "Touch a wall + hold toward it / W / S: hang & climb");
		Novice::ScreenPrintf(470, 420, "Jump while hanging: wall jump   Land on bats to kill");
		break;
	case State::Play:
		if (breakTimer_ > 0) {
			Novice::ScreenPrintf(590, 300, "CIRCLE %d", circle_ + 1);
			Novice::ScreenPrintf(595, 320, "WAVE %d", wave_ + 1);
		}
		break;
	case State::LifeLost:
		Novice::ScreenPrintf(590, 300, "-1 LIFE  (%d left)", lives_);
		break;
	case State::Descend:
	case State::Arrive:
		Novice::ScreenPrintf(570, 300, "GOING DOWN...");
		break;
	case State::GameOver:
		Novice::ScreenPrintf(590, 300, "GAME OVER");
		Novice::ScreenPrintf(540, 330, "reached circle %d   SPACE: title", circle_ + 1);
		break;
	case State::Win:
		Novice::ScreenPrintf(590, 300, "YOU ESCAPED!");
		Novice::ScreenPrintf(560, 330, "SPACE: title");
		break;
	}
}