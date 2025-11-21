/*
    ◆ AudioSource.h

    クラス名        : AudioSource クラス
    作成日          : 2025/06/13 (金) 11:13:57
    最終変更日      : 2025/06/15 (日) 14:49:01
    作成者          : 𡈽屋 響
    概要            : 音に関係する 物.
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "AudioClip.h"

// AudioSource class and AppBase class
class AudioSource : public AppBase {
private:    // 非公開.
    std::shared_ptr<GameEngine::AudioClip> clip;     // 音実態データ.
    // Output <>.
    bool mute                   = false;    // ミュートかどうか.
    bool bypassEffects          = false;    // エフェクトを無視.
    bool bypassListenerEffects  = false;    // AudioListener による影響.
    bool bypassReverbZones      = false;    // リバーブゾーン効果を無視.
    bool playOnStart            = true;     // 実行時 音を鳴らすか.
    bool loop                   = false;    // ループするかどうか.
private: // ユーザーに関係する変数.
    float priority      = 128.0f;           // 同時再生数が多い時の優先度.
    float volume        = 1.0f;             // 音量 0.0 ~ 1.0.
    float pitch         = 1.0f;             // 再生速度（高さ）.
    float stereoPan     = 0.0f;             // ステレオの左右バランス -1.0 ~ 1.0.
    float spatialBlend  = 0.0f;             // 2D〜3D間のブレンド（0: 2D, 1: 3D）
    float reverbZoneMix = 1.0f;             // リバーブゾーンとのミックス度合い.
private:
    bool  isPause       = false;            // ポーズ時.

public:     // 公開.

    // コンストラクタ.
    AudioSource();
    AudioSource(std::shared_ptr<GameObject>);
	~AudioSource() = default;

	void SetAudioClip(const GameEngine::AudioClip&);			// AudioClip を設定.
	void SetAudioClip(std::shared_ptr<GameEngine::AudioClip>);  // AudioClip を設定.

	std::shared_ptr<GameEngine::AudioClip> GetClip() const;

	void PlayFromStart();						// 先頭から再生
	void Play(bool restart = false);			// 再生<再生中は実行できない>.
	void PlayOverlap();							// SE用など、重ねて何度でも再生したいときに使う.
	void Stop();								// 停止<再生中のみ実行できる>.
	void Pause();								// 一時停止<再生中のみ実行できる>.
	void Resume();								// 再開<一時停止中のみ実行できる>.
	bool IsPlaying() const;						// 再生中かどうか.

	bool IsMute() const;						// ミュートかどうか.
	bool IsBypassEffects() const;				// エフェクトを無視しているか.
	bool IsBypassListenerEffects() const;		// AudioListener による影響を無視しているか.
	bool IsBypassReverbZones() const;			// リバーブゾーン効果を無視しているか.
	bool IsPlayOnStart() const;					// 実行時 音を鳴らすかどうか.
	bool IsLoop() const;						// ループ再生かどうか.
	void Skip(float);

	float GetPriority() const;					// 優先度.
	float GetVolume() const;					// 音量 0.0 ~ 1.0.
	float GetPitch() const;						// 再生速度（高さ）.
	float GetStereoPan() const;					// ステレオの左右バランス -1.0 ~ 1.0.
	float GetSpatialBlend() const;				// 2D〜3D間のブレンド（0: 2D, 1: 3D）
	float GetReverbZoneMix() const;				// リバーブゾーンとのミックス度合い.

	void SetMute(bool value);                   // ミュートにするかどうか.
	void SetBypassEffects(bool value);          // エフェクトを無視するかどうか.
	void SetBypassListenerEffects(bool value);  // AudioListener による影響を無視するかどうか.
	void SetBypassReverbZones(bool value);      // リバーブゾーン効果を無視するかどうか.
	void SetPlayOnStart(bool value);            // 実行時 音を鳴らすかどうか.
	void SetLoop(bool value);                   // ループするかどうか.
	void SetPlaybackTime(float);

	void SetPriority(float value);              // 優先度を設定.
	void SetVolume(float value);                // 音量を設定 0.0 ~ 1.0.
	void SetPitch(float value);                 // 再生速度（高さ）を設定.
	void SetStereoPan(float value);             // ステレオの左右バランスを設定 -1.0 ~ 1.0.
	void SetSpatialBlend(float value);          // 2D〜3D間のブレンドを設定（0: 2D, 1: 3D）
	void SetReverbZoneMix(float value);         // リバーブゾーンとのミックス度合いを設定.

	void SetLoopReturnOffset(float samplePos);  // ループ開始位置を設定.

    // AppBase 関数.
	void Start()    override;                   // 初期化処理.
	void OnDestroy()override;
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<AudioSource>(*this);
    }
};