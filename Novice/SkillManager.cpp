#include "SkillManager.h"
#include <algorithm>
#include <random>

// 初期化
void SkillManager::Initialize() { skillLevels_.fill(0); }

// スキルを3つ抽選
std::array<SkillType, 3> SkillManager::GenerateChoices() const {
	std::array<SkillType, 6> skills = {SkillType::LifeUp, SkillType::SpeedUp, SkillType::JumpUp, SkillType::Shield, SkillType::SlowDown, SkillType::FloorUp};

	static std::mt19937 engine(std::random_device{}());

	std::shuffle(skills.begin(), skills.end(), engine);

	return {skills[0], skills[1], skills[2]};
}

// スキルを獲得
void SkillManager::AcquireSkill(SkillType skill) {
	int index = static_cast<int>(skill);

	if (index < 0 || index >= kSkillCount) {
		return;
	}

	skillLevels_[index]++;
}

// スキルの獲得回数
int SkillManager::GetSkillLevel(SkillType skill) const {
	int index = static_cast<int>(skill);

	if (index < 0 || index >= kSkillCount) {
		return 0;
	}

	return skillLevels_[index];
}

// スキル名
const char* SkillManager::GetSkillName(SkillType skill) {
	switch (skill) {
	case SkillType::LifeUp:
		return "LIFE UP";
	case SkillType::SpeedUp:
		return "SPEED UP";
	case SkillType::JumpUp:
		return "JUMP UP";
	case SkillType::Shield:
		return "SHIELD";
	case SkillType::SlowDown:
		return "SLOW DOWN";
	case SkillType::FloorUp:
		return "FLOOR UP";
	default:
		return "UNKNOWN";
	}
}

// スキル説明
const char* SkillManager::GetSkillDescription(SkillType skill) {
	switch (skill) {
	case SkillType::LifeUp:
		return "MAX LIFE +1";
	case SkillType::SpeedUp:
		return "MOVE SPEED +15%";
	case SkillType::JumpUp:
		return "JUMP POWER +15%";
	case SkillType::Shield:
		return "BLOCK ONE HIT";
	case SkillType::SlowDown:
		return "ENEMY SPEED DOWN";
	case SkillType::FloorUp:
		return "FLOOR WIDTH +20%";
	default:
		return "";
	}
}