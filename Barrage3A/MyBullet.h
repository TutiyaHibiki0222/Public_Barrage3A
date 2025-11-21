/*
    ◆ MyBulletBase.cpp

    クラス名        : MyBulletBase クラス
    作成日          : 2025/07/11 (金) 12:29:32
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/
#pragma once

/*
#include "headers.h"
#include "AppBase.h"
#include "BulletInstruction.h"
#include "BulletControllerBase.h"
#include "BulletType.h"
class MyBullet;

// MyBulletBase class and AppBase class
class MyBullet : public AppBase , public std::enable_shared_from_this<MyBullet>
{
private:    // 非公開.
    float speed  = 0;
    float acceleration = 0.0f;
    float rotationRate = 0.0f;
    
    // 攻撃力.
    int   damage = 0;
    bool  isGraze = false;
    std::shared_ptr<BulletControllerBase> controller = nullptr;
public:     // 公開.

    // コンストラクタ.
    MyBullet();
    MyBullet(std::shared_ptr<GameObject>);

    float GetSpeed() const          { return speed; }
    void SetSpeed(float s)          { speed = s; }
    void SetAcceleration(float a)   { acceleration = a; }
    void SetRotationRate(float r)   { rotationRate = r; }

    // AppBase 関数.
    void Awake()    override;
    void Update()   override;
    void OnDestroy()override;

    void OnCollisionEnter(GameObject* other) override;

    void SetBulletControllerInstance(std::shared_ptr<BulletControllerBase> ctrl) {
        controller = std::move(ctrl);
    }

    void SetDamage(int _damage) {
        damage = _damage;
    }
    int GetDamage() const { return damage; }

    void SetBulletType(BulletParentID, BulletColor);

    template<
        typename T, typename... Args,
        typename = std::enable_if_t<
        std::is_base_of<BulletControllerBase, T>::value&&
        std::is_constructible<T, Args...>::value
        >
    >
    std::shared_ptr<T> SetBulletController(Args... args) {
        auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
        controller = ptr;
        return ptr;
    }
    void ClearMove() {
        controller.reset();
        controller = nullptr;
    }
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<MyBullet>(*this);
    }
};
*/