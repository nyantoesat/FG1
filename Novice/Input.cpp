#include "Input.h"
#include <Novice.h>
#include <cstring>

char Input::keys_[256] = {};
char Input::preKeys_[256] = {};

void Input::Update() {
	std::memcpy(preKeys_, keys_, 256);
	Novice::GetHitKeyStateAll(keys_);
}

bool Input::Press(int key) { return keys_[key] != 0; }
bool Input::Trigger(int key) { return keys_[key] != 0 && preKeys_[key] == 0; }

bool Input::Left() { return Press(DIK_A) || Press(DIK_LEFT); }
bool Input::Right() { return Press(DIK_D) || Press(DIK_RIGHT); }
bool Input::Up() { return Press(DIK_W) || Press(DIK_UP); }
bool Input::Down() { return Press(DIK_S) || Press(DIK_DOWN); }
bool Input::JumpPress() { return Press(DIK_SPACE); }
bool Input::JumpTrigger() { return Trigger(DIK_SPACE); }