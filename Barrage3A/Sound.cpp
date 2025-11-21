#include "Project.h"
#include "Sound.h"

// サウンドマネージャーのインスタンスの初期化
MySoundManager MySoundManager::sm_sndMgrObj;

/////////////////////// MySound class ///////////////////////////

// サウンドを解放する
void MySound::Release() {
    if (m_handle != -1) {
        DeleteSoundMem(m_handle);
        m_handle = -1;
    }
}

// サウンドを再生する
void MySound::Play(bool loop) {
    if (m_handle != -1) {
        PlaySoundMem(m_handle, loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK, TRUE);
    }
}

// サウンドを停止する
void MySound::Stop() {
    if (m_handle != -1) {
        StopSoundMem(m_handle);
    }
}

// サウンドの音量を設定する
void MySound::SetVolume(int volume) {
    if (m_handle != -1) {
        ChangeVolumeSoundMem(volume, m_handle);
    }
}

/////////////////////// MySoundManager class ///////////////////////////

// サウンドマネージャーのデストラクタ.
MySoundManager::~MySoundManager() {
    Release();
}

// サウンドデバイスの初期化.
bool MySoundManager::InitSound() {
    return true; // DxLibは初期化時にサウンドも初期化されるので、特に何もしない.
}

// サウンドデバイスの解放.
void MySoundManager::Release() {
    // SEの 解放.
    for (int i = SE_COUNT - 1; i >= 0; i--) {
        m_se[i].Release();
    }
    // BGMの 解放.
    for (int i = BGM_COUNT - 1; i >= 0; i--) {
        m_bgm[i].Release();
    }
}

// サウンドデータの読み込み.
void MySoundManager::LoadSoundData() {

    // BGMのファイルパス.
    static const TCHAR* pcszBgmFileName[BGM_COUNT] = {
        _T("NoFilePath"),
    };
    // BGMの読み込み処理.
    for (int i = BGM_BEGIN; i < BGM_COUNT; i++) {
        if (!LoadSoundFile(&m_bgm[i], pcszBgmFileName[i])) {
            throw - 1;
        }
    }

    // SEのファイルパス.
    static const TCHAR* pcszSeFileName[SE_COUNT] = {
        _T("NoFilePath"),
    };
    // SEの読み込み処理.
    for (int i = SE_BEGIN; i < SE_COUNT; i++) {
        if (!LoadSoundFile(&m_se[i], pcszSeFileName[i])) {
            throw - 1;
        }
    }
}

// BGMを再生する
void MySoundManager::PlayBGM(E_BGM bgm, bool loop) {
    if (IS_VALID_BGM(bgm)) {
        m_bgm[bgm].Play(loop);
    }
}

// すべてのBGMを停止する
void MySoundManager::StopAllBGM() {
    for (int i = 0; i < BGM_COUNT; ++i) {
        m_bgm[i].Stop();
    }
}

// 効果音の音量を設定する
void MySoundManager::SetBGMVolume(E_BGM bgm, int volume) {
    if (IS_VALID_SE(bgm)) {
        m_se[bgm].SetVolume(volume);
    }
}

// 効果音を再生する
void MySoundManager::PlaySE(E_SE se, bool loop) {
    if (IS_VALID_SE(se)) {
        m_se[se].Play(loop);
    }
}

// 効果音の音量を設定する
void MySoundManager::SetSEVolume(E_SE se, int volume) {
    if (IS_VALID_SE(se)) {
        m_se[se].SetVolume(volume);
    }
}

// ファイルを読み込む
bool MySoundManager::LoadSoundFile(MySound* sound, const char* fname) {
    int handle = LoadSoundMem(fname);
    if (handle == -1) {
        return false;
    }
    sound->m_handle = handle;
    return true;
}