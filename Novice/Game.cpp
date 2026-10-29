
#include "Game.h"

// 初期化
void Game::Initialize() {
	titleScene_.Initialize();

	scene_ = Scene::Title;
}

// 更新
void Game::Update() {
	switch (scene_) {

	// タイトルシーン
	case Scene::Title:
		titleScene_.Update();

		// SPACEでゲーム開始
		if (titleScene_.IsFinished()) {

			// 選択したラウンド数を取得
			int rounds = titleScene_.GetSelectedRounds();

			// ゲームシーンを初期化
			gameScene_.Initialize(rounds);

			// ゲームシーンへ切り替え
			scene_ = Scene::Playing;
		}
		break;

	// ゲームシーン
	case Scene::Playing:
		gameScene_.Update();

		// 全ラウンドクリア
		if (gameScene_.IsCleared()) {

			// クリアシーンを初期化
			clearScene_.Initialize(gameScene_.GetTotalRounds());

			// クリアシーンへ切り替え
			scene_ = Scene::Clear;
		}

		// ゲームオーバー後にタイトルへ戻る
		else if (gameScene_.IsReturnToTitle()) {

			titleScene_.Initialize();

			scene_ = Scene::Title;
		}
		break;

	// ゲームクリアシーン
	case Scene::Clear:
		clearScene_.Update();

		// SPACEでタイトルに戻る
		if (clearScene_.IsFinished()) {

			titleScene_.Initialize();

			scene_ = Scene::Title;
		}
		break;
	}
}

// 描画
void Game::Draw() {
	switch (scene_) {

	// タイトル画面
	case Scene::Title:
		titleScene_.Draw();
		break;

	// ゲーム画面
	case Scene::Playing:
		gameScene_.Draw();
		break;

	// ゲームクリア画面
	case Scene::Clear:
		clearScene_.Draw();
		break;
	}
}
