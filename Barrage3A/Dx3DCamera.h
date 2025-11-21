#pragma once
#include <DxLib.h>
#include "Vector.h" // ← 自前Vector3などがあればインクルード
#include "IDraw.h"

class Dx3DCamera {
private:
    VECTOR position;    // カメラ位置
    VECTOR target;      // 注視点
    VECTOR upVector;    // 上方向ベクトル
    float nearZ;
    float farZ;
    float screenCenterX;
    float screenCenterY;

    // Fog設定
    bool fogEnabled;
    Color fogColor = Color(0,0,0);       // RGB値
    float fogStart;
    float fogEnd;

    Dx3DCamera();
public:
    // Singleton インスタンス取得
    static Dx3DCamera& Instance() {
        static Dx3DCamera instance;
        return instance;
    }
    // 基本設定
    void SetPosition(const VECTOR& pos);
    void SetTarget(const VECTOR& tgt);
    void SetUpVector(const VECTOR& up);
    void SetNearFar(float nearPlane, float farPlane);
    void SetScreenCenter(float centerX, float centerY);
    // カメラを注視点を中心に回転させる（水平回転・高さ指定）
    void OrbitAroundTarget(float angleRad, float radius, float height);
    // 任意の角度と高度で球面回転したい場合（オプション）
    void OrbitAroundTargetSpherical(float azimuthRad, float elevationRad, float radius);
    // 一括設定（便利）
    void SetCamera(const VECTOR& pos, const VECTOR& tgt, const VECTOR& up = VGet(0.f, 1.f, 0.f));

    // カメラ適用
    void Apply();  // DxLib に反映

    void Reset();
    // Getter（必要に応じて）
    const VECTOR& GetPosition() const { return position; }
    const VECTOR& GetTarget() const { return target; }
    const VECTOR& GetUpVector() const { return upVector; }

    // Fog設定
    void SetFogEnable(bool enable);
    void SetFogColor(int r, int g, int b);
    void SetFogRange(float start, float end);

    // コピー防止
    Dx3DCamera(const Dx3DCamera&) = delete;
    Dx3DCamera& operator=(const Dx3DCamera&) = delete;
};
