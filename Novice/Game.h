#pragma once
#include "Cage.h"
#include "Hazards.h"
#include "Map.h"
#include "Player.h"
#include <vector>

// The whole game: title -> 9 circles of waves -> game over / win.
class Game {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	enum class State {
		Title,    // press SPACE to start
		Play,     // survive the waves
		LifeLost, // hearts reached 0 -> short pause, then retry the circle
		Descend,  // circle cleared: the cage goes down
		Arrive,   // the cage comes down into the next circle
		GameOver, // no lives left
		Win,      // cleared all 9 circles
	};

	// flow
	void StartRun();
	void StartCircle(); // (re)start the current circle from wave 1
	void UpdatePlay();
	void UpdateWaves();
	void SpawnHazards();
	void CheckHits();
	void PutPlayerOnCage();
	void ClearHazards();

	// spawning
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

	State state_ = State::Title;
	int stateTimer_ = 0;

	int circle_ = 0; // 0..8
	int wave_ = 0;   // 0..kWavesPerCircle-1
	int lives_ = kStartLives;
	int waveTimer_ = 0;
	int breakTimer_ = 0; // > 0 = pause between waves (nothing spawns)
	int batsKilled_ = 0;

	// spawn timers (count down to the next spawn)
	int rockTimer_ = 0;
	int arrowTimer_ = 0;
	int batTimer_ = 0;
};