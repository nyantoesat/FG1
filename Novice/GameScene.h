
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

	// 死亡したラウンドから再開
	void RestartCurrentRound(const SkillManager& skills);

	bool IsCleared() const;
	bool IsGameOver() const;

	int GetTotalRounds() const;
	int GetReachedRound() const;

private:
	enum class State { Play, Descend, Arrive, GameOver };

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

	// スキル関連
	int baseLives_ = 3;
	int shieldCharges_ = 0;
	float enemySpeedScale_ = 1.0f;

	bool cleared_ = false;
};
