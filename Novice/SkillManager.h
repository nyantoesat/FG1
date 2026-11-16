#pragma once

#include <array>

enum class SkillType { LifeUp, SpeedUp, JumpUp, Shield, SlowDown, FloorUp, Heal, Count };

class SkillManager {
public:
	void Initialize();

	std::array<SkillType, 3> GenerateChoices() const;

	void AcquireSkill(SkillType skill);

	int GetSkillLevel(SkillType skill) const;

	static const char* GetSkillName(SkillType skill);
	static const char* GetSkillDescription(SkillType skill);

private:
	static constexpr int kSkillCount = static_cast<int>(SkillType::Count);

	std::array<int, kSkillCount> skillLevels_{};
};