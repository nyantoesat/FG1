
#include "SkillSelectScene.h"
#include "Config.h"
#include "Input.h"
#include <Novice.h>

// ===================== 初期化 =====================

void SkillSelectScene::Initialize(const SkillManager& skillManager) {
	// ランダムなスキルを3つ抽選
	choices_ = skillManager.GenerateChoices();

	// 最初のスキルを選択状態にする
	selectedIndex_ = 0;

	// 決定状態をリセット
	finished_ = false;

	// キー入力状態をリセット
	prevLeft_ = Input::Left();
	prevRight_ = Input::Right();
}

// ===================== 更新 =====================

void SkillSelectScene::Update() {
	if (finished_) {
		return;
	}

	// 現在のキー入力
	bool currentLeft = Input::Left();
	bool currentRight = Input::Right();

	// 左キーを押した瞬間だけ反応
	if (currentLeft && !prevLeft_) {
		selectedIndex_--;

		if (selectedIndex_ < 0) {
			selectedIndex_ = 2;
		}
	}

	// 右キーを押した瞬間だけ反応
	if (currentRight && !prevRight_) {
		selectedIndex_++;

		if (selectedIndex_ > 2) {
			selectedIndex_ = 0;
		}
	}

	// 前フレームの入力を保存
	prevLeft_ = currentLeft;
	prevRight_ = currentRight;

	// SPACEキーで決定
	if (Input::JumpTrigger()) {
		finished_ = true;
	}
}

// ===================== 描画 =====================

void SkillSelectScene::Draw() const {
	// 背景
	Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, 0x1C2033FF, kFillModeSolid);

	// タイトル
	Novice::ScreenPrintf(kWindowWidth / 2 - 40, 130, "YOU DIED");

	Novice::ScreenPrintf(kWindowWidth / 2 - 65, 170, "CHOOSE ONE SKILL");

	// スキルカードのサイズ
	const int cardWidth = 260;
	const int cardHeight = 180;
	const int gap = 25;

	const int totalWidth = cardWidth * 3 + gap * 2;

	const int startX = (kWindowWidth - totalWidth) / 2;

	const int cardY = 250;

	// 3つのスキルカードを表示
	for (int i = 0; i < 3; i++) {
		int x = startX + i * (cardWidth + gap);

		// 選択中のカードを緑色にする
		unsigned int color = i == selectedIndex_ ? 0x8FDCC8FF : 0x303950FF;

		// カード背景
		Novice::DrawBox(x, cardY, cardWidth, cardHeight, 0.0f, color, kFillModeSolid);

		// スキル番号
		Novice::ScreenPrintf(x + 25, cardY + 20, "SKILL %d", i + 1);

		// スキル名
		Novice::ScreenPrintf(x + 25, cardY + 55, "%s", SkillManager::GetSkillName(choices_[i]));

		// スキルの効果
		Novice::ScreenPrintf(x + 25, cardY + 95, "%s", SkillManager::GetSkillDescription(choices_[i]));

		// 選択中の表示
		if (i == selectedIndex_) {
			Novice::ScreenPrintf(x + 25, cardY + 145, "SELECTED");
		}
	}

	// 操作説明
	Novice::ScreenPrintf(kWindowWidth / 2 - 95, 500, "LEFT / RIGHT : SELECT");

	Novice::ScreenPrintf(kWindowWidth / 2 - 75, 530, "SPACE : CONFIRM");
}

// ===================== 選択完了 =====================

bool SkillSelectScene::IsFinished() const { return finished_; }

// ===================== 選択スキル取得 =====================

SkillType SkillSelectScene::GetSelectedSkill() const { return choices_[selectedIndex_]; }
