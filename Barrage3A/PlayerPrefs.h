#pragma once
/*
    PlayerPrefs.h
*/

#include <string>
#include <unordered_map>

/// <summary>
/// データ保存.
/// </summary>
class PlayerPrefs {
private:
    static std::unordered_map<std::string, std::string> prefs;
    static std::string filename;
    static bool initialized;
    static bool needsSave;

public:
    // 初期化（Loadを内部で呼ぶ）
    static void Init();

    // データを保存
    static void Save();

    // データを読み込み
    static void Load();

    // データをセット
    static void SetInt(const std::string& key, int value);
    static void SetFloat(const std::string& key, float value);
    static void SetString(const std::string& key, const std::string& value);

    // データを取得
    static int GetInt(const std::string& key, int defaultValue = 0);
    static float GetFloat(const std::string& key, float defaultValue = 0.0f);
    static std::string GetString(const std::string& key, const std::string& defaultValue = "");

    // 指定キーの存在確認
    static bool HasKey(const std::string& key);

    // 全てのデータを削除
    static void DeleteAll();

    // 指定キーのデータを削除
    static void DeleteKey(const std::string& key);
};
