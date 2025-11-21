#include <unordered_map>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "AudioClip.h"
#include "Mathf.h"
namespace GameEngine {


    // ----------- 列挙型・構造体例 -----------------
    enum class AudioCategory {
        BGM,
        SE,
        VOICE
    };

    struct AudioResourceEntry {
        std::shared_ptr<AudioClip> clip = nullptr;
        AudioCategory category	= AudioCategory::BGM;
        float maxVolume			= 1.0f;
        std::string groupId		= "";
    };

    struct AudioEntry { // CSVに記録される情報だけ持つ構造体
        std::string id		= "";
        std::string path	= "";
        AudioCategory category = AudioCategory::BGM;
        float maxVolume		= 1.0f;
        std::string groupId = "";
    };

    // ----------- AudioResource 実装 -----------------
    class AudioResource {
    private:
        std::unordered_map<std::string, AudioEntry> allEntries;       // CSVに登録された全素材情報（id→AudioEntry）
        std::unordered_map<std::string, AudioResourceEntry> clipMap;  // 実際にロード済みの素材（id→AudioResourceEntry）

        AudioResource() {}

    public:
        static AudioResource& GetInstance() {
            static AudioResource instance;
            return instance;
        }

        // -------------------
        // CSVを読み込んでallEntriesに登録（メモリ読み込みはしない）
        bool LoadSetFromCSV(const std::string& csvPath) {
            std::ifstream file(csvPath);
            if (!file.is_open()) {
                std::cerr << "Failed to open CSV file: " << csvPath << std::endl;
                return false;
            }

            allEntries.clear();

            std::string line;
            bool firstLine = true;
            while (std::getline(file, line)) {
                if (firstLine) { firstLine = false; continue; } // ヘッダー行スキップ

                std::stringstream ss(line);
                std::string id, path, categoryStr, maxVolStr, groupId;

                std::getline(ss, id, ',');
                std::getline(ss, path, ',');
                std::getline(ss, categoryStr, ',');
                std::getline(ss, maxVolStr, ',');
                std::getline(ss, groupId, ',');

                if (id.empty() || path.empty()) continue; // 不正行はスキップ

                AudioCategory category = ToAudioCategory(categoryStr);
                float maxVol = 1.0f;
                try {
                    maxVol = std::stof(maxVolStr);
                }
                catch (...) { maxVol = 1.0f; }
                maxVol = Mathf::Clamp01(maxVol);
                allEntries[id] = AudioEntry{ id, path, category, maxVol, groupId };
            }
            return true;
        }

        // -------------------
        // CSV読み込み＋全素材をロードする（一括ロード）
        bool LoadFromCSV(const std::string& csvPath) {
            if (!LoadSetFromCSV(csvPath)) return false;

            bool allSuccess = true;
            for (auto& [id, entry] : allEntries) {
                if (!Load(id, entry.path, entry.category, entry.maxVolume, entry.groupId)) {
                    allSuccess = false;
                }
            }
            return allSuccess;
        }

        // -------------------
        // グループ単位で素材をまとめてロード
        bool LoadGroup(const std::string& groupName) {
            bool allSuccess = true;
            for (auto& [id, entry] : allEntries) {
                if (entry.groupId == groupName) {
                    if (!Load(id, entry.path, entry.category, entry.maxVolume, entry.groupId)) {
                        allSuccess = false;
                    }
                }
            }
            return allSuccess;
        }

        // -------------------
        // グループ単位で素材をアンロード
        void UnloadGroup(const std::string& groupName) {
            for (auto it = clipMap.begin(); it != clipMap.end();) {
                if (it->second.groupId == groupName) {
                    it = clipMap.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        // -------------------
        // 個別ロード（内部使用）
        bool Load(const std::string& id, const std::string& path, AudioCategory cat, float maxVol, const std::string& groupId = "") {
            if (clipMap.find(id) != clipMap.end()) return false; // すでにロード済み

            auto clip = std::make_shared<AudioClip>(path);
            if (!clip->GetIsLoaded()) return false;

            AudioResourceEntry entry;
            entry.clip = clip;
            entry.clip->SetMaxVolume(maxVol);
            entry.maxVolume = maxVol;
            entry.category = cat;
            entry.groupId = groupId;

            clipMap[id] = entry;
            return true;
        }

        // --------------- ユーティリティ -------------------
        AudioCategory ToAudioCategory(const std::string& str) {
            if (str == "BGM") return AudioCategory::BGM;
            if (str == "SE") return AudioCategory::SE;
            if (str == "VOICE") return AudioCategory::VOICE;
            return AudioCategory::SE; // デフォルト
        }

        std::shared_ptr<AudioClip> GetClip(const std::string& id) const {
            auto it = clipMap.find(id);
            if (it != clipMap.end()) return it->second.clip;
            return nullptr;
        }

        float GetMaxVolume(const std::string& id) {
            auto it = clipMap.find(id);
            if (it != clipMap.end()) return it->second.maxVolume;
            return 1.0f;
        }

        AudioCategory GetCategory(const std::string& id) {
            auto it = clipMap.find(id);
            if (it != clipMap.end()) return it->second.category;
            return AudioCategory::SE;
        }

        bool Exists(const std::string& id) {
            return clipMap.find(id) != clipMap.end();
        }

        void Unload(const std::string& id) {
            clipMap.erase(id);
        }

        void Clear() {
            clipMap.clear();
            allEntries.clear();
        }
    };
}
