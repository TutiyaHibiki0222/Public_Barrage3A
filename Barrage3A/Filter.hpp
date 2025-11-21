#pragma once

#include "headers.h" 
#include <vector>

class FilterManager {
	friend class Filter;
private:
	FilterManager(){}
	~FilterManager() {
		if(filterScreen >= 0) {
			DeleteGraph(filterScreen); // スクリーンの削除.
			filterScreen = -1;
		}
	}
	int filterScreen = -1; // フィルターのデータ.
	int retumeScene = -1; // フィルターの戻り値.
protected:

	static FilterManager& GetInstance() {
		static FilterManager instance;
		return instance;
	}

	void Init() {
		if(filterScreen < 0) {
			Vector2D screenSize = Window::GetInstance().GetMaxVector2D();
			filterScreen = MakeScreen((int)screenSize.x,(int)screenSize.y, TRUE);
		}

		retumeScene = GetDrawScreen();
		
		SetDrawScreen(filterScreen);
		
		ClearDrawScreen();
	}

	void Draw() {
		DrawGraph(0,0, filterScreen, true); // フィルターのスクリーンを描画.
	}

	void End() {
		SetDrawScreen(retumeScene);
	}

	void Remove() {
		if(filterScreen >= 0) {
			DeleteGraph(filterScreen); // スクリーンの削除.
			filterScreen = -1;
		}
		retumeScene = -1;
	}
};

enum class FilterType {
	NOBLEND				= DX_BLENDMODE_NOBLEND,			// ノーマルブレンド.
	ALPHA				= DX_BLENDMODE_ALPHA,			// αブレンド.
	ADD					= DX_BLENDMODE_ADD,				// 加算ブレンド.
	SUB					= DX_BLENDMODE_SUB,				// 減算ブレンド.
	MUL					= DX_BLENDMODE_MUL,				// 乗算ブレンド.
	SUB2				= DX_BLENDMODE_SUB2,			// 内部処理用減算ブレンド２.
	// BLINEALPHA	= DX_BLENDMODE_BLINEALPHA
	XOR					= DX_BLENDMODE_XOR,				// XORブレンド.
	DESTCOLOR			= DX_BLENDMODE_DESTCOLOR,		// カラーは更新されない.
	INVDESTCOLOR		= DX_BLENDMODE_INVDESTCOLOR,	// 描画先の色の反転値を掛ける.
	INVSRC				= DX_BLENDMODE_INVSRC,			// 描画元の色を反転する.
	MULA				= DX_BLENDMODE_MULA,			// アルファチャンネル考慮付き乗算ブレンド.
	ALPHA_X4			= DX_BLENDMODE_ALPHA_X4,		// αブレンドの描画元の輝度を最大４倍にできるモード.
	ADD_X4				= DX_BLENDMODE_ADD_X4,			// 加算ブレンドの描画元の輝度を最大４倍にできるモード.
	SRCCOLOR			= DX_BLENDMODE_SRCCOLOR,		// 描画元のカラーでそのまま描画される.
	HALF_ADD			= DX_BLENDMODE_HALF_ADD,		// 半加算ブレンド.
	SUB1				= DX_BLENDMODE_SUB1,			// 内部処理用減算ブレンド１.
	PMA_ALPHA			= DX_BLENDMODE_PMA_ALPHA,		// 乗算済みαブレンドモードのαブレンド.
	PMA_ADD				= DX_BLENDMODE_PMA_ADD,			// 乗算済みαブレンドモードの加算ブレンド.
	PMA_SUB				= DX_BLENDMODE_PMA_SUB,			// 乗算済みαブレンドモードの減算ブレンド.
	PMA_INVSRC			= DX_BLENDMODE_PMA_INVSRC,		// 乗算済みαブレンドモードの描画元の色を反転する.
	PMA_ALPHA_X4		= DX_BLENDMODE_PMA_ALPHA_X4,	// 乗算済みαブレンドモードのαブレンドの描画元の輝度を最大４倍にできるモード.
	PMA_ADD_X4			= DX_BLENDMODE_PMA_ADD_X4,		// 乗算済みαブレンドモードの加算ブレンドの描画元の輝度を最大４倍にできるモード.
	LIVE2D_ZERO			= DX_BLENDMODE_LIVE2D_ZERO,		// Live2D のブレンドモード Zero 用.
	LIVE2D_NORMAL		= DX_BLENDMODE_LIVE2D_NORMAL,	// Live2D のブレンドモード Normal 用.
	LIVE2D_ADD			= DX_BLENDMODE_LIVE2D_ADD,		// Live2D のブレンドモード Add 用.
	LIVE2D_MULT			= DX_BLENDMODE_LIVE2D_MULT,		// Live2D のブレンドモード Mult 用.
	LIVE2D_MASK			= DX_BLENDMODE_LIVE2D_MASK,		// Live2D のブレンドモード Mask 用.
	SPINE_NORMAL		= DX_BLENDMODE_SPINE_NORMAL,	// Spine のブレンドモード Normal 用.
	SPINE_ADDITIVE		= DX_BLENDMODE_SPINE_ADDITIVE,	// Spine のブレンドモード Additive 用.
	SPINE_MULTIPLY		= DX_BLENDMODE_SPINE_MULTIPLY,	// Spine のブレンドモード Multiply 用.
	SPINE_SCREEN		= DX_BLENDMODE_SPINE_SCREEN,	// Spine のブレンドモード Screen 用.
	CUSTOM				= DX_BLENDMODE_CUSTOM,			// カスタムブレンドモード.
	DST_RGB_SRC_A		= DX_BLENDMODE_DST_RGB_SRC_A,	// 描画元の A のみを書き込む( 描画先の RGB は変更されない ).
	INVDESTCOLOR_A		= DX_BLENDMODE_INVDESTCOLOR_A,	// 描画先の A の反転値を掛ける( 描画先の RGB は変更されない ).
	MUL_A				= DX_BLENDMODE_MUL_A,			// A のみの乗算ブレンド( 描画先の RGB は変更されない ).
	PMA_INVDESTCOLOR_A	= DX_BLENDMODE_PMA_INVDESTCOLOR_A, // 乗算済みα用の DX_BLENDMODE_INVDESTCOLOR_A.
	PMA_MUL_A			= DX_BLENDMODE_PMA_MUL_A,		// 乗算済みα用の DX_BLENDMODE_MUL_A.
	NUM					= DX_BLENDMODE_NUM				// ブレンドモードの数.
};

class FilterConfig {
private:
	FilterType blendMode; // ブレンドモード.
	int param; 
public:
	FilterConfig(FilterType mode, int p)
	{
		blendMode = mode;
		param = Mathf::Clamp(p, 0, 255);
	}

	FilterType GetBlendMode() const { return blendMode; }
	int GetParam() const { return param; }
};

class Filter {
private:
	FilterConfig filter; // フィルターの設定.
public:
	Filter() : filter(FilterType::NOBLEND , 0) {} // デフォルトはノーブレンド.
	
	// 設定.
	void SetFilter(FilterType mode, int param) {
		filter = (FilterConfig(mode, param));
	}
	// 設定を取得.
	void SetFilter(const FilterConfig& _filter) {
		filter = _filter;
	}

	void FilterCast(std::function<void()> drawFunc) {
		SetDrawBlendMode((int)filter.GetBlendMode(), filter.GetParam()); // ブレンドモードを設定.
		// 中間スクリーンに描画.
		drawFunc();
		// 忘れずに描画モードをリセット.
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
};