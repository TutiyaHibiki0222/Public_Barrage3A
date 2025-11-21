/*
    ◆ PlayerBullet.cpp

    クラス名        : PlayerBullet クラス
    作成日          : 2025/07/09 (水) 14:59:43
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "BulletBase.h"
#include "Animator.h"

// PlayerBullet class and AppBase class
class PlayerBullet : public BulletBase {
private:    // 非公開.

public:     // 公開.
    System::WeakAccessor<Animator> animator;

    // 当たり時に呼ばれるカスタムラムダ
    std::function<void(PlayerBullet*)> onHitLambda;

    // コンストラクタ.
    PlayerBullet();
    PlayerBullet(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Initialize()    override;
    void OnCollisionEnter(GameObject* other) override;

protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<PlayerBullet>(*this);
    }
};