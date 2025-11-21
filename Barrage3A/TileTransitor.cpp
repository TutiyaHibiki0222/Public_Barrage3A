#include "headers.h"
#include "TileTransitor.h"

TileTransitor::TileTransitor(int interval, int _width, bool _directionRightDown) 
	: Transitor(interval), width(_width), directionRightDown(_directionRightDown)
{
}

void TileTransitor::Draw()
{
	if (IsEnd()) return;

	SetDrawScreen(DX_SCREEN_BACK);

	const auto& wsize = Window::GetInstance().GetMaxVector2D();
	float rate = (float)frame / (float)interval;

	int winX = Mathf::Round<int>(wsize.x);
	int winY = Mathf::Round<int>(wsize.y);

	int cols = (winX / width) + 1;
	int rows = (winY / width) + 1;

	DrawRectGraph(0, 0, 0, 0, winX, winY, oldRt, true);

	for (int j = 0; j < rows; ++j) {
		for (int i = 0; i < cols; ++i) {
			// 交互タイル指定（チェス盤）
			bool isBlack = (i + j) % 2 == 0;

			// タイル出現の進行順（斜め方向）
			int delayA = (i + j);                         // 左上→右下
			int delayB = (cols - 1 - i) + (rows - 1 - j); // 右下→左上

			int delay = isBlack ? delayB : delayA; // 黒タイルは右下から、白タイルは左上から

			float progress = (float)delay / (cols + rows);
			if (rate >= progress) {
				int x = i * width;
				int y = j * width;
				DrawRectGraph(x, y, x, y, width, width, newRt, true);
			}
		}
	}
}
