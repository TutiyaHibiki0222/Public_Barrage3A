#pragma once

#include "headers.h"
#include "AppBase.h"
#include "IDraw.h"
#include "Texture2D.h"
#include <deque>

using namespace GameEngine;

// SpriteRenderer class and AppBase class
class SpriteRenderer : public AppBase, public IRendererDraw, public std::enable_shared_from_this<SpriteRenderer> {
private:    // 非公開.
    // 残像用構造体
    struct AfterImage {
        Vector2D position{};
        float rotation = 0.f;
        Color baseColor{};
        float spawnTime= 0.f;
    };

    std::shared_ptr<Sprite> sprite;
    SortingLayer sortingLayer;
    float rotation = 0.0f;
    Color color    = Color(0xFFFFFF);
    Vector2D anchor = { 0.5f, 0.5f };
	Vector2D offset = { 0.0f, 0.0f }; // オフセット位置
    bool flipX = false;
    bool flipY = false;

    bool afterImageEnabled = false;
    std::deque<AfterImage> afterImages;
    float afterImageInterval = 0.01f;
    float afterImageLife = 0.15f;
    float afterImageTimer = 0.0f;
    int   maxAfterImages = 16;
    float lastAfterImageAddTime = 0.0f;
public:     // 公開.

    // コンストラクタ.
    SpriteRenderer();
    SpriteRenderer(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Awake()    override;
    void OnDestroy()override;
    // IRendererDraw 関数.
    bool IsDraw()   override;
    void Draw()     override;
    RectF GetAABB() const override;

    void SetSprite(const Sprite& s) { sprite = std::make_shared<Sprite>(s); }
	void SetSprite(const std::shared_ptr<Sprite>& s) { sprite = s; }
    std::shared_ptr<Sprite> GetSprite() const { return sprite; }

    void SetAnchor(const Vector2D& a) { anchor = a; }
    Vector2D GetAnchor() const { return anchor; }

	void SetOffset(const Vector2D& o) { offset = o; }
	Vector2D GetOffset() const { return offset; }

    void SetLayer(int);
    int  GetLayer();
    Vector2DInt GetSize();

    int GetSortingOrder() const override { return sortingLayer.GetSortingOrder(); }

    SortingLayer& GetSortingLayer() { return sortingLayer; }
    Color& GetColor()              { return color; }
	void SetColor(const Color& c) { color = c; }
    void  SetRotation(float r) { rotation = r; }
    float GetRotation() const  { return rotation; }

    void FlipX(bool);
    void FlipY(bool);

    void SetAfterImageEnabled(bool enable) {
        afterImageEnabled = enable;
        if (!enable) afterImages.clear();
    }
    bool IsAfterImageEnabled() const { return afterImageEnabled; }
    void SetAfterImageSettings(float interval, float life, int maxCount) {
        afterImageInterval = interval;
        afterImageLife = life;
        maxAfterImages = maxCount;
    }

protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<SpriteRenderer>(*this);
    }
};