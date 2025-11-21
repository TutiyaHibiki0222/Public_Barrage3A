#pragma once

#include <string>
#include <memory>
#include "Mathf.h"

namespace GameEngine {

    // サウンドファイルを読み込み・保持し、基本情報を提供するクラス
    class AudioClip {
    private:
        bool isLoaded;          // ファイルが正しく読み込まれたかどうか
        std::string path;      // 読み込んだフォルダパス.

        std::string title;      // 音楽タイトル名.
        int soundHandle;        // DxLibのサウンドハンドル（再生に使用）
        double durationSeconds; // 音声の再生時間（秒単位）
        int channels;           // チャンネル数
        int sampleRate;         // サンプルレート（Hz）
        float maxVolume = 1.0f;
    public:
        // コンストラクタ：指定されたファイルを読み込み、再生ハンドルと情報を取得
        AudioClip(const std::string& filepath);
        AudioClip();
        // デストラクタ：DxLibのサウンドリソースを解放
        ~AudioClip();

        // 読み込み <成功 true , 失敗 false>.
        bool Load(const std::string&);

        // タイトル名.
        std::string GetTitle() const;

        // DxLib のサウンドハンドルを取得（再生時に使用）
        int GetSoundHandle() const;

        // 音声の再生時間を取得（秒
        double GetDuration() const;

        // チャンネル数を取得
        int GetChannels() const;

        // サンプルレート（Hz）を取得
        int GetSampleRate() const;
        
        // ファイルが正しく読み込まれたかどうか.
        bool GetIsLoaded() const;

        float GetMaxVolume() const { return maxVolume; }
        void SetMaxVolume(float v) { maxVolume = Mathf::Clamp(v, 0.0f, 1.0f); }
    public:
        // 複製.
        std::shared_ptr<AudioClip> Clone() const;

    private:
        // ファイルパスから拡張子を小文字で取得（例: ".wav", ".mp3"）
        std::string GetFileExtension(const std::string& filepath);

        // タイトル名取得 （例: "Resources/Sound/BGM/tmp.wav" -> "tmp"）
        std::string GetPathTitle(const std::string& filepath);
    };
}
