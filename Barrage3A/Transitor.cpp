/*
	◆ Transitor.cpp

	クラス名	: Transitor (基礎クラス)
	作成日      : 2025/06/16 (月) 18:48:00
	最終更新日  : 2025/06/16 (月) 19:08:00
	作成者		: 𡈽屋 響
	概要		: トランジスタの基礎クラス
*/
#include "headers.h"
#include "Transitor.h"

Transitor::~Transitor() {
	if (oldRt != -1) {
		DeleteGraph(oldRt);
		oldRt = -1;
	}
	if (newRt != -1) {
		DeleteGraph(newRt);
		newRt = -1;
	}
}

bool Transitor::IsStart() const {
	return isStart;
}

void Transitor::Start() {
	// windowの最大サイズを取得.
	const auto& size = Window::GetInstance().GetMaxVector2D();
	// 画面サイズに合わせてレンダリングターゲットを作成.
	oldRt = MakeScreen((int)size.x, (int)size.y);
	newRt = MakeScreen((int)size.x, (int)size.y);
	// 画面をレンダリングターゲットに切り替え.
	GetDrawScreenGraph(0, 0, (int)size.x, (int)size.y, oldRt);
	frame	= 0;
	isStart = true;
}

void Transitor::Update() {

	if (frame < interval) {
		++frame;
		SetDrawScreen(newRt);
	}
	else if (frame == interval) {
		SetDrawScreen(DX_SCREEN_BACK);
	}
}

bool Transitor::IsEnd() const {
	return frame >= interval;
}