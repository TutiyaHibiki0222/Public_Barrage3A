#pragma once
#include "AppBase.h"
#include "ShapesRenderer.h"
#include "GameObject.h"

class ExplosionEffect : public AppBase
{
private:
    struct CircleInfo
    {
        std::shared_ptr<ShapesRenderer> renderer;
        Vector2D direction;
        float speed;
        float acceleration;
    };
    std::vector<CircleInfo> circles;
    float time = 0.0f;
    Vector2D centerPos = Vector2D(0, 0);
public:
    ExplosionEffect()
        : time(0.0f), AppBase("ExplosionEffect") {
    }

    void SetCenterPosition(const Vector2D& pos) {
        centerPos = pos;
    }

    void Start() override;
    void CreateCircle(const Vector2D& dir, float initSpeed, float accel, int l = 0);
    void Update() override;
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<ExplosionEffect>(*this);
    }
};