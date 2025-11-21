#pragma once
#include <string>
#include <memory>
#include "AudioResource.hpp"
#include "Debug.hpp"

namespace GameEngine {

    class AudioResourceShortcut {
    public:
        // CSVから一括読み込み（成功判定も返す）
        static bool LoadFromCSV(const std::string& csvPath) {
            return AudioResource::GetInstance().LoadFromCSV(csvPath);
        }

        // ID文字列でAudioClip取得（失敗時はnullptr）
        static std::shared_ptr<AudioClip> GetClip(const std::string& id) {
            return AudioResource::GetInstance().GetClip(id);
        }

        // ID文字列で最大音量取得（無ければ1.0f）
        static float GetMaxVolume(const std::string& id) {
            return AudioResource::GetInstance().GetMaxVolume(id);
        }

        // ID文字列でカテゴリ取得（無ければSE）
        static AudioCategory GetCategory(const std::string& id) {
            return AudioResource::GetInstance().GetCategory(id);
        }

        // IDの存在チェック
        static bool Exists(const std::string& id) {
            return AudioResource::GetInstance().Exists(id);
        }

        // ID文字列で登録（重複はfalse）
        static bool LoadClip(const std::string& id, const std::string& path, AudioCategory cat = AudioCategory::SE, float maxVol = 1.0f, const std::string& groupID = "") {
            return AudioResource::GetInstance().Load(id, path, cat, maxVol, groupID);
        }

        // ID指定でアンロード
        static void Unload(const std::string& id) {
            AudioResource::GetInstance().Unload(id);
        }

        // すべてアンロード
        static void Clear() {
            AudioResource::GetInstance().Clear();
        }

        // グループ単位でロード
        static bool LoadGroup(const std::string& groupID) {
            return AudioResource::GetInstance().LoadGroup(groupID);
        }

        // グループ単位でアンロード
        static void UnloadGroup(const std::string& groupID) {
            AudioResource::GetInstance().UnloadGroup(groupID);
        }
    };


    class AudioResourceOperator {
    public:
        std::shared_ptr<AudioClip> operator[](const std::string& idOrCommand) const {
            if (idOrCommand.empty()) return nullptr;

            if (idOrCommand[0] == '#') {
                if (idOrCommand.size() < 3) return nullptr; // 簡易チェック

                char cmd = idOrCommand[1];
                std::string groupId = idOrCommand.substr(2);

                if (cmd == '!') {
                    AudioResourceShortcut::UnloadGroup(groupId);
                    return nullptr;
                }
                else if (cmd == '?') {
                    AudioResourceShortcut::LoadGroup(groupId);
                    return nullptr;
                }
                else {
                    return nullptr; // 未定義コマンド
                }
            }

            // グループ記号以外は従来通り
            if (idOrCommand[0] == '!') {
                std::string id = idOrCommand.substr(1);
                AudioResourceShortcut::Unload(id);
                return nullptr;
            }
            else if (idOrCommand[0] == '?') {
                std::string id = idOrCommand.substr(1);
                // ここで単体ロード処理（パス等は別管理が必要）
                // 例としては省略
                return AudioResourceShortcut::GetClip(id);
            }
            else {
                return AudioResourceShortcut::GetClip(idOrCommand);
            }
        }
    };

    /**
     * @brief AudioClip にアクセスできる操作クラス（[]演算子ベース）
     *
     * 例：
     * 
     * - Sounds["id"]       : 取得
     * 
     * - Sounds["?id"]      : 読み込み
     * 
     * - Sounds["!id"]      : アンロード
     * 
     * - Sounds["#?group"]  : グループ読み込み
     * 
     * - Sounds["#!group"]  : グループ解放
     */
    inline const AudioResourceOperator Sounds;


} // namespace GameEngine
