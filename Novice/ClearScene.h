
#pragma once

class ClearScene {
public:
	void Initialize(int totalRounds);
	void Update();
	void Draw();

	bool IsFinished() const;

private:
	int totalRounds_ = 0;
	int timer_ = 0;
	bool finished_ = false;
};
