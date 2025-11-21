/*
	Canvas class
*/
#include "Canvas.h"
#include "GameObject.h"
#include "GraphicRaycaster.h"

#include "RectTransform2D.h"
#include "RendererManager.h"
// コンストラクタ 
Canvas::Canvas() : AppBase("Canvas"){

}

Canvas::Canvas(std::shared_ptr<GameObject> owner) : AppBase("Canvas", owner) {

}
void Canvas::Awake() {
	RendererManager::GetInstance().AddRenderer(shared_from_this());
}
void Canvas::Update() {
    if (rectTransform.expired()) {
		rectTransform = std::dynamic_pointer_cast<RectTransform2D>(transform.lock());
    }
}
void Canvas::OnDestroy() {
    RendererManager::GetInstance().Remove(shared_from_this());
}
void Canvas::Draw() {

    // Canvas::Draw() 内でスケールと大きさを計算
    auto rt = rectTransform.lock();
    if (rt) {
        Vector2D screenSize = Window::GetInstance().GetMaxVector2D(); // 実際のスクリーン解像度
        Vector2D scale = scaler.GetScaleFactor(screenSize);

        switch (renderMode) {
        case RenderMode::ScreenSpaceOverlay:
        case RenderMode::ScreenSpaceCamera:
            rt->width  = screenSize.x;
            rt->height = screenSize.y;
			rt->anchoredPosition = screenSize * 0.5f; // 中央に配置
            rt->scale = scale; // ここも合わせる
            break;
        case RenderMode::WorldSpace:
            rt->scale = scale; // ワールド空間ではwidth/heightはRectTransformそのまま
            break;
        }
    }


    switch (renderMode) {
    case RenderMode::ScreenSpaceOverlay:
        DrawOverlayUI();
        break;
    case RenderMode::ScreenSpaceCamera:
        DrawCameraSpaceUI();
        break;
    case RenderMode::WorldSpace:
        DrawWorldUI();
        break;
    }
}

std::vector<std::shared_ptr<UiBase>> Canvas::GetUiBases() const {
    if (!transform) return {};
    return transform->GetAppBasesInChildren<UiBase>();
}


void Canvas::DrawOverlayUI() {
    for (auto& child : GetUiBases()) {
        if (child && child->GetGameObject()->IsActive()) 
            child->UiBaseDraw(shared_from_this());
    }
}

void Canvas::DrawCameraSpaceUI() {
    // カメラ考慮の描画（未実装仮）
    for (auto& child : GetUiBases()) {
        if (child && child->GetGameObject()->IsActive()) 
            child->UiBaseDraw(shared_from_this());
    }
}

void Canvas::DrawWorldUI() {
    for (auto& child : GetUiBases()) {
        if (child && child->GetGameObject()->IsActive()) 
            child->UiBaseDraw(shared_from_this());
    }
}