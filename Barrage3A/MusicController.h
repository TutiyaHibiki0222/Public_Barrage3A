/*
    ◆ MusicController.h

    クラス名        : MusicController クラス
    作成日          : 2025/07/13 (日) 14:17:52
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "AudioSource.h"
#include "AudioClip.h"

// MusicController class and AppBase class
class MusicController : public AppBase, public std::enable_shared_from_this<MusicController> {
private:    // 非公開.
    static std::weak_ptr<MusicController> instance;

    std::shared_ptr<AudioSource> audioSource;
    std::shared_ptr<GameEngine::AudioClip> musicClip;        // 現在（または通常）のBGM
    std::shared_ptr<GameEngine::AudioClip> previousClip;     // ゲームオーバー前に再生していた曲を保存

    float musicVolume = 1.0f;
    float maxVolume = 1.0f;
    float seVolume = 1.0f;

    bool isFadingOut = false;
    bool isFadingIn = false;

    std::shared_ptr<GameEngine::AudioClip> gameOverClip;     // ゲームオーバー用BGM
    float pausedTime = 0.0f;                                // 停止時の再生位置（秒）
    bool isPaused = false;                                  // ポーズ中フラグ
    bool inGameOver = false;                                // ゲームオーバー中フラグ

public:     // 公開.

    // コンストラクタ.
    MusicController();
    MusicController(std::shared_ptr<GameObject>);

    static std::shared_ptr<MusicController> GetInstance();

    void SetMusicClip(std::shared_ptr<GameEngine::AudioClip>);

    // ゲームオーバー用クリップをセット
    void SetGameOverClip(std::shared_ptr<GameEngine::AudioClip> clip) {
        gameOverClip = clip;
    }

    void PlayNewClip(std::shared_ptr<GameEngine::AudioClip> newClip);
    void FadePlayUnfade(std::shared_ptr<GameEngine::AudioClip> newClip);
    void UnFadeMusic();
    void FadeMusic();

    void SetLoopSong(bool isLoop);
    void SkipToPoint(float time);

    void SetMusicVolume(float vol);
    void SetSEVolume(float vol);

    // ポーズ処理
    void PauseBGM();    // フェードアウトして停止（位置保存）
    void ResumeBGM();   // 停止位置から再生（フェードイン）

    // ゲームオーバー処理
    void TriggerGameOver();                      // 現在を停止して gameOverClip をフェードインで再生
    void ContinueFromGameOverAndResume();        // ゲームオーバー解除 → 元曲を pausedTime からフェードインで再開

    std::shared_ptr<AudioSource> OneShotAudio(std::shared_ptr<GameEngine::AudioClip> clip, float = MusicController::GetInstance()->seVolume);
    std::shared_ptr<AudioSource> OneShotAudio(std::string key, float = MusicController::GetInstance()->seVolume);

    // AppBase 関数.
    void Awake()    override;
    void Start()    override;

    std::shared_ptr<AudioSource> GetAudioSource();
private:
    System::Coroutine IE_FadeMusic(float duration = 0.5f);            // フェードアウト（停止を含める）
    System::Coroutine IE_UnFadeMusic(float duration = 0.5f);          // フェードイン（現在再生中のclipを maxVolume まで）
    System::Coroutine IE_FadePlayUnfade(float duration = 0.5f, bool reStart = false);       // 既存: フェードアウト→切替→フェードイン
    System::Coroutine IE_PauseFadeOut(float duration = 0.5f);
    System::Coroutine IE_ResumeFadeIn(float duration = 0.5f);
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<MusicController>(*this);
    }
};
