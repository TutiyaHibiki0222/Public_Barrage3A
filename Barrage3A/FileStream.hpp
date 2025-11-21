/*
    FileStream.hpp

   ÅFclass
        - FileStream -> Stream

    çÏê¨ì˙         : 2025/04/02
    ç≈èIïœçXì˙     : 2025/04/02
*/
#pragma once
#include "Stream.hpp"

namespace System {
    namespace IO {
        class FileStream : public Stream {
            std::fstream file;
        public:
            FileStream(const std::string& path, std::ios::openmode mode) {
                file.open(path, mode);
                if (!file) throw std::runtime_error("Cannot open file: " + path);
            }

            ~FileStream() {
                Close();
            }

            void Write(const std::string& content) {
                file << content;
            }

            void Write(const void* data, std::size_t size) {
                file.write(reinterpret_cast<const char*>(data), size);
                if (!file) {
                    throw std::runtime_error("Binary write failed.");
                }
            }

            std::string Read() {
                return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            }

            void ReadRaw(void* buffer, std::size_t size) {
                file.read(reinterpret_cast<char*>(buffer), size);
                if (!file) {
                    throw std::runtime_error("Binary read failed or incomplete.");
                }
            }

            bool IsOpen() const {
                return file.is_open();
            }

            void Close() override {
                if (file.is_open()) file.close();
            }
        };
    }
}