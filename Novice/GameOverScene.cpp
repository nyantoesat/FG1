#include "GameOverScene.h"
#include "Config.h"
#include "Input.h"
#include <Novice.h>

// 初期化
void GameOverScene::Initialize(int reachedRound) {
	reachedRound_ = reachedRound;
	timer_ = 0;
	finished_ = false;
}

// 更新
void GameOverScene::Update() {
	timer_++;

	// SPACEキーでタイトルに戻る
	if (timer_ > 40 && Input::JumpTrigger()) {
		finished_ = true;
	}
}

// 描画
void GameOverScene::Draw() {
	Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, 0x251C2BFF, kFillModeSolid);

	Novice::ScreenPrintf(580, 280, "GAME OVER");

	Novice::ScreenPrintf(530, 330, "REACHED ROUND %d", reachedRound_);

	Novice::ScreenPrintf(530, 390, "SPACE : TITLE");
}

bool GameOverScene::IsFinished() const { return finished_; }