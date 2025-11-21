#pragma once

#include "Transitor.h"

class WipeTransitor : public Transitor
{
private:
	int gradationH;//グラデーション用ハンドル
public:
	WipeTransitor(int interval = 60 , TransitDirection dir = TransitDirection::LEFT);
	virtual void Draw() override;
};