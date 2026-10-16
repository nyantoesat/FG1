#pragma once

class Input {
public:
	static void Update();
	static bool Press(int key);   // held down
	static bool Trigger(int key); // pressed this frame only

	// Game actions
	static bool Left();
	static bool Right();
	static bool Up();   // climb up
	static bool Down(); // climb down
	static bool JumpPress();
	static bool JumpTrigger();

private:
	static char keys_[256];
	static char preKeys_[256];
};