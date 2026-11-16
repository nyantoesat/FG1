
#pragma once

class TitleScene {
public:
	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const;

	int GetSelectedRounds() const;
	int GetSelectedLives() const;
	int GetSelectedStocks() const;

private:
	int selectedRounds_ = 5;
	int selectedLives_ = 3;
	int selectedItem_ = 0;

	int timer_ = 0;
	bool finished_ = false;

	static constexpr int kMinRounds = 1;
	static constexpr int kMaxRounds = 10;

	static constexpr int kMinLives = 1;
	static constexpr int kMaxLives = 10;
};
