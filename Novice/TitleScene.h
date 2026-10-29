
#pragma once

class TitleScene {
public:
	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const;
	int GetSelectedRounds() const;

private:
	int selectedRounds_ = 5;
	int timer_ = 0;
	bool finished_ = false;

	static constexpr int kMinRounds = 1;
	static constexpr int kMaxRounds = 10;
};
