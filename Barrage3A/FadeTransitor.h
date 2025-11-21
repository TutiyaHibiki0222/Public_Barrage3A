/*
	◆ FadeTransitor.h

	クラス名	: FadeTransitor
	作成日      : 2025/06/16 (月) 19:06:00
	最終更新日  : 2025/06/17 (火) 01:36:00
	作成者		: 𡈽屋 響
	概要		: クロスフェード.
*/
#pragma once

#include "Transitor.h"

class FadeTransitor : public Transitor {
public:
	FadeTransitor(int endFrame) : Transitor(endFrame) {}

	void Update() override;
	void Draw() override;
};