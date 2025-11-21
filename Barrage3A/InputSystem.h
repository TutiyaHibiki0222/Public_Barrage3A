#pragma once
/*
	InputSystem.h
*/

#include "Code.h"
#include "Vector.h"
#include "LibrarySettings.h"
#include <functional>

namespace GameEngine::Utilities {
    template<size_t N>
    constexpr bool has_maxPad = (N < LibrarySettings::MaxPadCount);
};

struct PadState {
    std::array<unsigned char, 16> current{};
    std::array<unsigned char, 16> previous{};
    unsigned char leftTrigger = 0;
    unsigned char rightTrigger = 0;
    short thumbLX = 0, thumbLY = 0;
    short thumbRX = 0, thumbRY = 0;
    bool isConnected = false;
};

class InputSystem {
private:
	// キーボードの状態を管理する配列.
	std::array<int, 256> keyStateCurrent;  // 現在のキーの状態.
	std::array<int, 256> keyStatePrevious; // 前フレームのキーの状態.

	// マウスの状態.
	Vector2DInt mousePos;			 // マウスの座標.
	int mouseInputCurrent;           // 現在のマウスボタンの状態.
	int mouseInputPrevious;          // 前フレームのマウスボタンの状態.
    int scrollAmount;
	//  マウスのボタンが押されている時間管理する配列をわかりやすくする.
	enum { LEFT, MIDDLE, RIGHT, MOUSE_COUNT };
	// マウスのボタンが押されている時間を管理する配列（左、中、右ボタン）.
	std::array<int, MOUSE_COUNT> mouseInputCount;

	// パッド.
    const float MAX_STICK = 32767.0f;
	std::array<PadState, LibrarySettings::MaxPadCount> pads;	    // 現在のパッド状態.

	// コンストラクタは外部からアクセスを禁止.
	InputSystem();
    ~InputSystem() = default;

public:
	// インスタンスを取得するための静的メソッド.
    static InputSystem& GetInstance() {
        static InputSystem instance;
        return instance;
    }

	// 入力の状態を更新する.
	void Update();

	// キーが押されているかをチェック.
	bool IsKey(int key);
	// キーが押された瞬間をチェック.
	bool IsKeyDown(int key);
	// キーが離された瞬間をチェック.
	bool IsKeyUp(int key);

	// マウスボタンが押されているかをチェック.
	bool IsMouseButton(int button);
	// マウスボタンが押された瞬間をチェック.
	bool IsMouseButtonDown(int button);
	// マウスボタンが離された瞬間をチェック.
	bool IsMouseButtonUp(int button);

	// マウスボタンがクリックされたかをチェック.
	bool IsMouseButtonClicked(int button, int clickDuration);
	// マウスボタンが長押しされているかをチェック
	bool IsMouseButtonLongPressed(int button, int longPressDuration);

    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    bool IsActivePad() {
        return pads[N].isConnected;
    }

    // Padに何ボタン押された押されているかをチェック.
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    bool IsPadButton(int button) {
        auto pad = pads[N];
        return pad.current[(int)button] != 0;
    }

    // Padに何ボタン押された瞬間をチェック.
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    bool IsPadButtonDown(int button) {
        auto pad = pads[N];
        return pad.current[(int)button] != 0 && pad.previous[(int)button] == 0;
    }

    // Padに何ボタン押された離された瞬間をチェック.
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    bool IsPadButtonUp(int button) {
        auto pad = pads[N];
        return pad.current[(int)button] == 0 && pad.previous[(int)button] != 0;
    }


	// マウスボタンが押されているかをチェック.
	bool IsMouseButton(MouseCode button);
	// マウスボタンが押された瞬間をチェック.
	bool IsMouseButtonDown(MouseCode button);
	// マウスボタンが離された瞬間をチェック.
	bool IsMouseButtonUp(MouseCode button);

	// キーが押されているかをチェック.
	bool IsKey(KeyCode key);
	// キーが押された瞬間をチェック.
	bool IsKeyDown(KeyCode key);
	// キーが離された瞬間をチェック.
	bool IsKeyUp(KeyCode key);

    // Padに何ボタン押された押されているかをチェック.
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    bool IsPadButton(PadCode button) {
        auto pad = pads[N];
        return pad.current[(int)button] != 0;
    }

    // Padに何ボタン押された瞬間をチェック.
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    bool IsPadButtonDown(PadCode button) {
        auto pad = pads[N];
        return pad.current[(int)button] != 0 && pad.previous[(int)button] == 0;
    }

    // Padに何ボタン押された離された瞬間をチェック.
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    bool IsPadButtonUp(PadCode button) {
        auto pad = pads[N];
        return pad.current[(int)button] == 0 && pad.previous[(int)button] != 0;
    }

	// マウスのX座標を取得
	int GetMouseX();
	// マウスのY座標を取得
	int GetMouseY();
	// Vector2D_Iを取得.
	Vector2DInt GetMousePoint_I();
	// Vector2Dを取得.
	Vector2D GetMousePoint_F();

    int GetMouseScroll();

    // ステックLを取得,
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    Vector2D GetLstick(float deadZone = LibrarySettings::StickDeadZone) {
        Vector2D raw = Vector2D((float)pads[N].thumbLX / MAX_STICK, (float)pads[N].thumbLY / MAX_STICK);
        if (raw.Length() < deadZone)
            return Vector2D(0, 0);
        return raw.GetNormalize();
    }

    // スティックR取得
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    Vector2D GetRstick(float deadZone = LibrarySettings::StickDeadZone) {
        Vector2D raw = Vector2D((float)pads[N].thumbRX / MAX_STICK, (float)pads[N].thumbRY / MAX_STICK);
        if (raw.Length() < deadZone)
            return Vector2D(0, 0);
        return raw.GetNormalize();
    }

    // L + R 合成スティック取得
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    Vector2D GetStick(float deadZone = LibrarySettings::StickDeadZone) {
        Vector2D l((float)pads[N].thumbLX / MAX_STICK, (float)pads[N].thumbLY / MAX_STICK);
        Vector2D r((float)pads[N].thumbRX / MAX_STICK, (float)pads[N].thumbRY / MAX_STICK);
        Vector2D combined = (l + r) * 0.5f;

        if (combined.Length() < deadZone)
            return Vector2D(0, 0);
        return combined.GetNormalize();
    }

    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    Vector2D GetLstickStrength(float deadZone = LibrarySettings::StickDeadZone) {
        Vector2D raw((float)pads[N].thumbLX / MAX_STICK, (float)pads[N].thumbLY / MAX_STICK);

        float len = raw.Length();
        if (len < deadZone)
            return Vector2D(0, 0);

        // デッドゾーンを除去した強さを 0.0〜1.0 に再スケーリング
        float strength = (len - deadZone) / (1.0f - deadZone);
        strength = Mathf::Clamp01(strength);
        return raw.GetNormalize() * strength;
    }

    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    Vector2D GetRstickStrength(float deadZone = LibrarySettings::StickDeadZone) {
        Vector2D raw((float)pads[N].thumbLX / MAX_STICK, (float)pads[N].thumbLY / MAX_STICK);

        float len = raw.Length();
        if (len < deadZone)
            return Vector2D(0, 0);

        // デッドゾーンを除去した強さを 0.0〜1.0 に再スケーリング
        float strength = (len - deadZone) / (1.0f - deadZone);
        strength = Mathf::Clamp01(strength);
        return raw.GetNormalize() * strength;
    }

    
    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    PadState GetPadState()
    {
        return pads[N];
    }

    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    int GetPadButton(PadCode button) {
        if (IS_VALID_PadCode(button)) {
            return pads[N].current[(int)button];
        }
        return 0;
    }

    template <typename... Keys, typename = std::enable_if_t<sizeof...(Keys) != 0>>
    bool IsAnyKey(Keys... keys) {
        return (... || IsKey(keys));
    }

    template <typename... Keys, typename = std::enable_if_t<sizeof...(Keys) != 0>>
    bool IsAnyKeyDown(Keys... keys) {
        return (... || IsKeyDown(keys));
    }

    template <typename... Keys, typename = std::enable_if_t<sizeof...(Keys) != 0>>
    bool IsAnyKeyUp(Keys... keys) {
        return (... || IsKeyUp(keys));
    }

    template <typename... Keys, typename = std::enable_if_t<(sizeof...(Keys) > 1)>>
    bool IsOnlyKeyCombo(Keys... keys) {
        return (... && IsKey(keys));
    }

    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    void SetPadVibration(int power,float timeSec = 1.f) {
        constexpr int padId = DX_INPUT_PAD1 + static_cast<int>(N); // DX定数に変換

        if (!IsActivePad<N>()) return;

        // DxLibでは 0〜1000 の範囲
        power = Mathf::Clamp(power, 0, 1000);
        int durationMs = (timeSec < 0.0f) ? -1 : Mathf::Round<int>(timeSec * 1000.0f);

        StartJoypadVibration(padId, power, durationMs);
    }

    template<size_t N, typename = std::enable_if_t<GameEngine::Utilities::has_maxPad<N>>>
    void StopPadVibration() {
        constexpr int padId = DX_INPUT_PAD1 + static_cast<int>(N);
        StopJoypadVibration(padId);  // DxLibの関数
    }

private:
	// マウス （左、中、右ボタン）に応じて Countを加算.
	void UpdateMouseCounter();
	// マウスボタンのインデックスを取得するメソッド.
	int GetMouseButtonIndex(int button);
};

inline InputSystem& GetInputSystem() { return InputSystem::GetInstance();}
// 入力.
#define Input InputSystem::GetInstance()