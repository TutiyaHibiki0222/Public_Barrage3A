/*
	◆ FadeTransitor.cpp

	クラス名	: FadeTransitor
	作成日      : 2025/06/16 (月) 19:06:00
	最終更新日  : 2025/06/17 (火) 01:36:00
	作成者		: 𡈽屋 響
	概要		: クロスフェード.
*/
#include "headers.h"
#include "FadeTransitor.h"

void FadeTransitor::Update()
{
	if (frame < interval) {
		++frame;
		
		(newRt);
	}
	else if (frame == interval) {
		SetDrawScreen(DX_SCREEN_BACK);
	}
}

void FadeTransitor::Draw()
{
	if (IsEnd()) {
		return;
	}
	SetDrawScreen(DX_SCREEN_BACK);
	auto rate = (float)frame / (float)interval;//割合を求める
	DrawGraph(0, 0, oldRt, true);
	//切り替え先画面をアルファブレンディング
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(rate * 255));
	DrawGraph(0, 0, newRt, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}