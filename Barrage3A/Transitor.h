/*
	◆ Transitor.h

	クラス名	: Transitor (基礎クラス)
	作成日      : 2025/06/16 (月) 18:48:00
	最終更新日  : 2025/06/16 (月) 19:08:00
	作成者		: 𡈽屋 響
	概要		: トランジスタの基礎クラス
*/
#pragma once

enum class TransitDirection {
	UP,
	DOWN,
	RIGHT,
	LEFT
};

// Transitorクラス
class Transitor {
protected:
	bool isStart = false;
	const int interval;	// 切り替えに掛かる時間.
	int frame	= 0;	// 現在のフレーム数.
	int oldRt	= 0;	// 切り替え前の画面.
	int newRt	= 0;	// 切り替え後の画面.
	// コンストラクタ
	Transitor(int _interval = 60) : interval(_interval) {}
public:
	// デストラクタ
	virtual ~Transitor();
	bool IsStart() const;
	void Start();//演出開始
	virtual void Update();	//Scene::Updateの最後に呼んでね
	virtual void Draw() = 0;	//Scene::Drawの最後に呼んでね
	virtual bool IsEnd()const;	//演出が終了した
	
};
