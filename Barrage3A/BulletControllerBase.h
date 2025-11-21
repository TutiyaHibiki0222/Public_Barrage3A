#pragma once

#include <memory>

class Transform2D;
class BulletBase;

class BulletControllerBase {
public:
    virtual ~BulletControllerBase() = default;
    virtual void Update(std::shared_ptr<Transform2D> _transform, BulletBase*) = 0;
};
