#pragma once
#include "headers.h"
#include "UiBase.h"
#include "IDraw.h"
#include "Delegate.hpp"
#include "RectTransform2D.h"
#include "Texture2D.h"
#include "Image.h"

class FilterUI : public UiBase {
private:
    int sceneHandle = -1;
    int graphHandle = -1;
    int filterMode = DX_GRAPH_FILTER_GAUSS;

public:
    FilterUI() = default;
    ~FilterUI() {
        if (sceneHandle != -1) {
            DeleteGraph(sceneHandle);
        }
    }

    void OnUiDraw(const Quad2D& q, const std::shared_ptr<Canvas>& c) override {
        int screenW = WIDTH;
        int screenH = HEIGHT;

        if (sceneHandle == -1) {
            sceneHandle = MakeScreen(screenW, screenH, TRUE);
        }

        // 現在のスクリーンをオフスクリーンにコピー
        GetDrawScreenGraph(0, 0, screenW, screenH, sceneHandle);

        // ぼかし処理
        GraphFilter(sceneHandle, filterMode, 16,125);

        // 元のスクリーンに描画
        DrawGraph(0, 0, sceneHandle, TRUE);
    }
};
