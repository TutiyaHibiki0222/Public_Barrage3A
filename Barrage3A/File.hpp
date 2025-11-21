/*
    File.hpp

    ÅFclass 
        - File 21Å´
        - FileInfo 81Å´
    çÏê¨ì˙         : 2025/04/02
    ç≈èIïœçXì˙     : 2025/04/02
*/
#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
namespace fs = std::filesystem;

namespace System {
    namespace IO {

        class File {
        public:
            static bool Exists(const std::string& path) {
                return fs::exists(path) && fs::is_regular_file(path);
            }

            static std::string ReadAllText(const std::string& path) {
                std::ifstream file(path);
                if (!file) throw std::runtime_error("File not found: " + path);
                return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            }

            static void WriteAllText(const std::string& path, const std::string& content) {
                std::ofstream file(path);
                if (!file) throw std::runtime_error("Cannot write to file: " + path);
                file << content;
            }

            static void AppendAllText(const std::string& path, const std::string& content) {
                std::ofstream file(path, std::ios::app);
                if (!file) throw std::runtime_error("Cannot append to file: " + path);
                file << content;
            }

            static void Delete(const std::string& path) {
                if (!fs::remove(path)) throw std::runtime_error("File not found or could not be deleted: " + path);
            }

            static void Copy(const std::string& src, const std::string& dest, bool overwrite = false) {
                if (!overwrite && fs::exists(dest)) throw std::runtime_error("Destination file already exists: " + dest);
                fs::copy(src, dest, fs::copy_options::overwrite_existing);
            }

            static void Move(const std::string& src, const std::string& dest) {
                fs::rename(src, dest);
            }

            static std::vector<std::string> ReadAllLines(const std::string& path) {
                std::ifstream file(path);
                if (!file) throw std::runtime_error("File not found: " + path);

                std::vector<std::string> lines;
                std::string line;
                while (std::getline(file, line)) {
                    lines.push_back(line);
                }
                return lines;
            }

            static void WriteAllLines(const std::string& path, const std::vector<std::string>& lines) {
                std::ofstream file(path);
                if (!file) throw std::runtime_error("Cannot write to file: " + path);

                for (const auto& line : lines) {
                    file << line << "\n";
                }
            }

            static void WriteAllBytes(const std::string& path, const std::string& data) {
                std::ofstream file(path, std::ios::binary);
                if (!file) throw std::runtime_error("Cannot write binary file: " + path);
                file.write(data.data(), data.size());
            }

            static std::string ReadAllBytes(const std::string& path) {
                std::ifstream file(path, std::ios::binary);
                if (!file) throw std::runtime_error("Cannot read binary file: " + path);
                std::ostringstream oss;
                oss << file.rdbuf();
                return oss.str();
            }


        };

        class FileInfo {
            fs::path filePath;
        public:
            explicit FileInfo(const std::string& path) : filePath(path) {}

            std::string GetFullName() const {
                return fs::absolute(filePath).string();
            }

            std::string GetFileName() const {
                return filePath.filename().string();
            }

            std::string GetExtension() const {
                return filePath.extension().string();
            }

            uintmax_t GetSize() const {
                return fs::file_size(filePath);
            }

            bool Exists() const {
                return fs::exists(filePath);
            }

            void Delete() {
                if (!fs::remove(filePath)) throw std::runtime_error("Could not delete file: " + filePath.string());
            }

            void MoveTo(const std::string& dest) {
                fs::rename(filePath, dest);
                filePath = dest;
            }

            void CopyTo(const std::string& dest, bool overwrite = false) {
                if (!overwrite && fs::exists(dest)) throw std::runtime_error("Destination file already exists: " + dest);
                fs::copy(filePath, dest, fs::copy_options::overwrite_existing);
            }

            std::time_t GetLastWriteTime() const {
                auto ftime = fs::last_write_time(filePath);
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
                );
                return std::chrono::system_clock::to_time_t(sctp);
            }

            std::time_t GetCreationTime() const {
                struct stat fileStat;
                if (stat(filePath.string().c_str(), &fileStat) != 0) {
                    throw std::runtime_error("Could not get file creation time: " + filePath.string());
                }
                return fileStat.st_ctime;
            }
        };

    }// namespace IO
}// namespace System


