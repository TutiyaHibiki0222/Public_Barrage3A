/*
	GraphicRaycaster class
*/
#include "GraphicRaycaster.h"
#include "GameObject.h"

// コンストラクタ 
GraphicRaycaster::GraphicRaycaster() : AppBase("GraphicRaycaster"){

}
GraphicRaycaster::GraphicRaycaster(std::shared_ptr<GameObject> owner) : AppBase("GraphicRaycaster", owner) {

}

void GraphicRaycaster::Update() {
    Vector2D mousePos = Input.GetMousePoint_F();
    for (auto& ui : transform->GetAppBasesInChildren<UiBase>()) {
		if (ui->GetGameObject()->IsActive() == false || !ui->raycastTarget) continue;
        if (ui->Raycast(mousePos)) {
            if (!ui->isHoveredLastFrame) {
                ui->OnMouseEnter();
                ui->isHoveredLastFrame = true;
            }

            ui->OnHover();

            if (Input.IsMouseButtonDown(MouseCode::Left)) {
                ui->OnMouseDown();
            }

            if (Input.IsMouseButtonUp(MouseCode::Left)) {
                ui->OnMouseUp();
                ui->OnClick();

                // ダブルクリック判定
                float doubleClickThreshold = 0.3f;
                if ((Time.time - lastClickTimeMap[ui]) < doubleClickThreshold) {
                    ui->OnDoubleClick();
                }
                lastClickTimeMap[ui] = Time.time;
            }

            if (Input.IsMouseButtonDown(MouseCode::Right)) {
                ui->OnRightClick();
            }

            // スクロール
            int scrollAmount = Input.GetMouseScroll();
            if (scrollAmount != 0) {
                ui->OnScroll(scrollAmount);
            }

            // ドラッグ判定
            if (Input.IsMouseButton(MouseCode::Left)) {
                if (!uiDragState[ui]) {
                    uiDragState[ui] = true;
                    ui->dragStartPos = mousePos;
                    ui->OnBeginDrag();
                }
                else if (ui->dragStartPos != mousePos) {
                    ui->OnDrag();
                    ui->dragStartPos = mousePos;
                }
            }
            else {
                if (uiDragState[ui]) {
                    uiDragState[ui] = false;
                    ui->OnEndDrag();
                }
            }
        }
        else {
            if (ui->isHoveredLastFrame) {
                ui->OnMouseExit();
                ui->isHoveredLastFrame = false;
            }
            // ドラッグ状態が続いている場合、UI 要素の外でもドラッグイベントを処理
            if (uiDragState[ui]) {
                if (Input.IsMouseButton(MouseCode::Left)) {
                    if (ui->dragStartPos != mousePos) {
                        ui->OnDrag();
                        ui->dragStartPos = mousePos;
                    }
                }
                else {
                    uiDragState[ui] = false;
                    ui->OnEndDrag();
                }
            }
        }
    }
}