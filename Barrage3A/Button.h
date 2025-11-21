#pragma once
#include "headers.h"
#include "UiBase.h"
#include "IDraw.h"
#include "Delegate.hpp"
#include "RectTransform2D.h"
#include "Texture2D.h"
#include "Image.h"

struct ColorBlock {
    Color normalColor       = Color(1.0f, 1.0f, 1.0f, 1.0f);
    Color highlightedColor  = Color(0.8f, 0.8f, 0.8f, 1.0f);
    Color pressedColor      = Color(0.6f, 0.6f, 0.6f, 1.0f);
    Color disabledColor     = Color(0.4f, 0.4f, 0.4f, 1.0f);
};

// Button class and AppBase class
class Button : public UiBase {
private:
    bool isPressed = false;

    std::weak_ptr<Image> image;

    ColorBlock colors;          // ここで ColorBlock を持つ
    Color baseImageColor{};     // Image の元の色
    Color currentColor  {};     // 現在の表示色
    Color targetColor   {};     // 目標の色

    float fadeDuration = 0.5f;    // フェードにかける秒数
    float fadeElapsedTime = 0.0f; // フェード開始からの経過時間

public:     // 公開.

    System::Delegate<void> onClick;
    System::Delegate<void> onHover;
    System::Delegate<void> onPress;
    System::Delegate<void> onRelease;

    // コンストラクタ.
    Button();
    Button(std::shared_ptr<GameObject>);

    void Start() override;
    void LateUpdate() override;

    // 色設定を外部から可能にする
    void SetColorBlock(const ColorBlock& cb) { colors = cb; }
    const ColorBlock& GetColorBlock() const { return colors; }

    void SetFadeDuration(float duration) {
        fadeDuration = duration;
	}
private:
    // AppBase 関数.
    void OnUiDraw(const Quad2D&, const std::shared_ptr<Canvas>&) override;

    // イベントハンドラオーバーライド
    void UpdateTargetColor(const Color& stateColor) {
        if (auto img = image.lock()) {
            targetColor = baseImageColor * stateColor;
			fadeElapsedTime = 0.0f; // フェード開始
        }
    }

    void OnMouseDown() override {
        isPressed = true;
        UpdateTargetColor(colors.pressedColor);
        onPress.Invoke();
    }

    void OnMouseUp() override {
        if (isPressed) {
            UpdateTargetColor(colors.normalColor);
            onRelease.Invoke();
            onClick.Invoke();
            isPressed = false;
        }
    }

    void OnHover() override {
        if (!isPressed) {
            UpdateTargetColor(colors.highlightedColor);
            onHover.Invoke();
        }
    }

    void OnMouseExit() override {
        if (!isPressed) {
            UpdateTargetColor(colors.normalColor);
        }
    }
};