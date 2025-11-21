#include <DxLib.h>

#include "AudioClip.h"
#include "Debug.hpp"
#include "AudioAnalyzer.hpp"
#include "SystemIO.h"
namespace GameEngine {

    AudioClip::AudioClip(const std::string& filepath)
        : soundHandle(-1), durationSeconds(0.0), channels(-1), sampleRate(-1), isLoaded(false), title(""), path("")
    {
        Load(filepath);
    }

    AudioClip::AudioClip()
        : soundHandle(-1), durationSeconds(0.0), channels(-1), sampleRate(-1), isLoaded(false), title(""), path("")
    {}

    AudioClip::~AudioClip() {
        if (soundHandle != -1) {
            DeleteSoundMem(soundHandle);
            soundHandle = -1;
        }
    }


    bool AudioClip::Load(const std::string& filepath) {
        soundHandle = LoadSoundMem(filepath.c_str());
        if (soundHandle == -1) {
            Debug::ErrorLog("音声ファイルの読み込みに失敗しました: {}", filepath);
            return false;
        }

        std::string ext = GetFileExtension(filepath);
        bool parsed = false;
        if (ext == ".mp3") {
            parsed = System::MP3Info::Parse(filepath, durationSeconds, channels, sampleRate);
        }
        else if (ext == ".wav") {
            parsed = System::WAVInfo::Parse(filepath, durationSeconds, channels, sampleRate);
        }
        else if (ext == ".ogg") {
            parsed = System::OGGInfo::Parse(filepath, durationSeconds, channels, sampleRate);
        }
        else if (ext == ".opus") {
            parsed = System::OPUSInfo::Parse(filepath, durationSeconds, channels, sampleRate);
        }
        else {
            Debug::ErrorLog("対応していない 形式です : {}", ext);
        }

        if (!parsed) {
            Debug::WarningLog("音声情報の解析に失敗しました : {0:}", filepath);          
        }
        // 最終結果.
        isLoaded = parsed;
        title    = GetPathTitle(filepath);
        path     = filepath;
        return parsed;
    }

    std::string AudioClip::GetTitle() const {
        return title;
    }

    int AudioClip::GetSoundHandle() const {
        return soundHandle;
    }

    double AudioClip::GetDuration() const {
        return durationSeconds;
    }

    int AudioClip::GetChannels() const {
        return channels;
    }

    int AudioClip::GetSampleRate() const {
        return sampleRate;
    }

    bool AudioClip::GetIsLoaded() const {
        return isLoaded;
    }

    std::shared_ptr<AudioClip> AudioClip::Clone() const {
        if (!isLoaded) {
            Debug::WarningLog("AudioClip: Cloneできません（未ロード）。");
            return nullptr;
        }
        // 複製処理..
        auto clip = std::make_shared<AudioClip>(*this);
        clip->soundHandle = DuplicateSoundMem(soundHandle);
        if (clip->soundHandle < 0) {
            Debug::ErrorLog("AudioClip: Clone 複製 失敗 [{}]", path);
            clip->isLoaded = false;
            return nullptr;
        }
        clip->isLoaded = true;
        return clip;
    }

    std::string AudioClip::GetFileExtension(const std::string& filepath) {
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos == std::string::npos) return "";
        std::string ext = filepath.substr(dotPos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }

    std::string AudioClip::GetPathTitle(const std::string& filepath)
    {
        return System::IO::Path::GetFileNameWithoutExtension(filepath);  // 拡張子なしのファイル名を取得
    }
}
