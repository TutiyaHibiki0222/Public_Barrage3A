
#include "headers.h"
#include "IrisTransitor.h"

IrisTransitor::IrisTransitor(int interval,bool _irisOut) 
	: Transitor(interval),irisOut(_irisOut)
{
	const auto& wsize = Window::GetInstance().GetMaxVector2D();
	handleForMaskScreen = MakeScreen(Mathf::Round<int>(wsize.x),Mathf::Round<int>(wsize.y), true);
	maskH				= CreateMaskScreen();
	diagonalLength		= std::hypotf(wsize.x, wsize.y) / 2.0f;
}

IrisTransitor::~IrisTransitor()
{
	DeleteMaskScreen();
}

void IrisTransitor::Update()
{
	if (frame < interval) {
		++frame;
		SetDrawScreen(newRt);
	}
	else if (frame == interval) {
		SetDrawScreen(DX_SCREEN_BACK);
	}
}

void IrisTransitor::Draw()
{
	if (IsEnd()) {
		return;
	}

	auto rate = (float)frame / (float)interval;
	int backRT = oldRt;
	int maskedRT = newRt;
	if (irisOut) {
		backRT = newRt;
		maskedRT = oldRt;
		rate = 1.0f - rate;
	}
	//
	float radius = (diagonalLength)*rate;
	SetDrawScreen(handleForMaskScreen);
	ClearDrawScreen();
	const auto& wsize = Window::GetInstance().GetMaxVector2D();
	DrawCircleAA(wsize.x / 2 , wsize.y / 2, radius, 32, 0xffffff, true);

	//隠し関数(現在のグラフィックハンドルをマスクスクリーンに転送)
	SetMaskScreenGraph(handleForMaskScreen);
	//描画領域を反転する
	SetMaskReverseEffectFlag(true);

	SetDrawScreen(DX_SCREEN_BACK);
	SetUseMaskScreenFlag(false);
	DrawGraph(0, 0, backRT, true);
	SetUseMaskScreenFlag(true);
	DrawGraph(0, 0, maskedRT, true);
	SetUseMaskScreenFlag(false);
}