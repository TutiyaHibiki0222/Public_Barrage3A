#pragma once
#include "headers.h"
#include "AppBase.h"
#include "UiBase.h"
#include "WeakAccessor.hpp"
#include "IDraw.h"
#include "RectTransform2D.h"

class CanvasScaler {
public:
    enum class ScaleMode {
        ConstantPixelSize,
        ScaleWithScreenSize,
        ConstantPhysicalSize
    } 
    scaleMode = ScaleMode::ScaleWithScreenSize;

    Vector2D referenceResolution = { WIDTH, HEIGHT };

    Vector2D GetScaleFactor(const Vector2D& currentResolution) const {
        switch (scaleMode) {
        case ScaleMode::ConstantPixelSize:
            return { 1.0f, 1.0f };
        case ScaleMode::ScaleWithScreenSize:
            return currentResolution / referenceResolution;
        case ScaleMode::ConstantPhysicalSize:
            return { 1.0f, 1.0f };
        }
        return { 1.0f, 1.0f };
    }
};

// Canvas class and AppBase class
class Canvas : public AppBase, public IRendererDraw , public std::enable_shared_from_this<Canvas>
{
private:    // 非公開.
    int canvasOrder = 0;
    CanvasScaler scaler;
    RenderMode renderMode = RenderMode::ScreenSpaceOverlay;
	std::weak_ptr<RectTransform2D> rectTransform;; // 所属する GameObject.
public:     // 公開.

    // コンストラクタ.
    Canvas();
    Canvas(std::shared_ptr<GameObject>);

    void SetOrder(int order) { canvasOrder = order; }
    int GetSortingOrder() const override { return canvasOrder; }

    void SetScaleMode(CanvasScaler::ScaleMode mode) { scaler.scaleMode = mode; }
    void SetReferenceResolution(const Vector2D& res) { scaler.referenceResolution = res; }

    void SetRenderMode(RenderMode mode) { renderMode = mode; }
    RenderMode GetRenderMode() const override { return renderMode; }

    void Draw() override;
    // AppBase 関数.
	void Awake()     override;
    void Update()    override;
	void OnDestroy() override;
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Canvas>(*this);
    }

private:
    std::vector<std::shared_ptr<UiBase>> GetUiBases() const;

    void DrawOverlayUI();
    void DrawCameraSpaceUI();
    void DrawWorldUI();
};