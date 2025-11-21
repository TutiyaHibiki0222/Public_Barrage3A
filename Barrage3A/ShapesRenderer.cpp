/*
    ◆ ShapesRenderer.cpp

    クラス名        : ShapesRenderer クラス
    作成日          : 2025/06/26 (木) 14:16:48
    最終変更日      : 2025/06/26 (木) 22:18:05
	作成者          : 𡈽屋 響
    概要            : 図形を描画するComponent
*/
#include "ShapesRenderer.h"
#include "RendererManager.h"
#include "GameObject.h"
#include "GameObjectMgr.h"

// コンストラクタ 
ShapesRenderer::ShapesRenderer() : AppBase("ShapesRenderer"){

}
ShapesRenderer::ShapesRenderer(std::shared_ptr<GameObject> owner) : AppBase("ShapesRenderer", owner) {

}

// AppBase Event.

void ShapesRenderer::Awake() {
	RendererManager::GetInstance().AddRenderer(shared_from_this());
}

void ShapesRenderer::OnDestroy() {
	RendererManager::GetInstance().Remove(shared_from_this());
}

bool ShapesRenderer::IsDraw() {
	return gameObject->IsActive() && enabled;
}

void ShapesRenderer::Draw() {
    if (!IsDraw()) return;

    auto tr = transform;
    Vector2D pos = tr->GetWorldPosition();
    Vector2D scale = tr->GetWorldScale();
    float angle = Mathf::DegToRad(tr->GetWorldRotation());

    // フィルタがあるならフィルタ内で描画処理を丸ごと管理
    if (filter) {
        filter->FilterCast([&]() { DrawShape(pos, scale, angle);});
    }
    else {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, color.A255());
        DrawShape(pos, scale, angle);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
    }
}

void ShapesRenderer::DrawShape(const Vector2D& pos, const Vector2D& scale, float angle) {

    switch (shapeType) {
    case ShapeType::Circle: 
        {
            DrawCircleAA(pos.x, pos.y, size / 2, Mathf::Round<int>(size / 2), color.ToPackedRGB(), TRUE);
        }
        break;
    case ShapeType::Rectangle:
        {
            SetDrawBright(color.R255(), color.G255(), color.B255());
            if (data >= 0) {
                int x = 0, y = 0;
                GetGraphSize(data, &x, &y);

                float scaleX = scale.x * size / static_cast<float>(x);
                float scaleY = scale.y * size / static_cast<float>(y);

                float centerX = x / 2.0f;
                float centerY = y / 2.0f;

                DrawRotaGraph3F(pos.x, pos.y, centerX, centerY, scaleX, scaleY, angle, data, TRUE);
            }
            SetDrawBright(255, 255, 255);
        }
        break;

    default:
        break;
    }
}

void ShapesRenderer::SetShapeType(ShapeType type) {
	if (shapeType == type) return; // 変更なしの場合は何もしない
    shapeType = type;
    if (data >= 0) {
	    DeleteMask(data); // 既存のデータを削除
		data = -1;        // データを無効化
    }
    // 型を作成する.
    switch (shapeType) {
        case ShapeType::Circle: {} break;
        case ShapeType::Rectangle: {
            int mode = GetDrawScreen();
            data = MakeScreen(Mathf::Round<int>(size), Mathf::Round<int>(size), true);
            SetDrawScreen(data);
            DrawBox(0,0, Mathf::Round<int>(size), Mathf::Round<int>(size),0xFFFFFF,true);
			SetDrawScreen(mode); // 元の描画先に戻す
		} 
        break;
        default: {
            // 未対応の型は何もしない
            return;
		}
    }
}



RectF ShapesRenderer::GetAABB() const {
    // ワールド変換取得
    Vector2D pos = transform->GetWorldPosition();
    Vector2D scale = transform->GetWorldScale();
    float rotDeg = transform->GetWorldRotation();
    float w = (float)size;
    float h = (float)size;

    // SIMDレジスタに詰める（幅・高さ・スケール）
    __m128 vec_wh = _mm_set_ps(0, 0, h, w);         // (w, h, 0, 0)
    __m128 vec_scale = _mm_set_ps(0, 0, scale.x, scale.y);     // (sx, sy, 0, 0)
    __m128 size_scaled = _mm_mul_ps(vec_wh, vec_scale); // スケーリングされたサイズ

    float sw = 0, sh = 0;
    _mm_store_ss(&sw, size_scaled);           // sw = w * sx
    _mm_store_ss(&sh, _mm_shuffle_ps(size_scaled, size_scaled, _MM_SHUFFLE(1, 1, 1, 1))); // sh = h * sy

    // 角度をラジアンに変換
    float angleRad = rotDeg * (3.14159265f / 180.0f);
    float cosA = fabsf(cosf(angleRad));
    float sinA = fabsf(sinf(angleRad));

    float hw = sw * 0.5f;
    float hh = sh * 0.5f;

    // 回転後のAABB幅と高さ
    float aabbW = cosA * hw + sinA * hh;
    float aabbH = sinA * hw + cosA * hh;

    // 結果を返す
    return RectF{
        pos.x - aabbW,
        pos.y - aabbH,
        aabbW * 2,
        aabbH * 2
    };
}
int ShapesRenderer::GetSortingOrder() const {
    return sortingLayer.GetSortingOrder(); // 描画順序は常に最大値.
}
