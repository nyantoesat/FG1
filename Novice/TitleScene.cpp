
#include "TitleScene.h"
#include "Input.h"
#include <Novice.h>
#include <dinput.h>

void TitleScene::Initialize() {
	selectedRounds_ = 5;
	timer_ = 0;
	finished_ = false;
}

void TitleScene::Update() {
	timer_++;

	// 左右キーでラウンド数を変更
	if (timer_ % 8 == 0) {
		if (Novice::CheckHitKey(DIK_LEFT) && selectedRounds_ > kMinRounds) {
			selectedRounds_--;
		}

		if (Novice::CheckHitKey(DIK_RIGHT) && selectedRounds_ < kMaxRounds) {
			selectedRounds_++;
		}
	}

	// SPACEでゲーム開始
	if (Input::JumpTrigger()) {
		finished_ = true;
	}
}

void TitleScene::Draw() {
	Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x24203FFF, kFillModeSolid);

	Novice::ScreenPrintf(560, 250, "DEATH LIFE");

	Novice::ScreenPrintf(530, 310, "SELECT ROUNDS");

	Novice::ScreenPrintf(550, 350, "<  %d  >", selectedRounds_);

	Novice::ScreenPrintf(470, 390, "LEFT / RIGHT : CHANGE ROUNDS");

	Novice::ScreenPrintf(520, 430, "SPACE : START");

	Novice::ScreenPrintf(470, 470, "A/D : MOVE    SPACE : JUMP");
}

bool TitleScene::IsFinished() const { return finished_; }

int TitleScene::GetSelectedRounds() const { return selectedRounds_; }
