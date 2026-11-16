#pragma once

#include "Cage.h"
#include "Hazards.h"
#include "Map.h"
#include "Player.h"
#include "SkillManager.h"

#include <vector>

class GameScene {
public:
	void Initialize(int totalRounds, int totalLives);
	void Update();
	void Draw();

	void ResumeAfterSkill(const SkillManager& skills);

	bool IsCleared() const;
	bool IsGameOver() const;
	bool NeedsSkillSelection() const;

	int GetTotalRounds() const;
	int GetReachedRound() const;

private:
	enum class State { Play, Descend, Arrive, SkillPending, GameOver };

	void StartCircle();
	void UpdatePlay();
	void UpdateWaves();
	void SpawnHazards();
	void CheckHits();

	void PutPlayerOnCage();
	void ClearHazards();

	void SpawnRock();
	void SpawnArrow();
	void SpawnBat();

	void DrawHud() const;
	void ApplySkills(const SkillManager& skills);

	bool TakeDamage();
	void HandleDamageResult();

	Map map_;
	Cage cage_;
	Player player_;

	std::vector<Rock> rocks_;
	std::vector<Arrow> arrows_;
	std::vector<Bat> bats_;

	Spike spike_;

	State state_ = State::Play;

	int stateTimer_ = 0;
	int totalRounds_ = 5;
	int circle_ = 0;
	int wave_ = 0;
	int batsKilled_ = 0;

	int waveTimer_ = 0;
	int breakTimer_ = 0;
	int rockTimer_ = 0;
	int arrowTimer_ = 0;
	int batTimer_ = 0;

	int shieldCharges_ = 0;
	int appliedHealLevel_ = 0;

	float enemySpeedScale_ = 1.0f;

	bool cleared_ = false;
	bool damageTakenThisFrame_ = false;
};