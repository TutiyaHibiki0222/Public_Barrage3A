#include "Dx3DCamera.h"

Dx3DCamera::Dx3DCamera()
    : position(VGet(0.0f, 0.0f, -100.0f)),
    target(VGet(0.0f, 0.0f, 0.0f)),
    upVector(VGet(0.0f, 1.0f, 0.0f)),
    nearZ(1.0f), farZ(10000.f),
    screenCenterX(0), screenCenterY(0) ,
    fogEnabled(false),
    fogColor(0,0,0),       // RGB値
    fogStart(100.0f),
    fogEnd(500.0f)
{
}

void Dx3DCamera::SetPosition(const VECTOR& pos) {
    position = pos;
}

void Dx3DCamera::SetTarget(const VECTOR& tgt) {
    target = tgt;
}

void Dx3DCamera::SetUpVector(const VECTOR& up) {
    upVector = up;
}

void Dx3DCamera::SetNearFar(float nearPlane, float farPlane) {
    nearZ = nearPlane;
    farZ = farPlane;
}

void Dx3DCamera::SetScreenCenter(float centerX, float centerY) {
    screenCenterX = centerX;
    screenCenterY = centerY;
}

void Dx3DCamera::OrbitAroundTarget(float angleRad, float radius, float height) {
    VECTOR offset;
    offset.x = cosf(angleRad) * radius;
    offset.y = height;
    offset.z = sinf(angleRad) * radius;
    position = VAdd(target, offset);
    upVector = VGet(0, 1, 0); // 必要に応じて上ベクトルも調整
}

void Dx3DCamera::OrbitAroundTargetSpherical(float azimuthRad, float elevationRad, float radius) {
    VECTOR offset;
    offset.x = cosf(elevationRad) * sinf(azimuthRad) * radius;
    offset.y = sinf(elevationRad) * radius;
    offset.z = cosf(elevationRad) * cosf(azimuthRad) * radius;
    position = VAdd(target, offset);
    upVector = VGet(0, 1, 0); // 必要ならより正確に制御（例：方向ベクトルのクロス積）
}

void Dx3DCamera::SetCamera(const VECTOR& pos, const VECTOR& tgt, const VECTOR& up) {
    position = pos;
    target = tgt;
    upVector = up;
}

void Dx3DCamera::SetFogEnable(bool enable) {
    fogEnabled = enable;
}

void Dx3DCamera::SetFogColor(int r, int g, int b) {
    fogColor = Color(r, g, b);
}

void Dx3DCamera::SetFogRange(float start, float end) {
    fogStart = start;
    fogEnd = end;
}

void Dx3DCamera::Apply() {
    SetCameraNearFar(nearZ, farZ);
    SetCameraScreenCenter(screenCenterX, screenCenterY);
    SetCameraPositionAndTargetAndUpVec(position, target, upVector);

    // Fog 設定
    DxLib::SetFogEnable(fogEnabled ? TRUE : FALSE);
    if (fogEnabled) {
        DxLib::SetFogColor(fogColor.R255(), fogColor.G255(), fogColor.B255());
        //DxLib::SetBackgroundColor(fogColor.R255(), fogColor.G255(), fogColor.B255());
        DxLib::SetFogStartEnd(fogStart, fogEnd);
    }
}

void Dx3DCamera::Reset() {
    position = VGet(0.0f, 0.0f, -100.0f);
    target = VGet(0.0f, 0.0f, 0.0f);
    upVector = VGet(0.0f, 1.0f, 0.0f);
    nearZ = 1.0f;
    farZ = 10000.f;
    screenCenterX = 0;
    screenCenterY = 0;

    // Fogリセット
    fogEnabled = false;
    fogColor = Color(0, 0, 0);
    fogStart = 100.0f;
    fogEnd = 500.0f;
}
