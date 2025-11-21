#pragma once

#include "Transitor.h"

class PushTransitor : public Transitor
{
private:
	TransitDirection direction;//ƒvƒbƒVƒ…•ûŒü
public:
	PushTransitor(int interval = 60, TransitDirection dir = TransitDirection::UP);
	virtual void Update() override;
	virtual void Draw() override;
};