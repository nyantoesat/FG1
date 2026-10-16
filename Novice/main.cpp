#include "Config.h"
#include "Game.h"
#include "Input.h"
#include <Novice.h>

const char kWindowTitle[] = "学籍番号";

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	Game game;
	game.Initialize();

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		Input::Update();

		///
		/// ↓更新処理ここから
		///
		game.Update();
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		game.Draw();
		///
		/// ↑描画処理ここまで
		///

		Novice::EndFrame();

		if (Input::Trigger(DIK_ESCAPE)) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}