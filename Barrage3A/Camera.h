#pragma once
#include "headers.h"
#include "AppBase.h"

// Camera class and AppBase class
class Camera : public AppBase {
private:    // 非公開.
public:     // 公開.

    // コンストラクタ.
    Camera();
    Camera(std::shared_ptr<GameObject>);

    // Event.
    void OnDestroy() override;
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Camera>(*this);
    }
};