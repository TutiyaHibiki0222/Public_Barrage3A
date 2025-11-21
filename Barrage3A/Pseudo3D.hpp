#pragma once
#include "AppBase.h"
#include "Transform2D.h"
#include "Time.h"
#include "SpriteRenderer.h"  // スプライトを取得するため

// 疑似3D効果を付与するAppBase：Y補正 + スプライトのスケーリング
class Pseudo3D : public AppBase {
private:
    float depth = 1.0f;       // 擬似Z軸（0.1～1.0）。1.0f = 手前、0.0f = 奥
    float maxDepth;
    float baseY = 0.0f;       // 初期のY座標
    float baseScale = 1.0f;   // スプライトの初期スケール（Y方向はXと同じにする）

public:
    Pseudo3D() : AppBase("Pseudo3D") {}

    void SetDepth(float z) {
        depth = std::clamp(z, 0.001f, maxDepth); // 安全な範囲に制限
    }
    void SetMaxDepth(float z) {
        maxDepth = z;
    }

    void Awake() override {
        baseY = transform->position.y;

        // SpriteRenderer があればスケール保存
        baseScale = transform->scale.x;  // 初期スケールを記録（X基準）
    }

    void Update() override {
        // Y座標に応じて depth を自動計算（仮定：y=600で手前、y=300で奥）
        float y = transform->position.y;
        float z = maxDepth - HEIGHT / 2 / ((HEIGHT - y)); // yが600のとき z=1.0、yが300のとき z=0.0
        depth = std::clamp(z, 0.001f, maxDepth);        // 最小スケール補正あり

        // スケールを depth に応じて補正
        float scale = baseScale * depth;
        transform->scale = { scale, scale };
        
        transform->position.y -= 1;
    }

    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Pseudo3D>(*this);
    }
};
