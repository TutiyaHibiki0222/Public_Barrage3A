#pragma once

#include "Transitor.h"

//’Zûƒgƒ‰ƒ“ƒWƒVƒ‡ƒ“(cÈ)
class StripTransitor : public Transitor
{
private:
	int width = 100;
public:
	StripTransitor(int interval, int _width = 100);

	virtual void Draw() override;
};