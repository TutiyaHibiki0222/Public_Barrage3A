#pragma once
#include "Scene.h"

class GameScene : public Scene
{
private:
	bool isChangeScene = false; // シーン変更フラグ.


public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};