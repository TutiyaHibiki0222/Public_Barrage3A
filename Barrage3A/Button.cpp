/*
	Button class
*/
#include "Button.h"
#include "GameObject.h"
#include "Canvas.h"
// コンストラクタ 
Button::Button() : UiBase(){

}
Button::Button(std::shared_ptr<GameObject> owner) : UiBase(owner) {

}

void Button::Start() {
	currentColor = colors.normalColor;
	targetColor  = colors.normalColor;
	image = gameObject->GetAppBase<Image>();
    auto img = image.lock();
    if (img) {
        baseImageColor = img->GetColor();
        currentColor = baseImageColor * colors.normalColor;
        targetColor = currentColor;
    }
}

void Button::LateUpdate() {
    fadeElapsedTime += Time.deltaTime;
    float t = fadeDuration > 0.0f ? Mathf::Min(fadeElapsedTime / fadeDuration, 1.0f) : 1.0f;

    currentColor = Color::Lerp(currentColor, targetColor, t);

    auto img = image.lock();
    if (img) {
        img->GetColor() = currentColor;
    }
}

void Button::OnUiDraw(const Quad2D& quad, const std::shared_ptr<Canvas>& canvas) {
    auto img = image.lock();
    if (img) img->OnUiDraw(quad, canvas);
}