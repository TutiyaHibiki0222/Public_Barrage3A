#pragma once

#include "BulletControllerBase.h"
#include "GameObject.h"

class EnemyHoming : public BulletControllerBase
{
private:
    std::weak_ptr<GameObject> target;

    // Homing parameters
    float homingPower = 0.0f;
    float homingAccelPerFrame = 0.025f;     // Homing strength increase per frame
    const int startDelayFrame = 3;          // Delay before homing starts (~0.25s)
    const int maxHomingFrame = 180;         // Homing duration (~3s)

    // Speed parameters
    const float maxSpeed;                   // Maximum speed
    const float minSpeed = 6.f;            // Minimum speed when turning
    float currentSpeed   = 0.0f;            // Bullet's base speed, accelerates up to maxSpeed
    float speedIncreasePerFrame = 0.085f;   // Acceleration per frame
    const float recoveryLerp = 0.1f;        // Speed recovery Lerp factor

    int frameCount = 0;

public:
    explicit EnemyHoming(float _maxSpeed)
        : maxSpeed(_maxSpeed * 1.5f)
    {
        currentSpeed = minSpeed; // Start at minSpeed
    }

    void Update(std::shared_ptr<Transform2D> trs, BulletBase* bul) override;
};
