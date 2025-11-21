#pragma once
#include "headers.h"
#include "AppBase.h"
#include "TouhouBase.h"
#include "Collider2D.h"
#include "BulletControllerBase.h"
#include "BulletType.h"

class BulletBase : public TouhouBase {
protected:
    float speed = 0.0f;
    float accel = 0.0f;
    float angle = 0.0f;   // 現在角度
    float rotVel = 0.0f;   // 角速度（回転レート）

    int damage = 0;

    System::WeakAccessor<Collider2D> collider;

    std::shared_ptr<BulletControllerBase> controller = nullptr;
    
    BulletBase() = default;
public:

    void Start() override;
    void Tick() override;

	virtual void Initialize() {}
    virtual void OnCollisionEnter(GameObject* other) override = 0;

    // ---- Controller ----
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

    void ClearController() { controller.reset(); }

    // ---- Bullet Parameters ----
    float GetSpeed() const { return speed; }
    void SetSpeed(float s) { speed = s; }

    void SetAcceleration(float a) { accel = a; }

    // 「回転レート」と「角度」を分離
    void SetRotationRate(float r) { rotVel = r; }
    void SetAngle(float a) { angle = a; }

    void SetDamage(int d) { damage = d; }
    int  GetDamage() const { return damage; }

    // ---- Appearance ----
    void SetBulletType(BulletParentID, BulletColor);

protected:
    virtual std::shared_ptr<AppBase> Clone() const override = 0;
};
