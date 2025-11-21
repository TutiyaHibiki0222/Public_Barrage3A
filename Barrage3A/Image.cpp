/*
	Image class
*/
#include "UiBase.h"
#include "GameObject.h"
#include "Image.h"

// コンストラクタ 
Image::Image() : UiBase(){

}

Image::Image(std::shared_ptr<GameObject> owner) : UiBase(owner) {
	
}

void Image::Awake() {
	raycastTarget = false;
}

void Image::OnUiDraw(const Quad2D& quad, const std::shared_ptr<Canvas>& canvas) {
	auto tex = sprite.lock();
	int oldBlendMode, oldParam;
	GetDrawBlendMode(&oldBlendMode, &oldParam);
	int oldR, oldG, oldB;
	GetDrawBright(&oldR, &oldG, &oldB);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, color.A255());
    SetDrawBright(color.R255(), color.G255(), color.B255());
    if (!tex) {
        DrawSimpleColor(quad);
	}
	else {
        switch (type) {
        case ImageType::Simple:
            DrawSimple(quad, tex);
            break;

        case ImageType::Sliced:
            DrawSliced(quad, tex);
            break;

        case ImageType::Tiled:
            DrawTiled(quad, tex);
            break;

        case ImageType::Filled:
            DrawFilled(quad, tex);
            break;
        }
	}

	SetDrawBright(oldR, oldG, oldB);
	SetDrawBlendMode(oldBlendMode, oldParam);
}

void Image::DrawSimpleColor(const Quad2D& quad) { 
    DrawTriangle((int)quad.topLeft.x, (int)quad.topLeft.y,
        (int)quad.topRight.x, (int)quad.topRight.y,
        (int)quad.bottomLeft.x, (int)quad.bottomLeft.y,
        color.ToPackedRGB(), true);

    DrawTriangle((int)quad.topLeft.x, (int)quad.topLeft.y,
        (int)quad.bottomRight.x, (int)quad.bottomRight.y,
        (int)quad.bottomLeft.x, (int)quad.bottomLeft.y,
        color.ToPackedRGB(), true);
}

void Image::DrawSimple(const Quad2D& quad, const std::shared_ptr<Sprite>& tex) {
    DrawModiGraph(
        (int)quad.topLeft.x, (int)quad.topLeft.y,
        (int)quad.topRight.x, (int)quad.topRight.y,
        (int)quad.bottomLeft.x, (int)quad.bottomLeft.y,
        (int)quad.bottomRight.x, (int)quad.bottomRight.y,
        tex->spriteData, true
    );
}

void Image::DrawTiled(const Quad2D& quad, const std::shared_ptr<Sprite>& tex) {
    auto rt = GetRectTransform();
    if (!rt || !tex) return;

    Vector2D size = rt->GetSize();  // ユニット単位

    int texW = tex->width;
    int texH = tex->height;

    float unitW = texW / pixelsPerUnitMultiplier;
    float unitH = texH / pixelsPerUnitMultiplier;

    int areaX1 = (int)quad.topLeft.x;
    int areaY1 = (int)quad.topLeft.y;
    int areaX2 = (int)(quad.topLeft.x + size.x);
    int areaY2 = (int)(quad.topLeft.y + size.y);

    SetDrawArea(areaX1, areaY1, areaX2, areaY2);

    for (float y = 0; y < size.y; y += unitH) {
        for (float x = 0; x < size.x; x += unitW) {
            // 残りの描画幅と高さ（ユニット単位）
            float remainW = Mathf::Min(unitW, size.x - x);
            float remainH = Mathf::Min(unitH, size.y - y);

            // ピクセル単位でのソース矩形サイズ
            int srcW = (int)(remainW * pixelsPerUnitMultiplier);
            int srcH = (int)(remainH * pixelsPerUnitMultiplier);

            // 描画先矩形
            int drawX1 = Mathf::Round<int>(quad.topLeft.x + x);
            int drawY1 = Mathf::Round<int>(quad.topLeft.y + y);
            int drawX2 = Mathf::Round<int>(drawX1 + remainW);
            int drawY2 = Mathf::Round<int>(drawY1 + remainH);

            DrawRectExtendGraph(
                drawX1, drawY1,
                drawX2, drawY2,
                0, 0, srcW, srcH,
                tex->spriteData, true
            );
        }
    }

    SetDrawAreaFull();
}




void Image::DrawSliced(const Quad2D& quad, const std::shared_ptr<Sprite>& tex) {
    if (!tex) return;

    auto rt = GetRectTransform();
    if (!rt) return;

    Vector2D size = rt->GetSize();
    float ppu = pixelsPerUnitMultiplier;

    // スプライトの border を PPU 補正
    int bl = (int)(tex->borderLeft / ppu);
    int br = (int)(tex->borderRight / ppu);
    int bt = (int)(tex->borderTop / ppu);
    int bb = (int)(tex->borderBottom / ppu);

    int w = (int)size.x;
    int h = (int)size.y;

    // 描画位置計算
    int x0 = (int)quad.topLeft.x;
    int y0 = (int)quad.topLeft.y;
    int x1 = x0 + bl;
    int x2 = x0 + w - br;
    int x3 = x0 + w;

    int y0_ = y0;
    int y1 = y0_ + bt;
    int y2 = y0_ + h - bb;
    int y3 = y0_ + h;

    // 9スライス部分描画 (左上から右下まで順に)
    // 左上
    DrawRectGraph(x0, y0_, 0, 0, tex->borderLeft, tex->borderTop, tex->spriteData, true);
    // 上
    DrawRectExtendGraph(x1, y0_, x2, y1, tex->borderLeft, 0, tex->width - tex->borderLeft - tex->borderRight, tex->borderTop, tex->spriteData, true);
    // 右上
    DrawRectGraph(x2, y0_, tex->width - tex->borderRight, 0, tex->borderRight, tex->borderTop, tex->spriteData, true);
    // 左
    DrawRectExtendGraph(x0, y1, x1, y2, 0, tex->borderTop, tex->borderLeft, tex->height - tex->borderTop - tex->borderBottom, tex->spriteData, true);
    // 中央
    DrawRectExtendGraph(x1, y1, x2, y2, tex->borderLeft, tex->borderTop, tex->width - tex->borderLeft - tex->borderRight, tex->height - tex->borderTop - tex->borderBottom, tex->spriteData, true);
    // 右
    DrawRectExtendGraph(x2, y1, x3, y2, tex->width - tex->borderRight, tex->borderTop, tex->borderRight, tex->height - tex->borderTop - tex->borderBottom, tex->spriteData, true);
    // 左下
    DrawRectGraph(x0, y2, 0, tex->height - tex->borderBottom, tex->borderLeft, tex->borderBottom, tex->spriteData, true);
    // 下
    DrawRectExtendGraph(x1, y2, x2, y3, tex->borderLeft, tex->height - tex->borderBottom, tex->width - tex->borderLeft - tex->borderRight, tex->borderBottom, tex->spriteData, true);
    // 右下
    DrawRectGraph(x2, y2, tex->width - tex->borderRight, tex->height - tex->borderBottom, tex->borderRight, tex->borderBottom, tex->spriteData, true);
}


void Image::DrawFilled(const Quad2D& quad, const std::shared_ptr<Sprite>& tex) {
    auto rt = GetRectTransform();
    if (!rt || !tex) return;
    Vector2D size = rt->GetSize();         // 描画枠サイズ（RectTransformの範囲）
    Vector2D pivot = rt->pivot;
    Vector2D pos = rt->GetWorldPosition();

    // 描画開始座標 = pivot位置 - (描画枠のサイズ × pivot)
    float drawAreaLeft = pos.x - size.x * pivot.x;
    float drawAreaTop = pos.y - size.y * pivot.y;

    // テクスチャの描画サイズは「実画像サイズのまま」
    float texW = static_cast<float>(tex->width);
    float texH = static_cast<float>(tex->height);

    // テクスチャ描画開始座標は、描画枠の左上 + ((枠サイズ - 画像サイズ) / 2) で中央寄せ
    float drawX = drawAreaLeft + (size.x - texW) * pivot.x;
    float drawY = drawAreaTop + (size.y - texH) * pivot.y;

    auto& opts = filledOptions;

   if (opts.method == FillMethod::Horizontal) {
        float fillW = texW * fillAmount;
        float offsetX = (opts.horizontalOrigin == FillOriginHorizontal::Right) ? texW - fillW : 0.0f;

        // 描画矩形の右端は左端 + fillWidth
        DrawRectExtendGraph(
            static_cast<int>(drawX), static_cast<int>(drawY),
            static_cast<int>(drawX + fillW), static_cast<int>(drawY + texH),
            static_cast<int>(offsetX), 0,
            static_cast<int>(fillW), static_cast<int>(texH),
            tex->spriteData,
            TRUE
        );
    }
    else if (opts.method == FillMethod::Vertical) {
        float fillH = texH * fillAmount;
        float offsetY = (opts.verticalOrigin == FillOriginVertical::Top) ? texH - fillH : 0.0f;

        DrawRectExtendGraph(
            static_cast<int>(drawX), static_cast<int>(drawY),
            static_cast<int>(drawX + texW), static_cast<int>(drawY + fillH),
            0, static_cast<int>(offsetY),
            static_cast<int>(texW), static_cast<int>(fillH),
            tex->spriteData,
            TRUE
        );
    }
    else if (opts.method == FillMethod::Radial360) {
        std::cout << "Radial360: 未対応" << std::endl;
    }
}



void Image::SetSprite(const std::shared_ptr<Sprite>& _sprite) {
	sprite = _sprite;
}

void Image::SetImageType(const ImageType _type) {
    type = _type;
}

Color& Image::GetColor() {
    return color;
}

void Image::SetColor(const Color& _color) {
    color = _color;
}