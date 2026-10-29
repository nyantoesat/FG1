#pragma once

class GameOverScene {
public:
	void Initialize(int reachedRound);
	void Update();
	void Draw();

	bool IsFinished() const;

private:
	int reachedRound_ = 1;
	int timer_ = 0;
	bool finished_ = false;
};