/*
	UiBase class
*/
#include "UiBase.h"
#include "GameObject.h"
#include "Canvas.h"
// コンストラクタ 
UiBase::UiBase() : AppBase("UiBase") {

}
UiBase::UiBase(std::shared_ptr<GameObject> owner) : AppBase("UiBase", owner) {
    if (gameObject) {
        rectTransform = gameObject->GetAppBase<RectTransform2D>();
    }
}

void UiBase::UiBaseDraw(const std::shared_ptr<Canvas> canvas) {
	if (!isRenderer) return;
    if (auto rect = GetRectTransform()) {
        OnUiDraw(rect->GetWorldQuad(),canvas);
    }
}

std::shared_ptr<RectTransform2D> UiBase::GetRectTransform() {
    auto cached = rectTransform.lock();
    if (cached) return cached;

    auto rt = std::dynamic_pointer_cast<RectTransform2D>(transform.lock());
    if (rt) {
        rectTransform = rt;
        return rt;
    }

    return 
        nullptr; // 変換できなかった場合
}
std::shared_ptr<Canvas> UiBase::GetRootCanvas() const {
    auto go = transform.lock();
    while (go) {
        if (auto canvas = go->GetGameObject()->GetAppBase<Canvas>()) {
            return canvas;
        }
        go = go->GetParent();
    }
    return nullptr;
}