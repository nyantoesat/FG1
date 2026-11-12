
#include "Game.h"

void Game::Initialize() {
	titleScene_.Initialize();
	skillManager_.Initialize();
	scene_ = Scene::Title;
}

void Game::Update() {
	switch (scene_) {

	case Scene::Title:
		titleScene_.Update();

		if (titleScene_.IsFinished()) {
			int rounds = titleScene_.GetSelectedRounds();
			int lives = titleScene_.GetSelectedLives();

			skillManager_.Initialize();
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
			skillSelectScene_.Initialize(skillManager_);
			scene_ = Scene::SkillSelect;
		}
		break;

	case Scene::SkillSelect:
		skillSelectScene_.Update();

		if (skillSelectScene_.IsFinished()) {
			SkillType selectedSkill = skillSelectScene_.GetSelectedSkill();

			skillManager_.AcquireSkill(selectedSkill);

			// 死亡したラウンドから再開
			gameScene_.RestartCurrentRound(skillManager_);

			scene_ = Scene::Playing;
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

	case Scene::SkillSelect:
		skillSelectScene_.Draw();
		break;

	case Scene::Clear:
		clearScene_.Draw();
		break;

	case Scene::GameOver:
		gameOverScene_.Draw();
		break;
	}
}
