/*
	RigidBody2D class
*/
#include "RigidBody2D.h"
#include "GameObject.h"

// コンストラクタ 
RigidBody2D::RigidBody2D() : AppBase("RigidBody2D"), velocity(0.0f, 0.0f), acceleration(0.0f, 0.0f), mass(1.0f), isKinematic(false) {

}
RigidBody2D::RigidBody2D(GameObject* owner) : AppBase("RigidBody2D", owner), velocity(0.0f, 0.0f), acceleration(0.0f, 0.0f), mass(1.0f), isKinematic(false) {

}

void RigidBody2D::ApplyGravity() {
    if (useGravity && !isStatic) {
        acceleration += gravity;  // 重力加速度を時間で加算
    }
}

void RigidBody2D::ApplyFriction() {
    if (velocity.Length() > 0) {
        Vector2D friction = -velocity.GetNormalize() * frictionCoefficient;
        acceleration += friction;  // 摩擦を時間で調整
    }
}

void RigidBody2D::UpdateVelocity() {
    velocity += acceleration;  // 加速度を時間に応じて更新
}

void RigidBody2D::UpdatePosition() {
    if (gameObject) {
        auto transform = gameObject->transform;
        if (transform) {
            nextPosition = transform->position + (velocity * Time.deltaTime);  // 時間で位置更新
        }
    }
}

void RigidBody2D::FixedUpdate() {
    if (!isKinematic) {
        // 各種物理処理
        ApplyGravity();       // 重力適用
        ApplyFriction();      // 摩擦適用
        UpdateVelocity();     // 速度更新
        UpdatePosition();     // 位置更新

        // 加速度リセット
        acceleration = Vector2D();

        if (gameObject && !isStatic) {
            gameObject->transform->position = nextPosition;  // 確定位置を適用
        }
    }
}

