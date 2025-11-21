/*
    CsvReader.hpp

    :class
        - CsvReader

    作成日         : 2025/05/03
    最終変更日     : 2025/05/03
*/
#pragma once

#include "StreamReader.hpp"

#include <vector>
#include <string>
#include <sstream>

namespace System::IO {

    class CsvReader {
    public:
        // 1行のCSV文字列を分割してベクターに格納
        static std::vector<std::string> ParseCsvLine(const std::string& line, char delimiter = ',') {
            std::vector<std::string> result;
            std::string cell;
            std::istringstream ss(line);
            while (std::getline(ss, cell, delimiter)) {
                result.push_back(cell); // 各セルを格納
            }
            return result;
        }

        // CSVテキストを解析して二次元ベクターに格納
        static std::vector<std::vector<std::string>> ParseCsvText(const std::string& text, char delimiter = ',') {
            std::vector<std::vector<std::string>> result;
            std::istringstream ss(text);
            std::string line;

            while (std::getline(ss, line)) {
                result.push_back(ParseCsvLine(line, delimiter)); // 行ごとにパース
            }

            return result;
        }

        // ファイルからCSVを読み込んで二次元ベクターで返す
        static std::vector<std::vector<std::string>> ReadCsvFile(const std::string& path, char delimiter = ',') {
            std::vector<std::vector<std::string>> result;
            std::ifstream file(path); // std::ifstreamを使ってファイルを開く
            std::string line;

            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file: " + path); // エラーハンドリング
            }

            while (std::getline(file, line)) {
                result.push_back(ParseCsvLine(line, delimiter)); // 行ごとにパース
            }

            return result;
        }

    };
}