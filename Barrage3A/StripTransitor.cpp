
#include "headers.h"
#include "StripTransitor.h"

StripTransitor::StripTransitor(int interval, int _width) : Transitor(interval), width(_width)
{}


void StripTransitor::Draw()
{
	if (IsEnd()) {
		return;
	}
	SetDrawScreen(DX_SCREEN_BACK);
	const auto& wsize = Window::GetInstance().GetMaxVector2D();
	auto rate = (float)frame/ (float)interval;
	int lp = Mathf::Round<int>(wsize.x / width) + 1;
	DrawRectGraph(0, 0, 0, 0, Mathf::Round<int>(wsize.x), Mathf::Round<int>(wsize.y), oldRt, true);
	for (int i = 0; i < lp; ++i) {
		if (i % 2 == 0) {
			DrawRectGraph(i * width, (int)(wsize.y * (rate - 1.0f)), i * width, 0, width, (int)wsize.y, newRt, true);
		}
		else {
			DrawRectGraph(i * width, (int)(wsize.y * (1.0f - rate)), i * width, 0, width, (int)wsize.y, newRt, true);
		}
	}
}