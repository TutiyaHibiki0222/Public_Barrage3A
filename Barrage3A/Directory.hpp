/*
    Directory.hpp

    :class
        - Directory
        - DirectoryInfo

    作成日         : 2025/04/02
    最終変更日     : 2025/04/02
*/
#pragma once
#ifdef _WIN32
  #include <Windows.h>
  #undef CreateDirectory   // ← これで Windows マクロを消す
  #undef CopyFile          // （もし他にも衝突するものがあれば同様に）
#endif


#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

namespace System {
    namespace IO {

        class Directory {
        public:
            static bool Exists(const std::string& path) {
                return fs::exists(path) && fs::is_directory(path);
            }

            static void CreateDirectory(const std::string& path) {
                fs::create_directories(path);
            }

            static void Delete(const std::string& path, bool recursive = false) {
                if (recursive) {
                    fs::remove_all(path);
                }
                else {
                    fs::remove(path);
                }
            }

            static std::vector<std::string> GetFiles(const std::string& path) {
                std::vector<std::string> files;
                for (const auto& entry : fs::directory_iterator(path)) {
                    if (fs::is_regular_file(entry.path())) {
                        files.push_back(entry.path().string());
                    }
                }
                return files;
            }

            static std::vector<std::string> GetDirectories(const std::string& path) {
                std::vector<std::string> directories;
                for (const auto& entry : fs::directory_iterator(path)) {
                    if (fs::is_directory(entry.path())) {
                        directories.push_back(entry.path().string());
                    }
                }
                return directories;
            }
        };

        class DirectoryInfo {
            fs::path dirPath;
        public:
            explicit DirectoryInfo(const std::string& path) : dirPath(path) {}

            std::string GetFullName() const {
                return fs::absolute(dirPath).string();
            }

            std::string GetDirectoryName() const {
                return dirPath.filename().string();
            }

            uintmax_t GetSize() const {
                uintmax_t size = 0;
                for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
                    if (fs::is_regular_file(entry.path())) {
                        size += fs::file_size(entry.path());
                    }
                }
                return size;
            }

            std::time_t GetLastWriteTime() const {
                auto ftime = fs::last_write_time(dirPath);
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
                );
                return std::chrono::system_clock::to_time_t(sctp);
            }
        };
    }// namespace IO
}// namespace System

