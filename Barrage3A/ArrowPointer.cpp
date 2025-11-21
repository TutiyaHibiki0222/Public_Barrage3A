/*
    ◆ ArrowPointer.cpp

    クラス名        : ArrowPointer クラス
    作成日          : 2025/07/21 (月) 17:14:17
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "ArrowPointer.h"
#include "GameObject.h"
#include "GameObjectMgr.h"

// コンストラクタ 
ArrowPointer::ArrowPointer() : AppBase("ArrowPointer"){

}
ArrowPointer::ArrowPointer(std::shared_ptr<GameObject> owner) : AppBase("ArrowPointer", owner) {

}
void ArrowPointer::SetTarget(std::shared_ptr<ItemBase> item) {
    targetItem = item;
	spriteRenderer = item->GetGameObject()->GetAppBase<SpriteRenderer>();
	itemSprite = spriteRenderer.lock()->GetSprite();
}
// AppBase Event.
void ArrowPointer::Update() {
    if (auto item = targetItem.lock()) {
        if (auto sr = spriteRenderer.lock()) {
            const Vector2D& itemPos = item->GetGameObject()->transform->position;

            float halfHeight = Window::GetInstance().GetMaxVector2D().y * 0.5f;
            const float margin = 18.0f;  // 画面端からの余白

            // 画面の上端 Y座標 (画面中心基準)
            float screenTopY = halfHeight;

            // アイテムが画面上端より上にいるかチェック
            if (itemPos.y > screenTopY - margin) {
                float arrowBaseY = screenTopY - margin; // 矢印を置きたい画面上端付近のY座標

                // アイテムの上端（position.y - アイテムの高さ / 2）を計算（必要に応じて調整）
                float itemTopY = itemPos.y + (arrawSprite->height);

                // 矢印のオフセットYは「矢印の基本位置 - アイテムの上端」
                float arrowOffsetY = itemTopY - arrowBaseY;
                sr->SetOffset(Vector2D(0.0f, arrowOffsetY));
                sr->SetSprite(arrawSprite);
            }
            else {
                // アイテムが画面内に戻った場合は非表示＆offsetリセット
                sr->SetOffset(Vector2D(0.0f, 0.0f));
                sr->SetSprite(itemSprite);
            }
        }
    }
}







