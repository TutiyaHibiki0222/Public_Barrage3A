#pragma once
#include "headers.h"
#include "AppBase.h"
#include "UiBase.h"
// GraphicRaycaster class and AppBase class
class GraphicRaycaster : public AppBase {
private:    // 非公開.
    std::unordered_map<std::shared_ptr<UiBase>, bool> uiDragState;
    std::unordered_map<std::shared_ptr<UiBase>, float> lastClickTimeMap;
public:     // 公開.

    // コンストラクタ.
    GraphicRaycaster();
    GraphicRaycaster(std::shared_ptr<GameObject>);

    // Update.
    void Update() override;
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<GraphicRaycaster>(*this);
    }
};