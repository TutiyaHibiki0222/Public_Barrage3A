/*
    ◆ AudioSource.cpp

    クラス名        : AudioSource クラス
    作成日          : 2025/06/13 (金) 11:13:57
	最終変更日      : 2025/06/15 (日) 14:49:01
    作成者          : 𡈽屋 響
    概要            : 音に関係する 物.
*/
#include "AudioSource.h"
#include "GameObject.h"
#include "GameObjectMgr.h"

// コンストラクタ 
AudioSource::AudioSource() : AppBase("AudioSource"){

}
AudioSource::AudioSource(std::shared_ptr<GameObject> owner) : AppBase("AudioSource", owner) {

}

void AudioSource::SetAudioClip(const AudioClip& _clip) {
    clip = std::make_shared<GameEngine::AudioClip>(_clip);
}

void AudioSource::SetAudioClip(std::shared_ptr<GameEngine::AudioClip> _clip) {
    clip = _clip;
}

std::shared_ptr<GameEngine::AudioClip> AudioSource::GetClip() const {
    return clip;
}

void AudioSource::PlayFromStart() {
    if (!clip || !clip->GetIsLoaded()) return;

    int handle = clip->GetSoundHandle();

    // 再生中でも一旦停止して先頭から再生
    StopSoundMem(handle);

    // ループ判定も反映
    PlaySoundMem(handle, loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK, TRUE);

    isPause = false;
}

void AudioSource::Play(bool restart) {
    if (!IsPlaying() && clip && clip->GetIsLoaded()) {
        int handle = clip->GetSoundHandle();
        PlaySoundMem(handle, loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK, (isPause || restart) ? FALSE : TRUE);
        isPause = false;
    }
}
void AudioSource::PlayOverlap() {
    if (!clip || !clip->GetIsLoaded()) return;
    int handle = clip->GetSoundHandle();
    PlaySoundMem(handle, DX_PLAYTYPE_BACK, TRUE);
}
void AudioSource::Stop() {
    if (IsPlaying() && clip) {
        int handle = clip->GetSoundHandle();
        StopSoundMem(handle);
    }
}

void AudioSource::Pause() {
    if (IsPlaying() && clip->GetIsLoaded()) {
        isPause = true;
        Stop();
    }
}

void AudioSource::Resume() {
    if (!IsPlaying() && clip->GetIsLoaded()) {
        if (isPause) {
            Play();
        }
    }
}

bool AudioSource::IsPlaying() const {
    if (!clip) return false;
    return CheckSoundMem(clip->GetSoundHandle()) == 1;
}

void AudioSource::Skip(float deltaSeconds) {
    if (!clip) return;

    int handle = clip->GetSoundHandle();
    int currentMs = (int)GetSoundCurrentTime(handle);
    int skipMs = Mathf::Round<int>(deltaSeconds * 1000);
    int targetMs = currentMs + skipMs;

    // Clamp: 負の位置にならないように
    if (targetMs < 0) targetMs = 0;

    SetSoundCurrentTime(targetMs, handle);
}


bool AudioSource::IsMute() const { return mute; }
bool AudioSource::IsBypassEffects() const { return bypassEffects; }
bool AudioSource::IsBypassListenerEffects() const { return bypassListenerEffects; }
bool AudioSource::IsBypassReverbZones() const { return bypassReverbZones; }
bool AudioSource::IsPlayOnStart() const { return playOnStart; }
bool AudioSource::IsLoop() const { return loop; }

float AudioSource::GetPriority() const {
    return priority;
}

float AudioSource::GetVolume() const {
    return volume;
}

float AudioSource::GetPitch() const {
    return pitch;
}

float AudioSource::GetStereoPan() const {
    return stereoPan;
}

float AudioSource::GetSpatialBlend() const {
    return spatialBlend;
}

float AudioSource::GetReverbZoneMix() const {
    return reverbZoneMix;
}

void AudioSource::SetMute(bool value) {
    mute = value;
    if (!clip) return;  // clipがないなら何もしない

    int handle = clip->GetSoundHandle();
    if (mute) {
        ChangeVolumeSoundMem(0, handle);
    }
    else {
        ChangeVolumeSoundMem(static_cast<int>(volume * 255), handle);
    }
}
void AudioSource::SetBypassEffects(bool value) { bypassEffects = value; }
void AudioSource::SetBypassListenerEffects(bool value) { bypassListenerEffects = value; }
void AudioSource::SetBypassReverbZones(bool value) { bypassReverbZones = value; }
void AudioSource::SetPlayOnStart(bool value) { playOnStart = value; }
void AudioSource::SetLoop(bool value) { loop = value; }
void AudioSource::SetPriority(float value) { priority = value;}
void AudioSource::SetPlaybackTime(float value) {
    if (!clip) return;
    int time = Mathf::Round<int>(value * 1000);
    int handle = clip->GetSoundHandle();
    SetSoundCurrentTime(time, handle);
}

void AudioSource::SetVolume(float value) {
    if (!clip) return;
    else {
        value = Mathf::Min(value, clip->GetMaxVolume());
    }
    volume = Mathf::Clamp(value,0.0f,1.0f);

    if (mute) {
        ChangeVolumeSoundMem(0, clip->GetSoundHandle());
    }
    else {
        ChangeVolumeSoundMem(static_cast<int>(volume * 255), clip->GetSoundHandle());
    }
}

void AudioSource::SetPitch(float value) {
    pitch = value;
    if (!clip) return;
    int handle   = clip->GetSoundHandle();
    int baseFreq = clip->GetSampleRate();
    int newFreq = static_cast<int>(baseFreq * pitch);
    SetFrequencySoundMem(newFreq, handle);
}

void AudioSource::SetStereoPan(float value) {
    stereoPan = value;
    stereoPan = Mathf::Clamp(stereoPan, -1.0f, 1.0f);
    if (!clip) return;
    int handle = clip->GetSoundHandle();
    // DxLib でパン設定
    ChangePanSoundMem(static_cast<int>(stereoPan * 255), handle);
}

void AudioSource::SetSpatialBlend(float value)  { spatialBlend = value;}

void AudioSource::SetReverbZoneMix(float value) { reverbZoneMix = value;}

// 秒数でループ位置を指定したい場合
void AudioSource::SetLoopReturnOffset(float seconds) {
    if (!clip) return;
    int handle = clip->GetSoundHandle();
    int sampleRate = clip->GetSampleRate();
    int samplePos = static_cast<int>(sampleRate * seconds);
    SetLoopSamplePosSoundMem(samplePos, handle);
}


// AppBase Event.

void AudioSource::Start() {
    if (clip && playOnStart) {
        Play();
    }
}

void AudioSource::OnDestroy() {
    Stop();
}
