
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
			const int rounds = titleScene_.GetSelectedRounds();

			const int hp = titleScene_.GetSelectedLives();

			skillManager_.Initialize();

			// 選択したラウンド数とHPを渡す
			gameScene_.Initialize(rounds, hp);

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
		} else if (gameScene_.NeedsSkillSelection()) {
			skillSelectScene_.Initialize(skillManager_);

			scene_ = Scene::SkillSelect;
		}
		break;

	case Scene::SkillSelect:
		skillSelectScene_.Update();

		if (skillSelectScene_.IsFinished()) {
			const SkillType skill = skillSelectScene_.GetSelectedSkill();

			skillManager_.AcquireSkill(skill);

			// HP・ラウンド・位置を維持して再開
			gameScene_.ResumeAfterSkill(skillManager_);

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
