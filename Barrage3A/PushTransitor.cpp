
#include "headers.h"
#include "PushTransitor.h"

PushTransitor::PushTransitor(int interval, TransitDirection _direction)
	: Transitor(interval), direction(_direction)
{

}

void PushTransitor::Update()
{
	if (frame < interval) {
		++frame;
		SetDrawScreen(newRt);
	}
	else if (frame == interval) {
		SetDrawScreen(DX_SCREEN_BACK);
	}
}

void PushTransitor::Draw()
{
	if (IsEnd()) {
		return;
	}
	const auto& wsize = Window::GetInstance().GetMaxVector2D();
	SetDrawScreen(DX_SCREEN_BACK);
	auto rate = (float)frame / (float)interval;

	//最終的に新画面が0に来るようにminusoneを用意する
	auto minusone = rate - 1.0f;
	int endX = 0;
	int endY = 0;
	switch (direction) {
	case TransitDirection::LEFT:
		endX = -(int)wsize.x;
		break;
	case TransitDirection::RIGHT:
		endX =  (int)wsize.x;
		break;
	case TransitDirection::UP:
		endY = -(int)wsize.y;
		break;
	case TransitDirection::DOWN:
		endY =  (int)wsize.y;
		break;

	}

	DrawGraphF(endX * rate, endY * rate, oldRt, true);
	DrawGraphF(endX * minusone, endY * minusone, newRt, true);
}