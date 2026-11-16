#include "SkillManager.h"

#include <algorithm>
#include <random>
#include <vector>

void SkillManager::Initialize() { skillLevels_.fill(0); }

std::array<SkillType, 3> SkillManager::GenerateChoices() const {

	// 最大HP増加のLifeUpは除外
	// HP回復のHealを追加
	std::vector<SkillType> available = {SkillType::SpeedUp, SkillType::JumpUp, SkillType::Shield, SkillType::SlowDown, SkillType::FloorUp, SkillType::Heal};

	static std::mt19937 engine(std::random_device{}());

	std::shuffle(available.begin(), available.end(), engine);

	return {available[0], available[1], available[2]};
}

void SkillManager::AcquireSkill(SkillType skill) {
	const int index = static_cast<int>(skill);

	if (index >= 0 && index < kSkillCount) {
		skillLevels_[index]++;
	}
}

int SkillManager::GetSkillLevel(SkillType skill) const {
	const int index = static_cast<int>(skill);

	if (index < 0 || index >= kSkillCount) {
		return 0;
	}

	return skillLevels_[index];
}

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
	case SkillType::Heal:
		return "HEAL";
	default:
		return "UNKNOWN";
	}
}

const char* SkillManager::GetSkillDescription(SkillType skill) {
	switch (skill) {
	case SkillType::LifeUp:
		return "NOT AVAILABLE";
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
	case SkillType::Heal:
		return "RESTORE 1 HP";
	default:
		return "";
	}
}