/*
    String.hpp

    :class
        - String

    作成日         : 2025/04/08
    最終変更日     : 2025/04/08
*/
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace System {

    /// <summary>
    /// C++ 標準機能 + C#機能
    /// </summary>
    class String {
    public:
        std::string value;
        // コンストラクタ.
        String() = default;
        String(const std::string& str) : value(str) {}
        String(const char* str) : value(str) {}

        bool Contains(const std::string& substr) const {
            return value.find(substr) != std::string::npos;
        }

        bool StartsWith(const std::string& prefix) const {
            return value.find(prefix) == 0;
        }

        String Replace(const std::string& from, const std::string& to) const {
            std::string result = value;
            size_t start_pos = 0;
            while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
                result.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
            return result;
        }

        std::vector<String> Split(char delimiter) const {
            std::vector<String> tokens;
            std::stringstream ss(value);
            std::string item;
            while (std::getline(ss, item, delimiter)) {
                tokens.emplace_back(item);
            }
            return tokens;
        }

        String ToLower() const {
            std::string result = value;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }

        operator std::string() const { return value; }

        // ostream 出力
        friend std::ostream& operator<<(std::ostream& os, const String& s) {
            return os << s.value;
        }
    };
}