#pragma once

#include "Transitor.h"

class IrisTransitor : public Transitor
{
private:
	int handleForMaskScreen;//マスク転送用グラフィックハンドル
	int maskH;//マスクハンドル
	float diagonalLength;//対角線の長さ
	bool irisOut = false;//アイリスアウトフラグ(falseならアイリスイン)
public:
	IrisTransitor(int interval,bool irisOut = false);
	~IrisTransitor();
	virtual void Update() override;
	virtual void Draw() override;
};