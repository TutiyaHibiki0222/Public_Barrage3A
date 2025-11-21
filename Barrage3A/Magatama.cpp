/*
    ◆ Magatama.cpp

    クラス名        : Magatama クラス
    作成日          : 2025/07/10 (木) 0:27:11
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "Magatama.h"
#include "GameObject.h"
#include "GameObjectMgr.h"

// コンストラクタ 
Magatama::Magatama() : AppBase("Magatama"){

}
Magatama::Magatama(std::shared_ptr<GameObject> owner) : AppBase("Magatama", owner) {

}

// AppBase Event.
void Magatama::Start() {

}

void Magatama::LateUpdate() {

    // 回転（視覚効果用）
    transform->rotation = fmodf(Time.time * -180.0f, 360.f);

    auto sp = player.lock();
    if (!sp) return;

    // 1. Shift 状態に応じてターゲットオフセットを選ぶ
    Vector2D targetOffset = (Input.IsKey(KEY_INPUT_LSHIFT) || Input.IsPadButton<0>(PadCode::RightBumper)) ?
        focusedOffset : normalOffset;

    // 2. オフセットを補間
    currentOffset = currentOffset.Lerp(targetOffset, speedFactor);

    // 3. ターゲット位置を計算
    Vector2D targetPos = sp->transform->position + currentOffset;
    Vector2D delta = targetPos - transform->position;
    float dist = delta.Length();

    if (dist < 0.01f) return;

    float speed = std::clamp(dist * speedFactor, minSpeed, maxSpeed);
    Vector2D move = delta.GetNormalize() * speed * Time.deltaTime;
    if (move.Length() > dist) move = delta;

    transform->position += move;
}
