/*
    StreamWriter.hpp

   Fclass
        - StreamWriter -> Stream

    ì¬“ú         : 2025/04/02
    ÅI•ÏX“ú     : 2025/04/02
*/
#pragma once
#include "Stream.hpp"

namespace System {
    namespace IO {
        class StreamWriter : public Stream {
            std::ofstream file;
        public:
            explicit StreamWriter(const std::string& path, bool append = false) {
                file.open(path, append ? std::ios::app : std::ios::out);
                if (!file) throw std::runtime_error("Cannot open file: " + path);
            }

            ~StreamWriter() {
                Close();
            }

            void WriteLine(const std::string& text) {
                file << text << '\n';
            }

            void Close() override {
                if (file.is_open()) file.close();
            }
        };
    }// namespace IO
}// namespace System