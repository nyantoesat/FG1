
#include "TitleScene.h"
#include "Input.h"
#include <Novice.h>
#include <dinput.h>

void TitleScene::Initialize() {
	selectedRounds_ = 5;
	selectedLives_ = 3;
	selectedItem_ = 0;

	timer_ = 0;
	finished_ = false;
}

void TitleScene::Update() {
	timer_++;

	// 上下キーで項目を選択
	if (timer_ % 10 == 0) {
		if (Novice::CheckHitKey(DIK_UP)) {
			selectedItem_ = 0;
		}

		if (Novice::CheckHitKey(DIK_DOWN)) {
			selectedItem_ = 1;
		}
	}

	// 左右キーで数値変更
	if (timer_ % 8 == 0) {
		if (Novice::CheckHitKey(DIK_LEFT)) {
			if (selectedItem_ == 0 && selectedRounds_ > kMinRounds) {
				selectedRounds_--;
			}

			if (selectedItem_ == 1 && selectedLives_ > kMinLives) {
				selectedLives_--;
			}
		}

		if (Novice::CheckHitKey(DIK_RIGHT)) {
			if (selectedItem_ == 0 && selectedRounds_ < kMaxRounds) {
				selectedRounds_++;
			}

			if (selectedItem_ == 1 && selectedLives_ < kMaxLives) {
				selectedLives_++;
			}
		}
	}

	// SPACEでゲーム開始
	if (Input::JumpTrigger()) {
		finished_ = true;
	}
}

void TitleScene::Draw() {
	Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x24203FFF, kFillModeSolid);

	Novice::ScreenPrintf(560, 220, "DEATH LIFE");

	Novice::ScreenPrintf(510, 300, "%s ROUNDS : < %d >", selectedItem_ == 0 ? ">" : " ", selectedRounds_);

	Novice::ScreenPrintf(510, 340, "%s LIVES  : < %d >", selectedItem_ == 1 ? ">" : " ", selectedLives_);

	Novice::ScreenPrintf(480, 410, "UP / DOWN : SELECT");
	Novice::ScreenPrintf(480, 440, "LEFT / RIGHT : CHANGE");
	Novice::ScreenPrintf(520, 490, "SPACE : START");
}

bool TitleScene::IsFinished() const { return finished_; }

int TitleScene::GetSelectedRounds() const { return selectedRounds_; }

int TitleScene::GetSelectedLives() const { return selectedLives_; }
