#pragma once
#include "StageUnit.h"
#include "StageObject.h"
#include "AbstractBackground.hpp"
#include <list>
#include <memory>
#include <DxLib.h>

class Stage3 : public AbstractBackground {
private:
    std::list<std::shared_ptr<StageUnit>> _units;
    int frame = 0;
public:
	~Stage3();
    void Init() override;
    void Update() override;
    void Draw() override;
};
