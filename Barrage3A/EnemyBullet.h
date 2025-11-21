/*
    ◆ EnemyBullet.h

    クラス名        : EnemyBullet クラス
    作成日          : 2025/11/16 (日) 16:45:27
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "BulletBase.h"
// EnemyBullet class and AppBase class
class EnemyBullet : public BulletBase, public std::enable_shared_from_this<EnemyBullet>
{
private:    // 非公開.
    bool isGraze = false;
public:     // 公開.

    // コンストラクタ.
    EnemyBullet();
    EnemyBullet(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Awake()    override;
    void OnDestroy()   override;
    void OnCollisionEnter(GameObject* other) override;

    void SetBulletControllerInstance(std::shared_ptr<BulletControllerBase> ctrl) {
        controller = std::move(ctrl);
    }
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<EnemyBullet>(*this);
    }
};