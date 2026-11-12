
#pragma once

#include "SkillManager.h"
#include <array>

class SkillSelectScene {
public:
	// 初期化
	void Initialize(const SkillManager& skillManager);

	// 更新・描画
	void Update();
	void Draw() const;

	// 選択完了したか
	bool IsFinished() const;

	// 選択したスキルを取得
	SkillType GetSelectedSkill() const;

private:
	// 表示する3つのスキル
	std::array<SkillType, 3> choices_{};

	// 現在選択している位置（0～2）
	int selectedIndex_ = 0;

	// 決定済みか
	bool finished_ = false;

	// 前フレームのキー入力状態
	bool prevLeft_ = false;
	bool prevRight_ = false;
};
