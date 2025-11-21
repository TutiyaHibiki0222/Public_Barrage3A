#include "Project.h"
#include "Vector.h"
#include "InputSystem.h"
#include "Debug.hpp"

// 初期化.
InputSystem::InputSystem() : mousePos(){
	keyStateCurrent.fill(0);
	keyStatePrevious.fill(0);
	mouseInputCurrent = mouseInputPrevious = 0;
	mouseInputCount.fill(0);
}

// 入力の状態を更新する.
void InputSystem::Update() {
	// キーボードの状態を保存.
	keyStatePrevious = keyStateCurrent;
	keyStateCurrent.fill(false);
	// 全てのキーを取得.
	GetHitKeyStateAllEx(keyStateCurrent.data());

	// マウスの状態を保存.
	mouseInputPrevious = mouseInputCurrent;
	mouseInputCurrent = GetMouseInput();
	GetMousePoint(&mousePos.x, &mousePos.y);

	// マウスの押されている時間を更新.
	UpdateMouseCounter();

    scrollAmount = GetMouseWheelRotVol();

	int state = 0;
	// パッド.
    for (int padIndex = 0; padIndex < LibrarySettings::MaxPadCount; ++padIndex) {
        auto& pad = pads[padIndex];

        // 前フレームの保存
        pad.previous = pad.current;

        XINPUT_STATE xInputState{};
        if (GetJoypadXInputState(DX_INPUT_PAD1 + padIndex, &xInputState) == 0) {
            if (!pad.isConnected) {
                pad.isConnected = true;
#if _DEBUG
                GameEngine::Debug::Log("PAD {:d} 接続されました。", padIndex);
#endif
            }

            // ボタン状態コピー
            for (int i = 0; i < 16; ++i) {
                pad.current[i] = xInputState.Buttons[i];
            }

            // トリガーとスティック
            pad.leftTrigger = xInputState.LeftTrigger;
            pad.rightTrigger = xInputState.RightTrigger;
            pad.thumbLX = xInputState.ThumbLX;
            pad.thumbLY = xInputState.ThumbLY;
            pad.thumbRX = xInputState.ThumbRX;
            pad.thumbRY = xInputState.ThumbRY;
        }
		else if ((state = GetJoypadInputState(DX_INPUT_PAD1 + padIndex)) != -1) {
			if (!pad.isConnected) {
				pad.isConnected = true;
#if _DEBUG
				GameEngine::Debug::Log("PAD {:d} 接続されました。(DirectInput)", padIndex);
#endif
			}

			for (int i = 0; i < 16; ++i) {
				pad.current[i] = (state & (1 << i)) ? 1 : 0;
			}

			int lx = 0, ly = 0;
			GetJoypadAnalogInput(&lx, &ly, DX_INPUT_PAD1 + padIndex);

			pad.thumbLX = lx;
			pad.thumbLY = ly;
			pad.thumbRX = 0;
			pad.thumbRY = 0;
			pad.leftTrigger = 0;
			pad.rightTrigger = 0;
		}
		else {
            if (pad.isConnected) {
                pad.isConnected = false;
#if _DEBUG
                GameEngine::Debug::Log("PAD {:d} 接続が切れました.", padIndex);
#endif
            }
            pad.current.fill(0);
            pad.leftTrigger = pad.rightTrigger = 0;
            pad.thumbLX = pad.thumbLY = pad.thumbRX = pad.thumbRY = 0;
        }
    }
}
// キーが押されているかをチェック.
bool InputSystem::IsKey(int key) {
	return keyStateCurrent[key] != 0;
}

// キーが押された瞬間をチェック.
bool InputSystem::IsKeyDown(int key) {
	return keyStateCurrent[key] != 0 && keyStatePrevious[key] == 0;
}

// キーが離された瞬間をチェック.
bool InputSystem::IsKeyUp(int key) {
	return keyStateCurrent[key] == 0 && keyStatePrevious[key] != 0;
}

// マウスボタンが押されているかをチェック.
bool InputSystem::IsMouseButton(int button) {
	return (mouseInputCurrent & button) != 0;
}

// マウスボタンが押された瞬間をチェック.
bool InputSystem::IsMouseButtonDown(int button) {
	return (mouseInputCurrent & button) != 0 && (mouseInputPrevious & button) == 0;
}

// マウスボタンが離された瞬間をチェック.
bool InputSystem::IsMouseButtonUp(int button) {
	return (mouseInputCurrent & button) == 0 && (mouseInputPrevious & button) != 0;
}

// マウスボタンがクリックされたかをチェック.
bool InputSystem::IsMouseButtonClicked(int button, int clickDuration = 10) {
	int index = GetMouseButtonIndex(button);
	return mouseInputCount[index] == clickDuration;
}

// マウスボタンが長押しされているかをチェック
bool InputSystem::IsMouseButtonLongPressed(int button, int longPressDuration = 30) {
	int index = GetMouseButtonIndex(button);
	return mouseInputCount[index] >= longPressDuration;
}



// マウスボタンが押されているかをチェック.
bool InputSystem::IsMouseButton(MouseCode button) {
	return (mouseInputCurrent & (int)button) != 0;
}

// マウスボタンが押された瞬間をチェック.
bool InputSystem::IsMouseButtonDown(MouseCode button) {
	return (mouseInputCurrent & (int)button) != 0 && (mouseInputPrevious & (int)button) == 0;
}

// マウスボタンが離された瞬間をチェック.
bool InputSystem::IsMouseButtonUp(MouseCode button) {
	return (mouseInputCurrent & (int)button) == 0 && (mouseInputPrevious & (int)button) != 0;
}

// キーが押されているかをチェック.
bool InputSystem::IsKey(KeyCode key) {
	return keyStateCurrent[(int)key] != 0;
}

// キーが押された瞬間をチェック.
bool InputSystem::IsKeyDown(KeyCode key) {
	return keyStateCurrent[(int)key] != 0 && keyStatePrevious[(int)key] == 0;
}

// キーが離された瞬間をチェック.
bool InputSystem::IsKeyUp(KeyCode key) {
	return keyStateCurrent[(int)key] == 0 && keyStatePrevious[(int)key] != 0;
}

// マウスのX座標を取得
int InputSystem::GetMouseX() {
	return mousePos.x;
}
// マウスのY座標を取得
int InputSystem::GetMouseY() {
	return mousePos.y;
}

int InputSystem::GetMouseScroll() {
    return scrollAmount;
}
// Vector2D_Iを取得.
Vector2DInt InputSystem::GetMousePoint_I() {
	return mousePos;
}
// Vector2Dを取得.
Vector2D InputSystem::GetMousePoint_F() {
	return Vector2D((float)mousePos.x, (float)mousePos.y);
}

// マウス （左、中、右ボタン）に応じて Countを加算.
void InputSystem::UpdateMouseCounter() {
	// 左ボタン
	if (IsMouseButton(MOUSE_INPUT_LEFT)) { mouseInputCount[LEFT]++; }
	else { mouseInputCount[LEFT] = 0; }
	// 中ボタン
	if (IsMouseButton(MOUSE_INPUT_MIDDLE)) { mouseInputCount[MIDDLE]++; }
	else { mouseInputCount[MIDDLE] = 0; }
	// 右ボタン
	if (IsMouseButton(MOUSE_INPUT_RIGHT)) { mouseInputCount[RIGHT]++; }
	else { mouseInputCount[RIGHT] = 0; }
}

// マウスボタンのインデックスを取得するメソッド.
int InputSystem::GetMouseButtonIndex(int button) {
	switch (button) {
	case MOUSE_INPUT_LEFT:   return LEFT;
	case MOUSE_INPUT_MIDDLE: return MIDDLE;
	case MOUSE_INPUT_RIGHT:  return RIGHT;
	default: return -1; // 無効なボタン.
	}
}