#pragma once
/*
	Sound.h
*/

#include <cassert>
// BGM一覧.
enum E_BGM
{
	BGM_0,			// 戦闘BGM.

	BGM_COUNT,		// BGMデータの最大個数.
	BGM_INVALID = -1,
	BGM_BEGIN,
};
#define IS_VALID_BGM(no) ((BGM_BEGIN <= (no)) && ((no) < BGM_COUNT))

// 効果音一覧.
enum E_SE
{
	SE_0,

	SE_COUNT,		// SEデータの最大個数.
	SE_INVALID = -1,
	SE_BEGIN,
};
#define IS_VALID_SE(no) ((SE_BEGIN <= (no)) && ((no) < SE_COUNT))

////////////////////////// MySound //////////////////////////
/// <summary>
/// サウンド１つを扱うためのクラス.
/// </summary>
class MySound {
	friend class MySoundManager;
private:
	int m_handle;				// サウンドデータ.
	MySound() : m_handle(-1){}	// MySoundManagerだけでしか持てないオブジェクトになる.
public:
	~MySound() { assert(m_handle == -1); } // サウンドハンドルの解放はMySoundManagerが責任を持つ.

	void Release();
	void Play(bool loop);
	void Stop();
	void SetVolume(int volume); // 音量を設定する.
};

////////////////////////// MySoundManager //////////////////////////
/// <summary>
/// サウンド類を管理するクラス.
/// </summary>
class MySoundManager {
private:
	static MySoundManager sm_sndMgrObj;  // このクラス自身のオブジェクト（実体）の宣言.

	//  勝手にオブジェクトを追加できないようにコンストラクタをprivate領域に配置.
	MySoundManager() {}

	// サウンド.
	MySound m_bgm[BGM_COUNT];
	MySound m_se[SE_COUNT];

public:
	~MySoundManager();

	static MySoundManager* GetInst() { return &sm_sndMgrObj; }

	bool InitSound();		// 音まわりの初期化.
	void Release();			// サウンドデバイスの解放.

	void LoadSoundData();						// サウンドデータ読み込み.
	// BGM類の関数.
	void PlayBGM(E_BGM, bool loop = false);		// BGMを再生.
	void StopAllBGM();							// BGMを停止.
	void SetBGMVolume(E_BGM bgm,int volume);	// BGMの音量を設定
	// SE類の関数.
	void PlaySE(E_SE, bool loop = false);		// SEを再生.
	void SetSEVolume(E_SE se,   int volume);	// SEの音量を設定
private:
	bool LoadSoundFile(MySound* sound, const char* fname); // ファイルの読み込み.
};

inline  MySoundManager* GetSoundInst() { return MySoundManager::GetInst(); }