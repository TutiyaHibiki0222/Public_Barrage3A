/*
    Path.hpp

   ：class
        - Path

    作成日         : 2025/04/02
    最終変更日     : 2025/04/02
*/
#pragma once

#include <filesystem>
#include <string>

namespace System {
    namespace IO {

        class Path {
        public:
            // 階層を制作.
            template<typename... Paths>
            static std::string Combine(const Paths&... paths) {
                std::filesystem::path result;
                (result /= ... /= paths);
                return result.string();
            }
            // ファイル名を含まないディレクトリパスを取得
            static std::string GetDirectoryName(const std::string& path) {
                return std::filesystem::path(path).parent_path().string();
            }

            // 拡張子を取得
            static std::string GetExtension(const std::string& path) {
                return std::filesystem::path(path).extension().string();
            }

            // ファイル名を取得（拡張子を含む）
            static std::string GetFileName(const std::string& path) {
                return std::filesystem::path(path).filename().string();
            }

            // ファイル名から拡張子を除いた部分を取得
            static std::string GetFileNameWithoutExtension(const std::string& path) {
                return std::filesystem::path(path).stem().string();
            }

            // パスを結合（引数の順に）
            static std::string Combine(const std::string& path1, const std::string& path2) {
                return (std::filesystem::path(path1) / std::filesystem::path(path2)).string();
            }

            // 絶対パスを取得
            static std::string GetFullPath(const std::string& path) {
                return std::filesystem::absolute(path).string();
            }

            // パスが存在するかどうかをチェック
            static bool Exists(const std::string& path) {
                return std::filesystem::exists(path);
            }

            // パスがディレクトリかどうかをチェック
            static bool IsDirectory(const std::string& path) {
                return std::filesystem::is_directory(path);
            }

            // パスがファイルかどうかをチェック
            static bool IsFile(const std::string& path) {
                return std::filesystem::is_regular_file(path);
            }
        };
    }// namespace IO
}// namespace System