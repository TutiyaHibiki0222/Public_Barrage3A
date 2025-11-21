/*
    ◆ ShiftAura.cpp

    クラス名        : ShiftAura クラス
    作成日          : 2025/07/09 (水) 13:03:04
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "SpriteRenderer.h"
#include "TouhouBase.h"

// ShiftAura class and AppBase class
class ShiftAura : public TouhouBase {
private:    // 非公開.
    enum class AuraState {
        Hidden,     // 非表示状態（縮小完了）
        Appearing,  // 出現中アニメ
        Active,     // 表示中（回転し続ける）
        Disappearing // 消えるアニメ中
    };

    std::weak_ptr<SpriteRenderer> sp;
       
    float startScale = 0;
    float endScale   = 1;
    float scale      = 0;

    float timeT      = 0.5f;        // 変動にかかる時間.
    float startA     = 0;           // 開始透明度 0 ~ 1:
    float endA       = 1;           // 終了透明度 0 ~ 1:
    float timeTDisappear = 0.25f;   // 消える時は速めに

    float startAngle = 0;
    float endAngle   = 0;   // 出現時補間先の角度
    float angleSpeed = 0;   // 左右回転速度設定.
    float angle      = 0;   // 現在の角度.

    float elapsedTime = 0.0f;   // 経過時間
    bool appeared = false;      // 演出完了フラグ

    float easeBackS = 1.95f; // デフォルトの強さ
    AuraState state = AuraState::Hidden;

    float currentScaleAtTransition = 0.0f;
    float currentAlphaAtTransition = 0.0f;

public:     // 公開.

    // コンストラクタ.
    ShiftAura();
    ShiftAura(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Start()    override;
    void Tick()     override;

    // 設定関数
    void SetScaleRange(float start, float end) { startScale = start; endScale = end; }
    void SetAlphaRange(float start, float end) { startA = start; endA = end; }
    void SetAppearTime(float t) { timeT = t; }
    void SetDisappearTime(float t) { timeTDisappear = t; }
    void SetRotation(float startDeg, float speed) { startAngle = startDeg; endAngle = startDeg; angleSpeed = speed; }
    void SetEaseBackStrength(float s) { easeBackS = s; }
    void SetRotation(float startDeg, float targetDeg, float speed) {
        startAngle = startDeg;
        endAngle = targetDeg;
        angleSpeed = speed;
    }

    // EaseOutBack関数
    float EaseOutBack(float t) {
        t -= 1.0f;
        return t * t * ((easeBackS + 1) * t + easeBackS) + 1.0f;
    }

    bool IsAuraTriggerHeld() {
        return Input.IsKey(KeyCode::LeftShift) || Input.IsPadButton<0>(PadCode::RightBumper); // 仮想キー
    }

protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<ShiftAura>(*this);
    }
};