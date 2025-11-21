
#include "EnemyHoming.h"
#include "BulletBase.h"
#include "EnemyManager.h"
#include "Mathf.h"

void EnemyHoming::Update(std::shared_ptr<Transform2D> trs, BulletBase* bul) {
    frameCount++;
    if (!trs || !bul) return;

    // Gradually accelerate base speed up to maxSpeed every frame
    currentSpeed += speedIncreasePerFrame;
    if (currentSpeed > maxSpeed) currentSpeed = maxSpeed;

    // Before homing delay: simply accelerate to base speed
    if (frameCount < startDelayFrame) {
        bul->SetSpeed(currentSpeed);
        return;
    }

    // Homing active period
    if (frameCount <= maxHomingFrame) {
        if (auto t = target.lock()) {
            if (!t->transform) {
                target.reset();
                return;
            }

            // Calculate angles
            Vector2D dir = t->transform->position - trs->position;
            float targetAngle = std::atan2(dir.y, dir.x);
            float currentAngle = trs->rotation * Mathf::PI / 180.0f;

            // Normalize angle difference to [-PI, PI]
            float diff = targetAngle - currentAngle;
            while (diff > Mathf::PI) diff -= 2.0f * Mathf::PI;
            while (diff < -Mathf::PI) diff += 2.0f * Mathf::PI;

            // Increase homing power
            homingPower += homingAccelPerFrame;
            if (homingPower > 1.0f) homingPower = 1.0f;

            // Limit turn speed
            const float maxTurnSpeed = 7.5f * Mathf::PI / 180.0f; // ~3 deg/frame
            float turnSpeed = maxTurnSpeed * homingPower;
            float clampedDiff = std::clamp(diff, -turnSpeed, turnSpeed);

            // Apply rotation
            currentAngle += clampedDiff;
            trs->rotation = currentAngle * 180.0f / Mathf::PI;

            // ---- Turn-based deceleration ----
            float turningRatio = (turnSpeed > 0) ? std::abs(clampedDiff) / turnSpeed : 0.0f;
            float decelSpeed = currentSpeed - (currentSpeed - minSpeed) * turningRatio;

            // Recover towards base speed
            float newSpeed = Mathf::Lerp(bul->GetSpeed(), currentSpeed, recoveryLerp);
            if (turningRatio > 0.01f) {
                newSpeed = std::min(newSpeed, decelSpeed);
            }

            bul->SetSpeed(newSpeed);
        }
        else {
            // No valid target during homing: try re-target
            target = EnemyManager::GetInstance().GetEnemyTarget(trs->position);
            homingPower = 0.0f;
            float newSpeed = Mathf::Lerp(bul->GetSpeed(), currentSpeed, recoveryLerp);
            bul->SetSpeed(newSpeed);
        }
    }
    else {
        // After homing ends: continue base acceleration behavior
        float newSpeed = Mathf::Lerp(bul->GetSpeed(), currentSpeed, recoveryLerp);
        bul->SetSpeed(newSpeed);
    }
}

