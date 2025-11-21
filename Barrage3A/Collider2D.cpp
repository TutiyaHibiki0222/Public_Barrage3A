/*
	Collider2D class
*/
#include "Collider2D.h"
#include "GameObject.h"
#include "ColliderManager.h"
#include "RendererManager.h"

#define DEBUG_COLLIDER_OBJ_DRAW (_DEBUG && false)
// コンストラクタ 
Collider2D::Collider2D() : AppBase("Collider2D"){

}
Collider2D::Collider2D(std::shared_ptr<GameObject> owner) : AppBase("Collider2D", owner) {

}

void Collider2D::Awake() {
    CollisionManager::GetInstance().AddCollider(shared_from_this());
#if DEBUG_COLLIDER_OBJ_DRAW
    RendererManager::GetInstance().AddRenderer(shared_from_this());
#endif
}

void Collider2D::OnDestroy() {
    CollisionManager::GetInstance().RemoveCollider(shared_from_this());
#if DEBUG_COLLIDER_OBJ_DRAW
    RendererManager::GetInstance().Remove(shared_from_this());
#endif
}

bool Collider2D::IsDraw() { return gameObject->IsActive() && enabled; }

int  Collider2D::GetSortingOrder() const { return INT_MAX; }

void BoxCollider::Draw() {
    OBB obb = GetOBB();

    Vector2D center     = obb.center;
    Vector2D halfSize   = obb.halfSize;
    Vector2D right      = obb.axes[0];
    Vector2D up         = obb.axes[1];

    Vector2D vertices[4];
    vertices[0] = center + right * halfSize.x + up * halfSize.y;    // 右上
    vertices[1] = center + right * halfSize.x - up * halfSize.y;    // 右下
    vertices[2] = center - right * halfSize.x - up * halfSize.y;    // 左下
    vertices[3] = center - right * halfSize.x + up * halfSize.y;    // 左上

    for (int i = 0; i < 4; ++i) {
        int next = (i + 1) % 4;
        DrawLineAA(vertices[i].x, vertices[i].y, vertices[next].x, vertices[next].y, 0x00CC00,1);
    }
}

MyRectangle BoxCollider::GetBounds() const {
    Vector2D position = transform->GetWorldPosition();
    return MyRectangle(position.x - size.x / 2, position.y - size.y / 2, size.x, size.y);
}

RectF BoxCollider::GetAABB() const {
    OBB obb = GetOBB();

    // OBBの4頂点を計算
    Vector2D corners[4];
    corners[0] = obb.center + obb.axes[0] * obb.halfSize.x + obb.axes[1] * obb.halfSize.y;
    corners[1] = obb.center + obb.axes[0] * obb.halfSize.x - obb.axes[1] * obb.halfSize.y;
    corners[2] = obb.center - obb.axes[0] * obb.halfSize.x + obb.axes[1] * obb.halfSize.y;
    corners[3] = obb.center - obb.axes[0] * obb.halfSize.x - obb.axes[1] * obb.halfSize.y;

    float minX = corners[0].x;
    float maxX = corners[0].x;
    float minY = corners[0].y;
    float maxY = corners[0].y;

    for (int i = 1; i < 4; ++i) {
        if (corners[i].x < minX) minX = corners[i].x;
        if (corners[i].x > maxX) maxX = corners[i].x;
        if (corners[i].y < minY) minY = corners[i].y;
        if (corners[i].y > maxY) maxY = corners[i].y;
    }

    return RectF(minX, minY, maxX - minX, maxY - minY);
}

OBB BoxCollider::GetOBB() const {
    auto transform = GetGameObject()->transform;

    // ワールド Transform 取得
    Transform2D worldTransform = transform->GetWorldTransform2D();

    Vector2D center     = worldTransform.position;
    Vector2D worldScale = worldTransform.scale;
    float rotation      = worldTransform.rotation;

    // スケールを反映
    Vector2D scaledSize = size * worldScale;
    Vector2D halfSize = scaledSize * 0.5f;

    // 回転反映
    float rad = Mathf::DegToRad(rotation);
    Vector2D right  = Vector2D( Mathf::Cos(rad), Mathf::Sin(rad));
    Vector2D up     = Vector2D(-Mathf::Sin(rad), Mathf::Cos(rad));

    OBB obb;
    obb.center      = center;
    obb.halfSize    = halfSize;
    obb.axes[0]     = right.GetNormalize();
    obb.axes[1]     = up.GetNormalize();

    return obb;
}

void CircleCollider::Draw() {
    Vector2D position = transform->GetWorldPosition();
    DrawCircleAA(position.x, position.y, radius,64, 0x00CC00, false);
}

MyRectangle CircleCollider::GetBounds() const {
    Vector2D position = transform->GetWorldPosition(); // ゲームオブジェクトの位置
    return MyRectangle(position.x - radius, position.y - radius, radius * 2, radius * 2); // AABBを返す
}

RectF CircleCollider::GetAABB() const {
    Vector2D position = transform->GetWorldPosition(); // ゲームオブジェクトの位置
    return RectF(position.x - radius, position.y - radius, radius * 2, radius * 2); // AABBを返す
}