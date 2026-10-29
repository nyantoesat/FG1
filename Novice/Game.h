
#pragma once

#include "ClearScene.h"
#include "GameScene.h"
#include "TitleScene.h"

class Game {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// シーンの種類
	enum class Scene { Title, Playing, Clear };

	// 現在のシーン
	Scene scene_ = Scene::Title;

	// 各シーン
	TitleScene titleScene_;
	GameScene gameScene_;
	ClearScene clearScene_;
};
