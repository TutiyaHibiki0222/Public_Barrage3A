/*
    ◆ MusicController.cpp
*/
#include "MusicController.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "AudioResourceShortcut.hpp"
using namespace GameEngine;
using namespace System;

std::weak_ptr<MusicController> MusicController::instance;

// コンストラクタ 
MusicController::MusicController() : AppBase("MusicController") {

}
MusicController::MusicController(std::shared_ptr<GameObject> owner) : AppBase("MusicController", owner) {

}

std::shared_ptr<MusicController> MusicController::GetInstance() {
    return instance.lock();
}

// AppBase Event.
void MusicController::Awake() {
    if (!audioSource) {
        audioSource = gameObject->GetAppBase<AudioSource>();
    }
    if (!audioSource) {
        audioSource = gameObject->AddAppBase<AudioSource>();
    }
}

void MusicController::Start() {
    if (instance.expired()) {
        instance = std::static_pointer_cast<MusicController>(shared_from_this());
        audioSource = gameObject->GetAppBase<AudioSource>();
        if (!audioSource) audioSource = gameObject->AddAppBase<AudioSource>();
        PlayNewClip(musicClip);
        audioSource->SetPlayOnStart(false);
        audioSource->SetLoop(true);
        audioSource->SetVolume(musicVolume);
        gameObject->SetDontDestroyOnLoad(true);
    }
    else {
        if (musicClip)
        {
            auto inst = GetInstance();
            if (!inst->musicClip)
                inst->PlayNewClip(musicClip);
            else if (musicClip->GetTitle() != inst->audioSource->GetClip()->GetTitle())
                inst->FadePlayUnfade(musicClip);
            else
            {
                inst->audioSource->PlayFromStart();
                inst->audioSource->SetVolume(0.0f);
                inst->UnFadeMusic();
            }
                
        }

        Destroy(gameObject);
    }
}

void MusicController::SetMusicClip(std::shared_ptr<AudioClip> clip) {
    musicClip = clip;
}

void MusicController::PlayNewClip(std::shared_ptr<AudioClip> newAudioClip)
{
    if (!newAudioClip) return;
    if (!audioSource) return;

    if (!audioSource->GetClip() ||
        audioSource->GetClip()->GetTitle() != newAudioClip->GetTitle())
    {
        audioSource->Stop();
        musicClip = newAudioClip;
        audioSource->SetAudioClip(newAudioClip);
        audioSource->Play();
    }
}


void MusicController::SkipToPoint(float value)
{
    if (audioSource)
        audioSource->SetPlaybackTime(value);
}

void MusicController::SetLoopSong(bool isLoop)
{
    if (audioSource)
        audioSource->SetLoop(isLoop);
}

void MusicController::SetMusicVolume(float vol) {
    musicVolume = std::clamp(vol, 0.0f, 1.0f);
    maxVolume = musicVolume; // フェード用の最大音量も更新
    if (audioSource) {
        audioSource->SetVolume(musicVolume);
    }
}

void MusicController::SetSEVolume(float vol) {
    seVolume = std::clamp(vol, 0.0f, 1.0f);
}

// ---------------------------
// ポーズ処理（仕様）
// - PauseBGM(): 再生位置を保存し、**フェードアウト**して停止。
// - ResumeBGM(): 保存位置から再生し、**フェードイン**。
// ---------------------------
System::Coroutine MusicController::IE_PauseFadeOut(float duration) {
    if (!audioSource) co_return;

    float startVolume = audioSource->GetVolume();
    float t = 0.0f;

    // duration 秒で線形フェードアウト
    while (t < duration) {
        t += Time.unscaledDeltaTime;
        float r = Mathf::Clamp01(t / duration);
        float newVolume = Mathf::Lerp(startVolume, 0.0f, r);
        audioSource->SetVolume(newVolume);
        _yield nullptr;
    }

    audioSource->SetVolume(0.0f);
    audioSource->Pause();    // StopではなくPause
    co_return;
}


System::Coroutine MusicController::IE_ResumeFadeIn(float duration) {
    if (!audioSource || !musicClip) co_return;

    audioSource->Play(); // Pause → 再生

    float startVolume = audioSource->GetVolume();   // 多くの場合 0
    float t = 0.0f;

    // duration 秒で線形フェードイン
    while (t < duration) {
        t += Time.unscaledDeltaTime;
        float r = Mathf::Clamp01(t / duration);
        float newVolume = Mathf::Lerp(startVolume, maxVolume, r);
        audioSource->SetVolume(newVolume);
        _yield nullptr;
    }

    audioSource->SetVolume(maxVolume);
    co_return;
}

void MusicController::PauseBGM() {
    if (!audioSource || !audioSource->GetClip()) return;

    pausedTime = audioSource->GetClip()->GetIsLoaded()
        ? static_cast<float>(GetSoundCurrentTime(audioSource->GetClip()->GetSoundHandle())) / 1000.0f
        : 0.0f;

    isPaused = true;

    StopAllCoroutine();
    StartCoroutine(IE_PauseFadeOut());
}

void MusicController::ResumeBGM() {
    if (!isPaused) return;

    StopAllCoroutine();
    StartCoroutine(IE_ResumeFadeIn());

    isPaused = false;
}



// ---------------------------
// ゲームオーバー処理（仕様）
// - TriggerGameOver(): 現在を即時停止（位置保持）、gameOverClip に切替えてフェードインで再生。
// - ContinueFromGameOverAndResume(): ゲームオーバー解除 → 元の曲を保存位置からフェードインで再開。
// ---------------------------
void MusicController::TriggerGameOver() {
    if (!audioSource || !gameOverClip) return;

    // 今の曲を保存する（再開用）
    if (audioSource->GetClip()) {
        previousClip = audioSource->GetClip();
        pausedTime = previousClip->GetIsLoaded()
            ? static_cast<float>(GetSoundCurrentTime(previousClip->GetSoundHandle())) / 1000.0f
            : 0.0f;
    }
    else {
        previousClip = nullptr;
        pausedTime = 0.0f;
    }

    inGameOver = true;

    // 切替用 clip にセットだけしておく
    musicClip = gameOverClip;

    StopAllCoroutine();
    StartCoroutine(IE_FadePlayUnfade());   // ← フェードアウト → フェードイン
}

void MusicController::ContinueFromGameOverAndResume() {
    if (!audioSource || !previousClip) return;

    inGameOver = false;

    // 元の曲に戻す
    musicClip = previousClip;

    StopAllCoroutine();
    StartCoroutine(IE_FadePlayUnfade(0.5f,true));  // ← ゲームオーバー曲をフェードアウト → 本来の曲をフェードイン
}

// ---------------------------
// OneShotAudio / helpers
// ---------------------------
std::shared_ptr<AudioSource> MusicController::OneShotAudio(std::shared_ptr<AudioClip> newAudioClip, float v)
{
    if (!newAudioClip) return nullptr;
    auto tmpObj = GameObject::Instantiate("ShotAudio_" + newAudioClip->GetTitle());
    tmpObj->transform->position = gameObject->transform->position;

    auto tmpAudio = tmpObj->AddAppBase<AudioSource>();
    tmpAudio->SetAudioClip(newAudioClip);
    tmpAudio->SetVolume(v);
    tmpAudio->Play();
    // Destroy 設定で クリップの最大時間をセット.
    Destroy(tmpObj, (float)newAudioClip->GetDuration());

    return tmpAudio;
}
std::shared_ptr<AudioSource> MusicController::OneShotAudio(std::string key, float v)
{
    auto newAudioClip = Sounds[key];
    if (!newAudioClip) return nullptr;
    auto tmpObj = GameObject::Instantiate("ShotAudio_" + newAudioClip->GetTitle());
    tmpObj->transform->position = gameObject->transform->position;

    auto tmpAudio = tmpObj->AddAppBase<AudioSource>();
    tmpAudio->SetAudioClip(newAudioClip);
    tmpAudio->SetVolume(v);
    tmpAudio->Play();
    // Destroy 設定で クリップの最大時間をセット.
    Destroy(tmpObj, (float)newAudioClip->GetDuration());

    return tmpAudio;
}
void MusicController::FadeMusic()
{
    StopAllCoroutine();
    StartCoroutine(IE_FadeMusic());
}

void MusicController::UnFadeMusic()
{
    StopAllCoroutine();
    StartCoroutine(IE_UnFadeMusic());
}

void MusicController::FadePlayUnfade(std::shared_ptr<AudioClip> newAudioClip)
{
    StopAllCoroutine();
    musicClip = newAudioClip;
    StartCoroutine(IE_FadePlayUnfade());
}


// ---------------------------
// Coroutines (フェード処理)
// ---------------------------

System::Coroutine MusicController::IE_FadeMusic(float duration) {
    if (!audioSource) co_return;

    float startVolume = audioSource->GetVolume();
    float t = 0.0f;

    while (t < duration) {
        t += Time.unscaledDeltaTime;
        float r = Mathf::Clamp01(t / duration);
        float newVolume = Mathf::Lerp(startVolume, 0.0f, r);
        audioSource->SetVolume(newVolume);
        _yield nullptr;
    }

    audioSource->SetVolume(0.0f);
    audioSource->Stop();
    co_return;
}


System::Coroutine MusicController::IE_UnFadeMusic(float duration) {
    if (!audioSource) co_return;

    float startVolume = audioSource->GetVolume();  // おそらく 0
    float t = 0.0f;

    while (t < duration) {
        t += Time.unscaledDeltaTime;
        float r = Mathf::Clamp01(t / duration);
        float newVolume = Mathf::Lerp(startVolume, maxVolume, r);
        audioSource->SetVolume(newVolume);
        _yield nullptr;
    }

    audioSource->SetVolume(maxVolume);
    co_return;
}


System::Coroutine MusicController::IE_FadePlayUnfade(float duration,bool reStart) {
    if (!audioSource) co_return;

    float startVolume = audioSource->GetVolume();
    float t = 0.0f;

    // ---------- フェードアウト ----------
    while (t < duration) {
        t += Time.unscaledDeltaTime;
        float r = Mathf::Clamp01(t / duration);
        float newVolume = Mathf::Lerp(startVolume, 0.0f, r);
        audioSource->SetVolume(newVolume);
        _yield nullptr;
    }

    // 停止してクリップ切替
    audioSource->SetVolume(0.0f);
    audioSource->Stop();
    audioSource->SetAudioClip(musicClip);
    audioSource->Play(reStart);

    // ---------- フェードイン ----------
    t = 0.0f;
    while (t < duration) {
        t += Time.unscaledDeltaTime;
        float r = Mathf::Clamp01(t / duration);
        float newVolume = Mathf::Lerp(0.0f, maxVolume, r);
        audioSource->SetVolume(newVolume);
        _yield nullptr;
    }

    audioSource->SetVolume(maxVolume);
    co_return;
}



std::shared_ptr<AudioSource> MusicController::GetAudioSource() {
    if (!audioSource) {
        audioSource = gameObject->GetAppBase<AudioSource>();
    }
    if (!audioSource) {
        audioSource = gameObject->AddAppBase<AudioSource>();
    }
    return audioSource;
}
