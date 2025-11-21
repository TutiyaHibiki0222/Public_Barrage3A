#pragma once

#include "Transitor.h"

class TileTransitor : public Transitor
{
private:
	int width = 100;
	bool directionRightDown = true;  // true: ç∂è„Å®âEâ∫, false: âEâ∫Å®ç∂è„
public:
	TileTransitor(int interval, int _width = 100,bool _directionRightDown = true);

	virtual void Draw() override;
};