#include "CollisionDispatcher.h"
#include "GameObject.h"
#include "Transform2D.h"
#include <emmintrin.h>

#if defined(_M_X64) || defined(__x86_64__) || defined(__SSE2__) || defined(__AVX__)
#define USE_SIMD
#endif

bool CollisionDispatcher::CheckCollision(std::shared_ptr<Collider2D> a, std::shared_ptr<Collider2D> b) {
    // BoxCollider - CircleCollider
    if (auto boxA = std::dynamic_pointer_cast<BoxCollider>(a)) {
        if (auto boxB = std::dynamic_pointer_cast<BoxCollider>(b)) {
            // Box同士の判定
            return CheckBoxBox(boxA, boxB);
        }
        if (auto circleB = std::dynamic_pointer_cast<CircleCollider>(b)) {
            // Box-Circle判定
            return CheckBoxCircle(boxA, circleB);
        }
    }
    // CircleCollider - BoxCollider
    if (auto circleA = std::dynamic_pointer_cast<CircleCollider>(a)) {
        if (auto boxB = std::dynamic_pointer_cast<BoxCollider>(b)) {
            // Circle-Box判定は Box-Circleと同じなので、逆順で呼ぶ
            return CheckBoxCircle(boxB, circleA);
        }
        if (auto circleB = std::dynamic_pointer_cast<CircleCollider>(b)) {
            return CheckCircleCircle(circleA, circleB);
        }
    }

    return false;
}

bool CollisionDispatcher::CheckBoxBox(std::shared_ptr<BoxCollider> a, std::shared_ptr<BoxCollider> b) {
    OBB obbA = a->GetOBB();
    OBB obbB = b->GetOBB();

    // 衝突判定に使う4軸（2D）
    const Vector2D axes[] = {
        obbA.axes[0],
        obbA.axes[1],
        obbB.axes[0],
        obbB.axes[1],
    };

    // 各軸で Separating Axis 判定
    for (const auto& axis : axes) {
        float minA, maxA;
        ProjectOBB(obbA, axis, minA, maxA);

        float minB, maxB;
        ProjectOBB(obbB, axis, minB, maxB);

        // オーバーラップしていない場合.
        if (maxA < minB || maxB < minA) {
            return false;
        }
    }

    // どの軸でも Separating Axis が見つからなかった → 衝突
    return true;
}

void CollisionDispatcher::ProjectOBB(const OBB& obb, const Vector2D& axis, float& min, float& max) {
#ifdef  USE_SIMD
    // 4頂点を生成
    Vector2D corners[4];
    corners[0] = obb.center + obb.axes[0] * obb.halfSize.x + obb.axes[1] * obb.halfSize.y;
    corners[1] = obb.center - obb.axes[0] * obb.halfSize.x + obb.axes[1] * obb.halfSize.y;
    corners[2] = obb.center - obb.axes[0] * obb.halfSize.x - obb.axes[1] * obb.halfSize.y;
    corners[3] = obb.center + obb.axes[0] * obb.halfSize.x - obb.axes[1] * obb.halfSize.y;

    // SSE2: 4頂点のx, yを一括ロード
    __m128 x = _mm_set_ps(corners[3].x, corners[2].x, corners[1].x, corners[0].x);
    __m128 y = _mm_set_ps(corners[3].y, corners[2].y, corners[1].y, corners[0].y);

    // 軸をブロードキャスト
    __m128 axisX = _mm_set1_ps(axis.x);
    __m128 axisY = _mm_set1_ps(axis.y);

    // 内積計算（x * axis.x + y * axis.y）
    __m128 mulX = _mm_mul_ps(x, axisX);
    __m128 mulY = _mm_mul_ps(y, axisY);
    __m128 projection = _mm_add_ps(mulX, mulY);

    // min/max を計算
    __m128 tempMin  = _mm_min_ps(projection, _mm_shuffle_ps(projection, projection, _MM_SHUFFLE(2, 3, 0, 1)));
    __m128 finalMin = _mm_min_ps(tempMin, _mm_shuffle_ps(tempMin, tempMin, _MM_SHUFFLE(1, 0, 3, 2)));

    __m128 tempMax  = _mm_max_ps(projection, _mm_shuffle_ps(projection, projection, _MM_SHUFFLE(2, 3, 0, 1)));
    __m128 finalMax = _mm_max_ps(tempMax, _mm_shuffle_ps(tempMax, tempMax, _MM_SHUFFLE(1, 0, 3, 2)));

    // 結果をスカラに戻す
    min = _mm_cvtss_f32(finalMin);
    max = _mm_cvtss_f32(finalMax);
#else
    Vector2D corners[4];
    corners[0] = obb.center + obb.axes[0] * obb.halfSize.x + obb.axes[1] * obb.halfSize.y;
    corners[1] = obb.center - obb.axes[0] * obb.halfSize.x + obb.axes[1] * obb.halfSize.y;
    corners[2] = obb.center - obb.axes[0] * obb.halfSize.x - obb.axes[1] * obb.halfSize.y;
    corners[3] = obb.center + obb.axes[0] * obb.halfSize.x - obb.axes[1] * obb.halfSize.y;

    min = max = corners[0].Dot(axis);
    for (int i = 1; i < 4; ++i) {
        float proj = corners[i].Dot(axis);
        if (proj < min) min = proj;
        if (proj > max) max = proj;
    }
#endif
}

bool CollisionDispatcher::CheckCircleCircle(std::shared_ptr<CircleCollider> a, std::shared_ptr<CircleCollider> b) {
    Vector2D centerA = a->GetGameObject()->transform->GetWorldPosition();
    Vector2D centerB = b->GetGameObject()->transform->GetWorldPosition();

    Vector2D scaleA = a->GetGameObject()->transform->GetWorldScale();
    Vector2D scaleB = b->GetGameObject()->transform->GetWorldScale();

    float uniformScaleA = Mathf::Min(scaleA.x, scaleA.y);
    float uniformScaleB = Mathf::Min(scaleB.x, scaleB.y);

    float radiusA = a->GetRadius() * uniformScaleA;
    float radiusB = b->GetRadius() * uniformScaleB;

    float distSq = (centerA - centerB).LengthSquared();
    float radiusSum = radiusA + radiusB;

    return distSq <= radiusSum * radiusSum;
}

bool CollisionDispatcher::CheckBoxCircle(std::shared_ptr<BoxCollider> box, std::shared_ptr<CircleCollider> circle) {
    OBB obb                 = box->GetOBB();
    Vector2D circleCenter   = circle->GetGameObject()->transform->GetWorldPosition();
    Vector2D scale          = circle->GetGameObject()->transform->GetWorldScale();
    float uniformScale      = Mathf::Min(scale.x, scale.y);
    float circleRadius      = circle->GetRadius() * uniformScale;

    // 円の中心をOBBの局所座標系に変換
    Vector2D dir = circleCenter - obb.center;

    // OBBの軸に投影（内積）
    float localX = dir.Dot(obb.axes[0]);
    float localY = dir.Dot(obb.axes[1]);

    // OBBの境界内にクランプ
    float clampedX = Mathf::Clamp(localX, -obb.halfSize.x, obb.halfSize.x);
    float clampedY = Mathf::Clamp(localY, -obb.halfSize.y, obb.halfSize.y);

    // 最近点を局所座標系からワールドに戻す
    Vector2D closestPoint = obb.center + obb.axes[0] * clampedX + obb.axes[1] * clampedY;

    // 円の中心と最近点の距離の二乗
    float distSq = (circleCenter - closestPoint).LengthSquared();

    return distSq <= circleRadius * circleRadius;
}
