/*
	RectTransform2D class
*/
#include "RectTransform2D.h"
#include "GameObject.h"

// コンストラクタ 
RectTransform2D::RectTransform2D() : Transform2D(),anchoredPosition( 0 , 0), anchorMin(0.5f, 0.5f), anchorMax(0.5f, 0.5f), pivot(0.5f, 0.5f), width(100.0f), height(100.0f) {
    this->SetName("RectTransform2D");
}
RectTransform2D::RectTransform2D(std::shared_ptr<GameObject> owner) : Transform2D(owner), anchoredPosition(0, 0), anchorMin(0.5f, 0.5f), anchorMax(0.5f, 0.5f), pivot(0.5f, 0.5f), width(100.0f), height(100.0f)  {
    this->SetName("RectTransform2D");
}

void RectTransform2D::UpdateRectTransform(Vector2D parentSize) {
    Vector2D anchorPosMin = anchorMin * parentSize;
    Vector2D anchorPosMax = anchorMax * parentSize;

    Vector2D baseSize = anchorPosMax - anchorPosMin;
    Vector2D size = baseSize + Vector2D(width,height);

    Vector2D pivotOffset = size * pivot;

    Vector2D pos = anchorPosMin + position - pivotOffset;

    // Transform2Dのpositionに反映
    this->position = pos;

    // width, heightを使う場合は以下のように設定
    this->width = size.x;
    this->height = size.y;
}

Vector2D RectTransform2D::CalculateSizeBasedOnStretch() const {
    if (auto p = parent.lock()) {
        auto parentRect = std::dynamic_pointer_cast<RectTransform2D>(p);
        if (!parentRect) return Vector2D(width, height);

        Vector2D parentSize(parentRect->width, parentRect->height);
        Vector2D anchorRange = anchorMax - anchorMin;

        // Stretch（anchorMin ≠ anchorMax）の場合のみ、親サイズを使う
        Vector2D baseSize = (anchorRange.x != 0.0f || anchorRange.y != 0.0f)
            ? anchorRange * parentSize
            : Vector2D(0, 0);

        // sizeDelta を加算（Unity の仕様と一致）
        return baseSize + Vector2D(width, height);
    }
    return Vector2D(width, height);
}


Vector2D RectTransform2D::GetWorldPosition() const {
    if (auto p = parent.lock()) {
        auto parentRect = std::dynamic_pointer_cast<RectTransform2D>(p);
        if (!parentRect) return Transform2D::GetWorldPosition();

        Vector2D parentPos = parentRect->GetWorldPosition();  // 中心
        Vector2D parentSize(parentRect->width, parentRect->height);

        Vector2D pivotAdjusted = Vector2D(pivot.x, 1.0f - pivot.y);

        Vector2D parentTopLeft = Vector2D(
            parentPos.x - parentSize.x * parentRect->pivot.x,
            parentPos.y - parentSize.y * (1.0f - parentRect->pivot.y)
        );

        Vector2D flippedAnchorMin = Vector2D(anchorMin.x, 1.0f - anchorMin.y);
        Vector2D flippedAnchorMax = Vector2D(anchorMax.x, 1.0f - anchorMax.y);

        Vector2D anchorPosMin = parentTopLeft + parentSize * flippedAnchorMin;
        Vector2D anchorPosMax = parentTopLeft + parentSize * flippedAnchorMax;
        Vector2D anchorRegionSize = anchorPosMax - anchorPosMin;

        Vector2D pivotOffset;
        if (anchorRegionSize.x == 0 && anchorRegionSize.y == 0) {
            pivotOffset = Vector2D(width, height) * anchorRegionSize;
        }
        else {
            // stretchアンカー時はアンカー範囲に対してピボット補正
            pivotOffset = anchorRegionSize * pivotAdjusted;
        }

         //anchoredPosition のY反転は座標系に応じて必要なら行う
        Vector2D correctedAnchoredPos = anchoredPosition;
        correctedAnchoredPos.y = -correctedAnchoredPos.y;


        Vector2D finalPos = anchorPosMin + pivotOffset + correctedAnchoredPos;
        return finalPos;
    }

    // 親なし → anchoredPositionそのまま
    return anchoredPosition;
}

Quad2D RectTransform2D::GetWorldQuad() const {
   
    Vector2D center= GetWorldPosition();
    Vector2D size  = CalculateSizeBasedOnStretch();

    // ピボット基準で四隅の相対位置を計算
    Vector2D offsetTopLeft = Vector2D(-size.x * pivot.x, -size.y * (1.0f - pivot.y));
    Vector2D offsetTopRight = offsetTopLeft + Vector2D(size.x, 0);
    Vector2D offsetBottomLeft = offsetTopLeft + Vector2D(0, size.y);
    Vector2D offsetBottomRight = offsetTopLeft + size;

    float rad = Mathf::DegToRad(rotation);
    auto RotateOffset = [&](const Vector2D& offset) {
        float s = Mathf::Sin(rad), c = Mathf::Cos(rad);
        return Vector2D(offset.x * c - offset.y * s, offset.x * s + offset.y * c);
        };

    Quad2D data {
        center + RotateOffset(offsetTopLeft),
        center + RotateOffset(offsetTopRight),
        center + RotateOffset(offsetBottomRight),
        center + RotateOffset(offsetBottomLeft),
    };
    return data;
}

Vector2D RectTransform2D::GetSize() {
    return Vector2D(width, height);
}

void RectTransform2D::SetSize(const Vector2D& size) {
    width  = size.x;
    height = size.y;
}

void RectTransform2D::SetSize(float w, float h) {
    width  = w;
    height = h;
}