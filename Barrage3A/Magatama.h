/*
    ◆ Magatama.h

    クラス名        : Magatama クラス
    作成日          : 2025/07/10 (木) 0:27:11
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include <deque>

// Magatama class and AppBase class
class Magatama : public AppBase {
private:    // 非公開.
public:     // 公開.
    Vector2D normalOffset;      // 通常時のオフセット
    Vector2D focusedOffset;     // 低速（Shift）時のオフセット
    Vector2D currentOffset;     // 現在の補間オフセット
    std::weak_ptr<GameObject> player;  // 追従対象プレイヤーの弱参照

    float minSpeed = 150.f;     // 最小移動速度（ピクセル/秒）
    float maxSpeed = 500.f;     // 最大移動速度
    float speedFactor = 12.5f;    // 距離にかける係数（調整用）

    // コンストラクタ.
    Magatama();
    Magatama(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Start()    override;
    void LateUpdate()   override;

protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Magatama>(*this);
    }
};