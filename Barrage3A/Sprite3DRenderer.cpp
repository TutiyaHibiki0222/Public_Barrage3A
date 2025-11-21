/*
    ◆ Sprite3DRenderer.cpp

    クラス名        : Sprite3DRenderer クラス
    作成日          : 2025/07/27 (日) 22:17:23
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "Sprite3DRenderer.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "RendererManager.h"

// コンストラクタ 
Sprite3DRenderer::Sprite3DRenderer() : AppBase("Sprite3DRenderer"){

}
Sprite3DRenderer::Sprite3DRenderer(std::shared_ptr<GameObject> owner) : AppBase("Sprite3DRenderer", owner) {

}

// AppBase Event.
void Sprite3DRenderer::Awake()
{
    RendererManager::GetInstance().AddRenderer(shared_from_this());
}
void Sprite3DRenderer::OnDestroy()
{
    RendererManager::GetInstance().Remove(shared_from_this());
}
// IRendererDraw Event.
bool Sprite3DRenderer::IsDraw() {
    return gameObject->IsActive() && enabled;
}

VECTOR Rotate(const VECTOR& v, float ax, float ay, float az) {
    float rx = ax * DX_PI_F / 180.0f;
    float ry = ay * DX_PI_F / 180.0f;
    float rz = az * DX_PI_F / 180.0f;

    // Z → Y → X の順で回転（Zを常に正面基準に）
    VECTOR a = v, b;
    // Z 回転（スクリーンに対する回転）
    b.x = a.x * cosf(rz) - a.y * sinf(rz);
    b.y = a.x * sinf(rz) + a.y * cosf(rz);
    b.z = a.z;
    // Y 回転（奥行き）
    a = b;
    b.x = a.z * sinf(ry) + a.x * cosf(ry);
    b.y = a.y;
    b.z = a.z * cosf(ry) - a.x * sinf(ry);
    // X 回転（上下傾き）
    a = b;
    b.x = a.x;
    b.y = a.y * cosf(rx) - a.z * sinf(rx);
    b.z = a.y * sinf(rx) + a.z * cosf(rx);
    return b;
}

void Sprite3DRenderer::Draw() {
    if (!sprite || !transform) return;

    auto sp = sprite;
    auto scale = transform->GetWorldScale();
    auto pos = transform->GetWorldPosition();

    float halfX = (sp->width * 0.5f) * scale.x;
    float halfY = (sp->height * 0.5f) * scale.y;

    // 4頂点のローカル座標（中心基準）
    VECTOR local[4] = {
        VGet(-halfX,  halfY, 0),  // 左上
        VGet(halfX,  halfY, 0),  // 右上
        VGet(halfX, -halfY, 0),  // 右下
        VGet(-halfX, -halfY, 0),  // 左下
    };

    float radX = Mathf::DegToRad(rotation.x);
    float radY = Mathf::DegToRad(rotation.y);
    float radZ = Mathf::DegToRad(rotation.z);

    int sx[4], sy[4];
    for (int i = 0; i < 4; ++i) {
        VECTOR r = Rotate(local[i], rotation.x, rotation.y, rotation.z);
        sx[i] = (int)(pos.x + r.x);
        sy[i] = (int)(pos.y - r.y);
    }

    SetDrawBright(color.R255(), color.G255(), color.B255());
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, color.A255());

    DrawModiGraph(
        sx[0], sy[0], sx[1], sy[1],
        sx[2], sy[2], sx[3], sy[3],
        sp->spriteData, TRUE);

    SetDrawBright(255, 255, 255);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

RectF Sprite3DRenderer::GetAABB() const {
    if (!sprite || !transform) return RectF();

    float width = static_cast<float>(sprite->width);
    float height = static_cast<float>(sprite->height);
    Vector2D scale = transform->GetWorldScale();
    Vector2D pos = transform->GetWorldPosition();

    // 中心原点ローカル座標（Z=0）
    VECTOR local[4] = {
        VGet(-width * 0.5f,  height * 0.5f, 0.0f), // 左上
        VGet(width * 0.5f,  height * 0.5f, 0.0f), // 右上
        VGet(width * 0.5f, -height * 0.5f, 0.0f), // 右下
        VGet(-width * 0.5f, -height * 0.5f, 0.0f), // 左下
    };

    // 回転適用
    Vector3D rot = rotation;
    float radX = Mathf::DegToRad(rot.x);
    float radY = Mathf::DegToRad(rot.y);
    float radZ = Mathf::DegToRad(rot.z);

    float minX = FLT_MAX, maxX = -FLT_MAX;
    float minY = FLT_MAX, maxY = -FLT_MAX;

    for (int i = 0; i < 4; ++i) {
        VECTOR p = local[i];

        // スケーリング
        p.x *= scale.x;
        p.y *= scale.y;
        p.z *= 1.0f;

        // 回転（Z→Y→X の順）
        p = Rotate(p, rot.x, rot.y, rot.z);

        // 平行移動（position）
        float worldX = pos.x + p.x;
        float worldY = pos.y - p.y; // Yは画面座標系で反転

        minX = std::min(minX, worldX);
        maxX = std::max(maxX, worldX);
        minY = std::min(minY, worldY);
        maxY = std::max(maxY, worldY);
    }

    return RectF(minX, minY, maxX - minX, maxY - minY);
}


Vector2DInt Sprite3DRenderer::GetSize() {
    if (sprite)
        return Vector2DInt(sprite->width, sprite->height);
    return Vector2DInt(0, 0);
}

void Sprite3DRenderer::SetLayer(int _layer) {
    sortingLayer.layer = _layer;
}

int Sprite3DRenderer::GetLayer() {
    return sortingLayer.layer;
}
