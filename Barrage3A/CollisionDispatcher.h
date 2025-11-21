#pragma once
#include "Collider2D.h"

class CollisionDispatcher {
public:
    static bool CheckCollision(std::shared_ptr<Collider2D> a, std::shared_ptr<Collider2D> b);
private:

    static bool CheckBoxBox(std::shared_ptr<BoxCollider> a, std::shared_ptr<BoxCollider> b);
    static void ProjectOBB(const OBB& obb, const Vector2D& axis, float& min, float& max);
    static bool CheckBoxCircle(std::shared_ptr<BoxCollider> box, std::shared_ptr<CircleCollider> circle);

private:
    static bool CheckCircleCircle(std::shared_ptr<CircleCollider> a, std::shared_ptr<CircleCollider> b);

};

