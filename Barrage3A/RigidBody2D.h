#pragma once
#include "headers.h"
#include "AppBase.h"


// RigidBody2D class and AppBase class
class RigidBody2D : public AppBase {
    friend class CollisionManager;
protected:
    bool isKinematic = false;
    bool useGravity = true;
    bool isStatic = false;
    float mass = 1.0f;
    float frictionCoefficient = 0.05f;
    Vector2D velocity = { 0.0f, 0.0f };
    Vector2D acceleration = { 0.0f, 0.0f };
    Vector2D gravity = { 0.0f, 9.81f };
private:
    Vector2D nextPosition;
public:
    // コンストラクタ.
    RigidBody2D();
    RigidBody2D(GameObject*);
   
    // 外部から力を加えるための関数
    void AddForce(const Vector2D& force) {
        acceleration += force / mass;
    }

    // 質量の設定
    void SetMass(float _mass) { mass = _mass; }
    float GetMass() const { return mass; }

    // 重力の有効/無効
    void SetUseGravity(bool _use) { useGravity = _use; }
    bool GetUseGravity() const { return useGravity; }
    // 固定の有効/無効
    void SetStatic(bool _static) { isStatic = _static; }
    bool GetStatic() const { return isStatic; }
    // 速度の設定
    void SetVelocity(const Vector2D& _velocity) { velocity = _velocity; }
    Vector2D GetVelocity() const { return velocity; }
    // 重力の有効/無効
    void SetGravity(const Vector2D& _gravity) { gravity = _gravity; }
    Vector2D GetGravity() const { return gravity; }
    // 物理演算を行う関数群
    // 重力の適用
    void ApplyGravity();
    // 摩擦の適用
    void ApplyFriction();
    // 速度の更新
    void UpdateVelocity();
    // 位置の更新
    void UpdatePosition();

    // 固定更新時の物理演算
    void FixedUpdate() override;
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<RigidBody2D>(*this);
    }

    static void ApplyFriction(RigidBody2D* rb1, RigidBody2D* rb2, const Vector2D& normal)
    {
        if (!rb1 || !rb2) return;

        // 相対速度ベクトル
        Vector2D relativeVelocity = rb1->velocity - rb2->velocity;

        // 接触面の接線方向を計算（法線に対する垂直ベクトル）
        Vector2D tangent = Vector2D(-normal.y, normal.x);

        // 相対速度の接線成分を取得
        float relativeSpeedTangent = relativeVelocity.Dot(tangent);

        if (std::abs(relativeSpeedTangent) < 1e-6) return; // ほぼ0なら摩擦不要

        // 平均の摩擦係数
        float avgFrictionCoefficient = (rb1->frictionCoefficient + rb2->frictionCoefficient) * 0.5f;

        // 摩擦力の大きさ
        float frictionForceMagnitude = avgFrictionCoefficient * (rb1->mass + rb2->mass) * 9.8f;

        // 摩擦力を接線方向の単位ベクトルにかける
        Vector2D frictionForce = tangent * (-frictionForceMagnitude * std::copysign(1.0f, relativeSpeedTangent));

        // 摩擦力を適用
        rb1->velocity += frictionForce / rb1->mass;
        rb2->velocity -= frictionForce / rb2->mass;
    }
};