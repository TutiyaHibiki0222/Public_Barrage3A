/*
    FileStream.hpp

   Fclass
        - StreamReader -> Stream

    ì¬“ú         : 2025/04/02
    ÅI•ÏX“ú     : 2025/04/02
*/
#pragma once
#include "Stream.hpp"

namespace System {
    namespace IO {
        class StreamReader : public Stream {
            std::ifstream file;
        public:
            explicit StreamReader(const std::string& path) {
                file.open(path);
                if (!file) throw std::runtime_error("Cannot open file: " + path);
            }

            ~StreamReader() {
                Close();
            }

            bool EndOfStream() const {
                return file.eof();
            }

            bool ReadLine(std::string& outLine) {
                if (!file.is_open()) return false;
                return static_cast<bool>(std::getline(file, outLine));
            }

            void Close() override {
                if (file.is_open()) file.close();
            }
        };
    }// namespace IO
}// namespace System