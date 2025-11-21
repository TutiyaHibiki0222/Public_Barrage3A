#pragma once

#include <string>
#include <sstream>
#include <memory>
#include "Texture2DManager.hpp"
#include "Texture2D.h"
#include "Debug.hpp"

namespace GameEngine {

    /**
    * @brief Texture2D と Sprite をフルパス文字列から簡単に取得するためのユーティリティクラス
    *
    * フルパスは "Effects/001/Leaf/Leaf01" のような形式を想定しています。
    */
    class Texture2DShortcut {
    public:
        /**
        * @brief fullPath形式の文字列からTexture2Dを取得する
        * 
        * @param fullPath 例: "Effects/Leaf/Leaf01"（この場合、"Effects/Leaf"がpath、"Leaf"がkey）
        * 
        * @return 見つかったTexture2Dのshared_ptr。失敗時はnullptr。
        */
        static std::shared_ptr<Texture2D> Get(const std::string& fullPath) {
            std::string path, key;
            if (!SplitPathForTexture(fullPath, path, key)) {
                Debug::ErrorLog("Get: Texture2D用パス形式が不正です: " + fullPath);
                return nullptr;
            }

            auto texture = Texture2DManager::GetInstance().GetTexture2D(path, key);
            if (!texture) {
                Debug::ErrorLog("Get: Texture2D が見つかりませんでした。パス: " + path + ", キー: " + key);
                return nullptr;
            }

            return texture;
        }


        /**
        * @brief フルパス文字列から Sprite を取得する
        *
        * フルパスはパス、キー、スプライト名を含む形式である必要があります。
        * 
        * 例: "Effects/001/Leaf/Leaf01"
        *
        * @param fullPath スプライトを特定するためのパス文字列
        * @return std::shared_ptr<Sprite> 有効ならSpriteのshared_ptr、失敗ならnullptr
        */
        static std::shared_ptr<Sprite> GetSprite(const std::string& fullPath) {
            std::string texPath, key, spriteName;
            if (!SplitPath(fullPath, texPath, key, spriteName)) {
                Debug::ErrorLog("GetSprite: パスの形式が不正です -> " + fullPath);
                return nullptr;
            }

            auto texture = Texture2DManager::GetInstance().GetTexture2D(texPath, key);
            if (!texture) {
                Debug::ErrorLog("GetSprite: Texture2D が見つかりません。パス: " + texPath + ", キー: " + key);
                return nullptr;
            }

            auto sprite = texture->GetTextureData(spriteName);
            if (!sprite) {
                Debug::ErrorLog("GetSprite: スプライトが見つかりません -> " + spriteName);
                return nullptr;
            }

            return sprite;
        }

        /**
        * @brief フルパスから Texture2D を取得し、その中のスプライト群をプレフィックスでフィルタリングして取得する
        *
        * @param fullPath テクスチャのパスとキーを含む文字列
        * @param prefix スプライト名の接頭辞
        * @return std::vector<std::shared_ptr<Sprite>> 該当スプライトのshared_ptr配列。見つからなければ空配列。
        */
        static std::vector<std::shared_ptr<Sprite>> GetSpritesByPrefix(const std::string& fullPath, const std::string& prefix) {
            std::string texPath, key, dummy;
            if (!SplitPath(fullPath, texPath, key, dummy)) {
                Debug::ErrorLog("GetSpritesByPrefix: パスの形式が不正です -> " + fullPath);
                return {};
            }

            auto texture = Texture2DManager::GetInstance().GetTexture2D(texPath, key);
            if (!texture) {
                Debug::ErrorLog("GetSpritesByPrefix: Texture2D が見つかりません。パス: " + texPath + ", キー: " + key);
                return {};
            }

            return texture->GetTexturesByPrefix(prefix);
        }
        /**
        * @brief フルパスから Texture2D を取得し、その中のスプライト群をプレフィックスと範囲指定でフィルタリングして取得する
        *
        * @param fullPath テクスチャのパスとキーを含む文字列
        * @param prefix スプライト名の接頭辞
        * @param start 範囲の開始インデックス（inclusive）
        * @param end 範囲の終了インデックス（inclusive）
        * @return std::vector<std::shared_ptr<Sprite>> 該当スプライトのshared_ptr配列。見つからなければ空配列。
         */
        static std::vector<std::shared_ptr<Sprite>> GetSpritesByRange(const std::string& fullPath, const std::string& prefix, int start, int end) {
            std::string texPath, key, dummy;
            if (!SplitPath(fullPath, texPath, key, dummy)) {
                Debug::ErrorLog("GetSpritesByRange: パスの形式が不正です -> " + fullPath);
                return {};
            }

            auto texture = Texture2DManager::GetInstance().GetTexture2D(texPath, key);
            if (!texture) {
                Debug::ErrorLog("GetSpritesByRange: Texture2D が見つかりません。パス: " + texPath + ", キー: " + key);
                return {};
            }

            return texture->GetTexturesByPrefixRange(prefix, start, end);
        }
    private:
        /**
        * @brief フルパスからパス、キー、スプライト名に分解する
        *
        * フォーマット例: "Effects/001/Leaf/Leaf01" または "Effects/001/Leaf:Leaf01"
        *
        * @param fullPath 分解対象のフルパス文字列
        * @param outPath 分解後のパス部分を返す（例: "Effects/001"）
        * @param outKey 分解後のキー部分を返す（例: "Leaf"）
        * @param outSprite 分解後のスプライト名部分を返す（例: "Leaf01"）
        * @return true 分解成功
        * @return false 分解失敗（形式不正など）
        */
        static bool SplitPath(const std::string& fullPath, std::string& outPath, std::string& outKey, std::string& outSprite) {
            size_t colonPos = fullPath.find(':');
            if (colonPos != std::string::npos) {
                // コロンがある形式: "Effects/001/Leaf:Leaf01"
                outSprite = fullPath.substr(colonPos + 1);
                std::string left = fullPath.substr(0, colonPos);

                std::istringstream iss(left);
                std::vector<std::string> tokens;
                std::string token;

                while (std::getline(iss, token, '/')) {
                    tokens.push_back(token);
                }

                if (tokens.size() < 1) return false;

                outKey = tokens.back();
                tokens.pop_back();

                outPath.clear();
                for (size_t i = 0; i < tokens.size(); ++i) {
                    outPath += tokens[i];
                    if (i != tokens.size() - 1) outPath += "/";
                }

                return true;
            }

            // コロンがない → 通常パス "Effects/001/Leaf/Leaf01"
            std::istringstream iss(fullPath);
            std::vector<std::string> tokens;
            std::string token;

            while (std::getline(iss, token, '/')) {
                tokens.push_back(token);
            }

            if (tokens.size() < 3) return false;

            outSprite = tokens.back(); tokens.pop_back();
            outKey = tokens.back(); tokens.pop_back();

            outPath.clear();
            for (size_t i = 0; i < tokens.size(); ++i) {
                outPath += tokens[i];
                if (i != tokens.size() - 1) outPath += "/";
            }

            return true;
        }

        /**
        * @brief フルパスからパスとキーに分解する（Texture2D取得用）
        *
        * コロンあり形式の場合は、コロン前の部分のみ使用し、スプライト名は無視する
        *
        * @param fullPath 分解対象のフルパス文字列
        * @param outPath 分解後のパス部分を返す
        * @param outKey 分解後のキー部分を返す
        * @return true 分解成功
        * @return false 分解失敗（形式不正など）
        */
        static bool SplitPathForTexture(const std::string& fullPath, std::string& outPath, std::string& outKey) {
            size_t colonPos = fullPath.find(':');
            if (colonPos != std::string::npos) {
                // コロン形式はここでは想定しない、false返すかコロン前だけ使うか
                std::string left = fullPath.substr(0, colonPos);

                std::istringstream iss(left);
                std::vector<std::string> tokens;
                std::string token;

                while (std::getline(iss, token, '/')) {
                    tokens.push_back(token);
                }

                if (tokens.empty()) return false;

                outKey = tokens.back();
                tokens.pop_back();

                outPath.clear();
                for (size_t i = 0; i < tokens.size(); ++i) {
                    outPath += tokens[i];
                    if (i != tokens.size() - 1) outPath += "/";
                }

                return true;
            }

            // コロンなしの形式: "Effects/001/Leaf"
            std::istringstream iss(fullPath);
            std::vector<std::string> tokens;
            std::string token;

            while (std::getline(iss, token, '/')) {
                tokens.push_back(token);
            }

            if (tokens.size() < 2) return false;

            outKey = tokens.back();
            tokens.pop_back();

            outPath.clear();
            for (size_t i = 0; i < tokens.size(); ++i) {
                outPath += tokens[i];
                if (i != tokens.size() - 1) outPath += "/";
            }

            return true;
        }
    };

    /**
    * @brief TexShortcut 名前空間の簡易エイリアス関数群
    */
    namespace TexShortcut {

        /**
        * @brief フルパスから Texture2D を取得する
        * @param fullPath パス文字列
        * @return Texture2Dのshared_ptr。失敗時はnullptr
        */
        inline std::shared_ptr<Texture2D> Get(const std::string& fullPath) {
            return Texture2DShortcut::Get(fullPath);
        }

        /**
        * @brief フルパスから Sprite を取得する
        * @param fullPath パス文字列
        * @return Spriteのshared_ptr。失敗時はnullptr
        */
        inline std::shared_ptr<Sprite> GetSprite(const std::string& fullPath) {
            return Texture2DShortcut::GetSprite(fullPath);
        }

        /**
        * @brief フルパスからプレフィックスでフィルタリングしたスプライト群を取得する
        * @param fullPath パス文字列
        * @param prefix プレフィックス文字列
        * @return スプライトのshared_ptr配列。失敗時は空配列
        */
        inline std::vector<std::shared_ptr<Sprite>> GetSpritesByPrefix(const std::string& fullPath, const std::string& prefix) {
            return Texture2DShortcut::GetSpritesByPrefix(fullPath, prefix);
        }
        
        /**
        * @brief フルパスからプレフィックスと範囲指定でフィルタリングしたスプライト群を取得する
        * @param fullPath パス文字列
        * @param prefix プレフィックス文字列
        * @param start 範囲開始（inclusive）
        * @param end 範囲終了（inclusive）
        * @return スプライトのshared_ptr配列。失敗時は空配列
        */
        inline std::vector<std::shared_ptr<Sprite>> GetSpritesByRange(const std::string& fullPath, const std::string& prefix, int start, int end) {
            return Texture2DShortcut::GetSpritesByRange(fullPath, prefix, start, end);
        }
    }

    /**
    * @brief TexOperator クラスは、Tex 演算子風のアクセスを提供する
    *
    * operator[] でスプライト取得、operator() で Texture2D取得が可能
    */
    class TexOperator {
    public:
        /**
        * @brief operator[] で Sprite を取得する
        * @param fullPath スプライトのフルパス文字列
        * @return Spriteのshared_ptr。失敗時はnullptr
        */
        std::shared_ptr<Sprite> operator[](const std::string& fullPath) const {
            return Texture2DShortcut::GetSprite(fullPath);
        }

        /**
        * @brief operator() で Texture2D を取得する
        * @param fullPath Texture2Dのフルパス文字列
        * @return Texture2Dのshared_ptr。失敗時はnullptr
        */
        std::shared_ptr<Texture2D> operator()(const std::string& fullPath) const {
            return Texture2DShortcut::Get(fullPath);
        }
    };

    /**
    * @brief グローバルで使いやすいTex演算子. operator
    * @param ("path") or ["path"]
    * @return () Texture2Dのshared_ptr。失敗時はnullptr
    * @return [] Spriteのshared_ptr。失敗時はnullptr
    */
    inline const TexOperator Tex;
}
