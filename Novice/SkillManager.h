#pragma once

#include <array>

// スキルの種類
enum class SkillType {
	LifeUp,
	SpeedUp,
	JumpUp,
	Shield,
	SlowDown,
	FloorUp,

	Count
};

// スキルの管理
class SkillManager {
public:
	// 初期化
	void Initialize();

	// ランダムなスキルを3つ抽選
	std::array<SkillType, 3> GenerateChoices() const;

	// スキルを獲得
	void AcquireSkill(SkillType skill);

	// スキルの獲得回数
	int GetSkillLevel(SkillType skill) const;

	// スキルの名前
	static const char* GetSkillName(SkillType skill);

	// スキルの説明
	static const char* GetSkillDescription(SkillType skill);

private:
	static constexpr int kSkillCount = static_cast<int>(SkillType::Count);

	// 各スキルの獲得回数
	std::array<int, kSkillCount> skillLevels_{};
};