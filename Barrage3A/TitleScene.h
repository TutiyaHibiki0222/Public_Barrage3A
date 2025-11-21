#pragma once
#include "Scene.h"

// TitleScene .
class TitleScene : public Scene {
private:


public:
	void Init()     override;
	void Update()   override;
	void Draw()     override;
	void Release()  override;
};
