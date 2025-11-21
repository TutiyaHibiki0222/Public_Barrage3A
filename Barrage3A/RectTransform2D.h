#pragma once
#include "headers.h"
#include "AppBase.h"
#include "Transform2D.h"

#undef min
#undef max

struct Quad2D {
    Vector2D topLeft;       // 左上.
    Vector2D topRight;      // 右上.
    Vector2D bottomLeft;    // 左下.
    Vector2D bottomRight;   // 右下.

    Quad2D FixQuadOrder() const {
        std::vector<Vector2D> points = { topLeft, topRight, bottomLeft, bottomRight };

        // 左上 = y が最小、同じなら x が最小
        auto leftTopIt = std::min_element(points.begin(), points.end(), [](const Vector2D& a, const Vector2D& b) {
            return (a.y < b.y) || (a.y == b.y && a.x < b.x);
            });
        Vector2D leftTop = *leftTopIt;
        points.erase(leftTopIt);

        // 右上 = y が最小、同じなら x が最大
        auto rightTopIt = std::min_element(points.begin(), points.end(), [](const Vector2D& a, const Vector2D& b) {
            return (a.y < b.y) || (a.y == b.y && a.x > b.x);
            });
        Vector2D rightTop = *rightTopIt;
        points.erase(rightTopIt);

        // 左下 = y が最大、同じなら x が最小
        auto leftBottomIt = std::max_element(points.begin(), points.end(), [](const Vector2D& a, const Vector2D& b) {
            return (a.y < b.y) || (a.y == b.y && a.x < b.x);
            });
        Vector2D leftBottom = *leftBottomIt;
        points.erase(leftBottomIt);

        // 残りは右下
        Vector2D rightBottom = points.front();

        return Quad2D{ leftTop, rightTop, leftBottom, rightBottom };
    }

    // AABB を取得
    Quad2D ToAABB() const {
        float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
        float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
        float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
        float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

        return Quad2D{
            Vector2D{minX, minY}, // 左上
            Vector2D{maxX, minY}, // 右上
            Vector2D{minX, maxY}, // 左下
            Vector2D{maxX, maxY}  // 右下
        };
    }

    void Print(std::string name = "") const {
        std::cout << name << "↓\n"
            << "topLeft     : " << topLeft.print() << "↓\n"
            << "topRight    : " << topRight.print() << "↓\n"
            << "bottomLeft  : " << bottomLeft.print() << "↓\n"
            << "bottomRight : " << bottomRight.print() << std::endl;
    }
};

// RectTransform2D class and AppBase class
class RectTransform2D : public Transform2D {
public:     // 公開.
    Vector2D anchoredPosition;

    float width, height;    
    // Anchors
    Vector2D anchorMin, anchorMax;  
    Vector2D pivot; 
public:
    // コンストラクタ.
    RectTransform2D();
    RectTransform2D(std::shared_ptr<GameObject>);

    void UpdateRectTransform(Vector2D parentSize);
    Vector2D CalculateSizeBasedOnStretch() const;
    Vector2D GetWorldPosition() const override;
    Quad2D GetWorldQuad() const;

    Vector2D GetSize();

	void SetSize(const Vector2D& size);
    void SetSize(float w, float h);
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<RectTransform2D>(*this);
    }
};
