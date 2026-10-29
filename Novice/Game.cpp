#include "Game.h"

void Game::Initialize() {
	titleScene_.Initialize();
	scene_ = Scene::Title;
}

void Game::Update() {
	switch (scene_) {

	case Scene::Title:
		titleScene_.Update();

		if (titleScene_.IsFinished()) {
			int rounds = titleScene_.GetSelectedRounds();
			int lives = titleScene_.GetSelectedLives();

			gameScene_.Initialize(rounds, lives);
			scene_ = Scene::Playing;
		}
		break;

	case Scene::Playing:
		gameScene_.Update();

		if (gameScene_.IsCleared()) {
			clearScene_.Initialize(gameScene_.GetTotalRounds());
			scene_ = Scene::Clear;
		} else if (gameScene_.IsGameOver()) {
			gameOverScene_.Initialize(gameScene_.GetReachedRound());
			scene_ = Scene::GameOver;
		}
		break;

	case Scene::Clear:
		clearScene_.Update();

		if (clearScene_.IsFinished()) {
			titleScene_.Initialize();
			scene_ = Scene::Title;
		}
		break;

	case Scene::GameOver:
		gameOverScene_.Update();

		if (gameOverScene_.IsFinished()) {
			titleScene_.Initialize();
			scene_ = Scene::Title;
		}
		break;
	}
}

void Game::Draw() {
	switch (scene_) {

	case Scene::Title:
		titleScene_.Draw();
		break;

	case Scene::Playing:
		gameScene_.Draw();
		break;

	case Scene::Clear:
		clearScene_.Draw();
		break;

	case Scene::GameOver:
		gameOverScene_.Draw();
		break;
	}
}