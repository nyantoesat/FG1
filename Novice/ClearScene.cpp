
#include "ClearScene.h"
#include "Input.h"
#include <Novice.h>

void ClearScene::Initialize(int totalRounds) {
	totalRounds_ = totalRounds;
	timer_ = 0;
	finished_ = false;
}

void ClearScene::Update() {
	timer_++;

	if (timer_ > 40 && Input::JumpTrigger()) {
		finished_ = true;
	}
}

void ClearScene::Draw() {
	Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x202D38FF, kFillModeSolid);

	Novice::ScreenPrintf(580, 280, "GAME CLEAR!");

	Novice::ScreenPrintf(530, 330, "CLEARED %d ROUNDS!", totalRounds_);

	Novice::ScreenPrintf(530, 390, "SPACE : TITLE");
}

bool ClearScene::IsFinished() const { return finished_; }
