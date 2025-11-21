/*
    ◆ ShiftAura.cpp

    クラス名        : ShiftAura クラス
    作成日          : 2025/07/09 (水) 13:03:04
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/
#include "ShiftAura.h"
#include "GameObject.h"
#include "GameObjectMgr.h"

// コンストラクタ 
ShiftAura::ShiftAura(){

}
ShiftAura::ShiftAura(std::shared_ptr<GameObject> owner){

}

// AppBase Event.
void ShiftAura::Start() {
    sp = gameObject->GetAppBase<SpriteRenderer>();

    angle = startAngle;
    scale = startScale;
    appeared = false;
    elapsedTime = 0.0f;
    auto renderer = sp.lock();
    if (!renderer) return;
    renderer->GetColor().a = startA;
}

void ShiftAura::Tick() {
    auto renderer = sp.lock();
    if (!renderer) return;

    bool trigger = IsAuraTriggerHeld();

    // 状態切替時に現在の見た目を記録して補間再開用に保存
    if (state == AuraState::Appearing && !trigger) {
        currentScaleAtTransition = transform->scale.x; // Yも同じなので片方だけ
        currentAlphaAtTransition = renderer->GetColor().a;

        state = AuraState::Disappearing;
        elapsedTime = 0;
    }
    else if (state == AuraState::Disappearing && trigger) {
        currentScaleAtTransition = transform->scale.x;
        currentAlphaAtTransition = renderer->GetColor().a;

        state = AuraState::Appearing;
        elapsedTime = 0;
    }


    switch (state) {
    case AuraState::Hidden:
        if (trigger) {
            state = AuraState::Appearing;
            elapsedTime = 0;
            angle = startAngle;
            currentScaleAtTransition = startScale;
            currentAlphaAtTransition = startA;
        }
        break;

    case AuraState::Appearing: {
        elapsedTime += Time.deltaTime;
        float t = Mathf::Clamp01(elapsedTime / timeT);
        float angleT = Mathf::Clamp01(elapsedTime / (timeT * 0.75f));
        float easeT = EaseOutBack(t);

        scale = startScale + (endScale - startScale) * easeT;
        float alpha = startA + (endA - startA) * easeT;

        transform->scale = Vector2D(scale, scale);
        renderer->GetColor().a = alpha;

        float currentAngle = startAngle + (endAngle - startAngle) * angleT;
        transform->rotation = currentAngle;

        if (t >= 1.0f) {
            state = AuraState::Active;
            transform->scale = Vector2D(endScale, endScale);
            renderer->GetColor().a = endA;
            angle = endAngle; // 回転開始用に記録
        }
        break;
    }

    case AuraState::Active:
        // 回転
        angle += angleSpeed * Time.deltaTime;
        angle = Mathf::NormalizeAngle180(angle);
        transform->rotation = startAngle + angle;

        if (!trigger) {
            state = AuraState::Disappearing;
            elapsedTime = 0;
        }
        break;

    case AuraState::Disappearing: {
        elapsedTime += Time.deltaTime;
        float t = Mathf::Clamp01(elapsedTime / timeT);
        float easeT = 1.0f - EaseOutBack(t);
        easeT = Mathf::Clamp01(easeT);

        scale = startScale + (endScale - startScale) * easeT;
        float alpha = startA + (endA - startA) * easeT;

        transform->scale = Vector2D(scale, scale);
        renderer->GetColor().a = alpha;

        if (t >= 1.0f) {
            state = AuraState::Hidden;
            transform->scale = Vector2D(startScale, startScale);
            renderer->GetColor().a = startA;
        }
        break;
    }
    }
}
