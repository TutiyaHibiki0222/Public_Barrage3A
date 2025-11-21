/*
	SpriteRenderer class
*/
#include "SpriteRenderer.h"
#include "GameObjectMgr.h"
#include "GameObject.h"
#include "RendererManager.h"
// コンストラクタ 
SpriteRenderer::SpriteRenderer() : AppBase("SpriteRenderer"), sprite(), sortingLayer(){

}
SpriteRenderer::SpriteRenderer(std::shared_ptr<GameObject> owner) : AppBase("SpriteRenderer", owner), sprite(),sortingLayer() {

}

// AppBase Event.
void SpriteRenderer::Awake()
{
    RendererManager::GetInstance().AddRenderer(shared_from_this());
}
void SpriteRenderer::OnDestroy()
{
    RendererManager::GetInstance().Remove(shared_from_this());
}
// IRendererDraw Event.

bool SpriteRenderer::IsDraw() {
    return gameObject->IsActive() && enabled;
}

void SpriteRenderer::Draw() {
    if (!sprite || sprite->spriteData == -1 || !gameObject) return;

    auto tr = transform;
    int centerX = Mathf::Round<int>(sprite->width * anchor.x);
    int centerY = Mathf::Round<int>(sprite->height * anchor.y);

    // --- 残像描画 ---
    if (afterImageEnabled) {
        float currentTime = Time.time;
        for (auto it = afterImages.begin(); it != afterImages.end();) {
            float elapsed = currentTime - it->spawnTime;  // ← ここを追加してください

            auto col = it->baseColor; // 0〜255

            float t = elapsed / afterImageLife;
            if (t >= 1.0f) {
                it = afterImages.erase(it);
                continue;
            }

            int alpha = static_cast<int>(col.A255() * (1.0f - t));
            alpha = std::clamp(alpha, 0, 255);

            SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
            SetDrawBright(col.R255(), col.G255(), col.B255());

            DrawRotaGraphFast3(
                static_cast<int>(it->position.x), static_cast<int>(it->position.y),
                centerX, centerY,
                1.0f, 1.0f,
                -Mathf::DegToRad(it->rotation + rotation),
                sprite->spriteData,
                TRUE,
                flipX,
                flipY
            );
            ++it;  // これも忘れずに
        }
        SetDrawBright(255, 255, 255);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }


    // --- 残像を追加（Draw内） ---
    if (afterImageEnabled) {
        float now = Time.time;
        if (now - lastAfterImageAddTime >= afterImageInterval) {
            lastAfterImageAddTime = now;

            AfterImage img;
            img.position = tr->GetWorldPosition();
            img.rotation = tr->GetWorldRotation();
            img.baseColor = color;
            img.spawnTime = now;
            afterImages.push_back(img);

            if (afterImages.size() > maxAfterImages) {
                afterImages.pop_front();
            }
        }
    }

    // --- 通常描画 ---
    Vector2D pos = tr->GetWorldPosition();
    Vector2D scale = tr->GetWorldScale();
    float angle = -Mathf::DegToRad(tr->GetWorldRotation() + rotation);

    float cosA = cosf(tr->GetWorldRotation() * (3.14159265f / 180.f));
    float sinA = sinf(tr->GetWorldRotation() * (3.14159265f / 180.f));
    Vector2D rotatedOffset = {
        offset.x * cosA - offset.y * sinA,
        offset.x * sinA + offset.y * cosA
    };
    pos += rotatedOffset;

    SetDrawBright(color.R255(), color.G255(), color.B255());
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, color.A255());

    DrawRotaGraphFast3(
        static_cast<int>(pos.x), static_cast<int>(pos.y),
        centerX, centerY,
        scale.x, scale.y,
        angle,
        sprite->spriteData,
        TRUE,
        flipX,
        flipY
    );

    SetDrawBright(255, 255, 255);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

#include <xmmintrin.h>
RectF SpriteRenderer::GetAABB() const {
    if (!sprite) return RectF{ 0, 0, 0, 0 };
    Vector2D pos = transform->GetWorldPosition();

    // offsetもワールド座標に回転考慮して変換して加算
    float rotDeg = transform->GetWorldRotation();
    float cosA = cosf(rotDeg * (3.14159265f / 180.0f));
    float sinA = sinf(rotDeg * (3.14159265f / 180.0f));
    Vector2D rotatedOffset = {
        offset.x * cosA - offset.y * sinA,
        offset.x * sinA + offset.y * cosA
    };
    pos += rotatedOffset;

    Vector2D scale = transform->GetWorldScale();
    float w = (float)sprite->width;
    float h = (float)sprite->height;

    // SIMD計算は省略（元コード通り）
    __m128 vec_wh = _mm_set_ps(0, 0, h, w);
    __m128 vec_scale = _mm_set_ps(0, 0, scale.x, scale.y);
    __m128 size_scaled = _mm_mul_ps(vec_wh, vec_scale);

    float sw = 0, sh = 0;
    _mm_store_ss(&sw, size_scaled);
    _mm_store_ss(&sh, _mm_shuffle_ps(size_scaled, size_scaled, _MM_SHUFFLE(1, 1, 1, 1)));

    float angleRad = rotDeg * (3.14159265f / 180.0f);
    float absCosA = fabsf(cosf(angleRad));
    float absSinA = fabsf(sinf(angleRad));

    float hw = sw * 0.5f;
    float hh = sh * 0.5f;

    float aabbW = absCosA * hw + absSinA * hh;
    float aabbH = absSinA * hw + absCosA * hh;

    return RectF{
        pos.x - aabbW,
        pos.y - aabbH,
        aabbW * 2,
        aabbH * 2
    };
}



Vector2DInt SpriteRenderer::GetSize() {
	if (sprite)
        return Vector2DInt(sprite->width, sprite->height);
    return Vector2DInt(0, 0);
}

void SpriteRenderer::SetLayer(int _layer) {
    sortingLayer.layer = _layer;
}

int SpriteRenderer::GetLayer() {
    return sortingLayer.layer;
}

void SpriteRenderer::FlipX(bool is) {
    flipX = is;
}

void SpriteRenderer::FlipY(bool is) {
    flipY = is;
}