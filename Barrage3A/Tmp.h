#pragma once
#include "StageUnit.h"
#include "StageObject.h"
#include "AbstractBackground.hpp"
#include <list>
#include <memory>
#include <DxLib.h>

class Tmp : public AbstractBackground
{
private:
    std::list<std::shared_ptr<StageUnit>> backgrounds;
    std::list<std::shared_ptr<StageUnit>> waters;
    std::list<std::shared_ptr<StageUnit>> grounds;
    std::list<std::shared_ptr<StageUnit>> tops;

    int frame = 0;
    FogScript script{};

    int fallPhaseFrame = 0;  // 地面とトップ用の内部タイマー
    float moveSpeedGround = -0.5f;
    float moveSpeedTop = -0.5f;

public:
    ~Tmp();

    void Init() override;
    void Update() override;
    void Draw() override;
  
};
