
#pragma once

#include "Cage.h"
#include "Hazards.h"
#include "Map.h"
#include "Player.h"
#include <vector>

class GameScene {
public:
	void Initialize(int totalRounds);
	void Update();
	void Draw();

	bool IsCleared() const;
	bool IsReturnToTitle() const;
	int GetTotalRounds() const;

private:
	enum class State { Play, LifeLost, Descend, Arrive, GameOver };

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

	Map map_;
	Cage cage_;
	Player player_;

	std::vector<Rock> rocks_;
	std::vector<Arrow> arrows_;
	std::vector<Bat> bats_;

	State state_ = State::Play;

	int stateTimer_ = 0;

	int totalRounds_ = 5;
	int circle_ = 0;
	int wave_ = 0;

	int lives_ = kStartLives;
	int batsKilled_ = 0;

	int waveTimer_ = 0;
	int breakTimer_ = 0;

	int rockTimer_ = 0;
	int arrowTimer_ = 0;
	int batTimer_ = 0;

	bool cleared_ = false;
	bool returnToTitle_ = false;
};
