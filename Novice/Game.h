
#pragma once

#include "ClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "SkillManager.h"
#include "SkillSelectScene.h"
#include "TitleScene.h"

class Game {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	enum class Scene { Title, Playing, SkillSelect, Clear, GameOver };

	Scene scene_ = Scene::Title;

	TitleScene titleScene_;
	GameScene gameScene_;
	SkillSelectScene skillSelectScene_;
	ClearScene clearScene_;
	GameOverScene gameOverScene_;

	SkillManager skillManager_;
};
