#pragma once

#include "ClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "TitleScene.h"

class Game {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	enum class Scene { Title, Playing, Clear, GameOver };

	Scene scene_ = Scene::Title;

	TitleScene titleScene_;
	GameScene gameScene_;
	ClearScene clearScene_;
	GameOverScene gameOverScene_;
};