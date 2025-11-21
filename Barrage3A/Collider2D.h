#pragma once
#include "headers.h"
#include "AppBase.h"
#include "Transform2D.h"
#include "QuadTree.h"
#include "IDraw.h"

struct OBB {
    Vector2D center;
    Vector2D halfSize;
    Vector2D axes[2];
};

// Collider2D class and AppBase class
class Collider2D : public AppBase, public IRendererDraw ,public std::enable_shared_from_this<Collider2D> {
private:    // 非公開.
public:     // 公開.
    // コンストラクタ.
    Collider2D();
    Collider2D(std::shared_ptr<GameObject>);

    void Awake()        override;
    void OnDestroy()    override;

    bool IsDraw()       override;
    int  GetSortingOrder() const override;

    virtual MyRectangle GetBounds() const = 0;
};

class BoxCollider : public Collider2D{
private:
    Vector2D size;     // コライダーのサイズ（幅、高さ）
public:

    BoxCollider() : Collider2D(), size() {}
    
    // コンストラクタ
    BoxCollider(std::shared_ptr<GameObject> obj)
        : Collider2D(obj), size() {
    }

    // サイズの取得
    Vector2D GetSize() const    { return size; }
    void SetSize(Vector2D set)  { size = set; }

    RectF GetAABB()const   override;
    OBB   GetOBB() const;

    void Draw()             override;

    MyRectangle GetBounds() const override;
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<BoxCollider>(*this);
    }
};

class CircleCollider : public Collider2D {
private:
    float radius; // 円の半径

public:
    CircleCollider(): Collider2D(), radius(0) {}

    CircleCollider(std::shared_ptr<GameObject> obj)
        : Collider2D(obj), radius(0) {
    }

    float GetRadius() const { return radius; }
    void SetRadius(float r) { radius = r; }

    RectF GetAABB() const override;
    void Draw() override;

    MyRectangle GetBounds() const override;

protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<CircleCollider>(*this);
    }
};

//class ArcCollider : public Collider2D{
//private:
//    float radius; // 円弧の半径
//    float startAngle; // 開始角度
//    float endAngle; // 終了角度
//
//public:
//    ArcCollider(GameObject* obj)
//        : Collider2D(obj), radius(0), startAngle(0), endAngle(0) {
//    }
//
//    bool CheckCollision(std::shared_ptr<Collider2D> other) override;
//
//    void Draw();
//};