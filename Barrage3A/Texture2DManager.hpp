/*
    Texture2DManager.hpp

    class
        - Texture2DManager

    作成日         : 2025/04/30
    最終変更日     : 2025/04/30
*/
#pragma once

#ifndef _TEXT_MGR
#define _TEXT_MGR

#include <unordered_map>
#include <map>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>

#include "SystemImage.hpp"
#include "SystemIO.h"
#include "Debug.hpp"
#include "Texture2D.h"
#include "SpriteFont.h"

namespace GameEngine {

    class Texture2DManager {
    private:
        // Node構造体の変更：shared_ptrを使って管理
        struct Node {
            std::unordered_map<std::string, std::shared_ptr<Node>> children;
            std::unordered_map<std::string, Texture2D> nextKeys;
            Node() = default;
        };
      
        struct SpriteMeta {
			std::string pathKey;
            std::string key;
            std::string fileCustomKey;
            std::string filePath;

            bool isDivided = false;
            int divX = 1, divY = 1;
            int width = 0, height = 0;

            bool hasOffset = false;
            int offsetX = 0, offsetY = 0;
            int paddingX = 0, paddingY = 0;
			bool excludeTransparent = false;
        };

        std::shared_ptr<Node> root;

        Texture2DManager() : root(std::make_shared<Node>()) {}

        // フォントは shared_ptr で管理
        std::unordered_map<std::string, std::shared_ptr<SpriteFont>> spriteFontMap;

    public:
        static Texture2DManager& GetInstance() {
            static Texture2DManager instance;
            return instance;
        }

        void LoadFileCsv(const std::string& file) {
            try {
                auto csvData = System::IO::CsvReader::ReadCsvFile(file);
                std::vector<SpriteMeta> metas;
                
                auto safe_stoi = [](const std::string& s) -> int {
                    try {
                        return s.empty() ? 0 : std::stoi(s);
                    }
                    catch (...) {
                        return 0;
                    }
                };

                auto safe_stob = [](const std::string& s) -> bool {
                    if (s.empty()) return false;
                    std::string lower = s;
                    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                    return (lower == "true" || lower == "1");
                    };

                // CSV行からSpriteMeta作成
                for (size_t i = 1; i < csvData.size(); ++i) {
                    const auto& row = csvData[i];
                    if (row.size() < 3) continue;

                    SpriteMeta meta;
                    meta.pathKey = row[0];
                    const std::string& rawKey = row[1];
                    auto pos = rawKey.find(':');
                    if (pos != std::string::npos) {
                        meta.fileCustomKey = rawKey.substr(pos + 1);  // 例: "Border"
                        meta.key = rawKey.substr(0, pos);             // 例: "PlayerSlow"
                    }
                    else {
                        meta.key = rawKey;
                        meta.fileCustomKey.clear(); // なければ空で扱う
                    }
                    meta.filePath = row[2];
                    meta.isDivided = safe_stob(row[3]);
                    meta.divX = safe_stoi(row[4]);
                    meta.divY = safe_stoi(row[5]);
                    meta.width = safe_stoi(row[6]);
                    meta.height = safe_stoi(row[7]);
                    meta.hasOffset= safe_stob(row[8]);
                    meta.offsetX  = (row.size() > 9)  ? safe_stoi(row[9])  : 0;
                    meta.offsetY  = (row.size() > 10) ? safe_stoi(row[10]) : 0;
                    meta.paddingX = (row.size() > 11) ? safe_stoi(row[11]) : 0;
                    meta.paddingY = (row.size() > 12) ? safe_stoi(row[12]) : 0;
					meta.excludeTransparent = (row.size() > 13) ? safe_stob(row[13]) : false;
                    metas.push_back(meta);
                }

                std::vector<std::string> errorLogs;

                // pathKey + key に対して Texture2D をまとめて管理.
                std::map<std::pair<std::string, std::string>, Texture2D> textureMap;

                for (const auto& meta : metas) {
                    auto mapKey = std::make_pair(meta.pathKey, meta.key);
                    Texture2D& texture = textureMap[mapKey]; // 存在しなければ自動生成

                    std::vector<int> images;
                    int image = 0;

                    if (meta.isDivided) {
                        if (meta.hasOffset) {
                            try {
                                System::ImageHelper::ImageCropper::LoadOffsetPaddingFile(
                                    images,
                                    meta.filePath.c_str(),
                                    meta.divX,      meta.divY,
                                    meta.offsetX,   meta.offsetY,
                                    meta.paddingX,  meta.paddingY,
                                    meta.excludeTransparent
                                );
                            }
                            catch (const std::exception& e) {
                                errorLogs.push_back("OffsetPaddingFile: " + meta.filePath + " - " + e.what());
                                continue;
                            }
                        }
                        else {
                            try {
                                System::ImageHelper::ImageCropper::LoadDivImageFile(
                                    images,
                                    meta.filePath.c_str(),
                                    meta.divX * meta.divY,
                                    meta.divX, meta.divY,
                                    meta.width, meta.height
                                );
                            }
                            catch (const std::exception& e) {
                                errorLogs.push_back("DivImageFile: " + meta.filePath + " - " + e.what());
                                continue;
                            }
                        }

                        std::string imgKey = !meta.fileCustomKey.empty()
                            ? meta.fileCustomKey
                            : System::IO::Path::GetFileNameWithoutExtension(meta.filePath);
                        texture.AddSpritesFromHandles(imgKey, images, meta.width, meta.height);
                    }
                    else {
                        try {
                            System::ImageHelper::ImageCropper::LoadImageFile(image, meta.filePath.c_str());
                        }
                        catch (const std::exception& e) {
                            errorLogs.push_back("ImageFile: " + meta.filePath + " - " + e.what());
                            continue;
                        }

                        std::string imgKey = !meta.fileCustomKey.empty()
                            ? meta.fileCustomKey
                            : System::IO::Path::GetFileNameWithoutExtension(meta.filePath);
                        texture.AddImageData(imgKey, image);
                    }
                }

                // Texture2Dを一括登録
                for (const auto& [keyPair, texture] : textureMap) {
                    AddTexture(keyPair.first, keyPair.second, texture);
                }

                // エラーログ出力
                if (!errorLogs.empty()) {
                    std::cout << "=== LoadFileCsv エラー一覧 ===" << std::endl;
                    for (const auto& log : errorLogs) {
                        GameEngine::Debug::ErrorLog(log);
                    }
                }
            }
            catch (const std::exception e) {
                GameEngine::Debug::ErrorLog(e.what());
            }
        }

        // テクスチャの追加
        void AddTexture(const std::string& path, const std::string& key, const Texture2D& texture) {
            auto current = root;
            std::istringstream iss(path);
            std::string token;

            // path を分割して階層をたどり、無ければ新しいノードを作成
            while (std::getline(iss, token, '/')) {
                if (current->children.find(token) == current->children.end()) {
                    current->children[token] = std::make_shared<Node>();
                }
                current = current->children[token]; // 共有ポインタで次のノードへ
            }

            // 最後に nextKeys にテクスチャを追加
            current->nextKeys[key] = texture;
        }

        // テクスチャを取得
        std::shared_ptr<Texture2D> GetTexture2D(const std::string& path, const std::string& key) const {
            auto current = root;
            std::istringstream iss(path);
            std::string token;

            while (std::getline(iss, token, '/')) {
                auto it = current->children.find(token);
                if (it == current->children.end()) return nullptr;
                current = it->second;
            }

            auto it = current->nextKeys.find(key);
            if (it != current->nextKeys.end()) {
                return std::make_shared<Texture2D>(it->second);
            }

            return nullptr;
        }

        std::vector<std::shared_ptr<Texture2D>> GetAllTextures(const std::string& path) const {
            std::vector<std::shared_ptr<Texture2D>> result;

            // 指定された path のノードを探す
            auto current = root;
            std::istringstream iss(path);
            std::string token;
            while (std::getline(iss, token, '/')) {
                auto it = current->children.find(token);
                if (it == current->children.end()) return result; // 見つからなければ空で返す
                current = it->second;
            }

            // 再帰的に収集する
            std::function<void(const std::shared_ptr<Node>&)> CollectAll;
            CollectAll = [&](const std::shared_ptr<Node>& node) {
                // 現在のノードにあるテクスチャを追加
                for (const auto& pair : node->nextKeys) {
                    result.push_back(std::make_shared<Texture2D>(pair.second));
                }
                // 子ノードも再帰的に処理
                for (const auto& child : node->children) {
                    CollectAll(child.second);
                }
                };

            CollectAll(current);

            return result;
        }

        std::vector<std::string> GetAllNodeKey(const std::string& path) const {
            std::vector<std::string> result;
            auto current = root;
            std::istringstream iss(path);
            std::string token;

            // 指定パスまで降りる
            while (std::getline(iss, token, '/')) {
                auto it = current->children.find(token);
                if (it == current->children.end()) return result;
                current = it->second;
            }

            // 相対パスでノード名を再帰的に収集
            std::function<void(const std::shared_ptr<Node>&, const std::string&)> dfs;
            dfs = [&](const std::shared_ptr<Node>& node, const std::string& prefix) {
                for (const auto& [name, child] : node->children) {
                    std::string newPath = prefix.empty() ? name : prefix + "/" + name;
                    result.push_back(newPath);
                    dfs(child, newPath);
                }
                };

            dfs(current, "");

            return result;
        }

        std::vector<std::shared_ptr<Node>> GetAllNode(const std::string& path) const {
            std::vector<std::shared_ptr<Node>> result;
            auto current = root;
            std::istringstream iss(path);
            std::string token;

            // 指定パスまで降りる
            while (std::getline(iss, token, '/')) {
                auto it = current->children.find(token);
                if (it == current->children.end()) return result;  // パスが無効な場合は空の結果を返す
                current = it->second;
            }

            // 相対パスでノードを再帰的に収集
            std::function<void(const std::shared_ptr<Node>&)> dfs;
            dfs = [&](const std::shared_ptr<Node>& node) {
                // 現在のノードを結果に追加
                result.push_back(node);

                // 子ノードを再帰的に処理
                for (const auto& [name, child] : node->children) {
                    dfs(child);
                }
                };

            dfs(current);  // 最初は指定されたパスのノードから始める

            return result;
        }

        /**
        * @brief フォントをマネージャに登録（重複登録を防止）
        * @param key    登録用キー
        * @param font   共有ポインタで渡す SpriteFont
        * @return 登録済みフォントの shared_ptr
        */
        std::shared_ptr<SpriteFont> AddSpriteFont(const std::string& key,const std::shared_ptr<SpriteFont>& font) {
            auto it = spriteFontMap.find(key);
            if (it == spriteFontMap.end()) {
                spriteFontMap[key] = font;
                return font;
            }
            return it->second;
        }

        /**
         * @brief 登録済みフォント取得
         * @param key 登録用キー
         * @return 存在すれば SpriteFont を返す (shared_ptr)、なければ nullptr
         */
        std::shared_ptr<SpriteFont> GetSpriteFont(const std::string& key) const {
            auto it = spriteFontMap.find(key);
            if (it != spriteFontMap.end()) {
                return it->second;
            }
            return nullptr;
        }

        // コピー禁止
        Texture2DManager(const Texture2DManager&) = delete;
        Texture2DManager& operator=(const Texture2DManager&) = delete;

    };
}
#endif // _TEXT_MGR