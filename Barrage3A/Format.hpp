/*
    Format.hpp

    class-

    作成日         : 2025/03/26
    最終変更日     : 2025/03/27
*/

#pragma once

#include <iomanip>
#include <sstream>
#include <vector>
#include <regex>
#include <bitset>
#include <set>
#include <type_traits>

namespace System {
    namespace Utility {

        class Format {
        public:
            /// <summary>
            /// <para>任意の型の値を文字列に変換する関数。</para>
            /// <para>ポインタ型の場合、`nullptr` の場合は "null" を、</para>
            /// <para>クラス型の場合は型名を出力します。</para>
            /// </summary>
            /// <typeparam name="T"> 任意の型（ポインタ型、クラス型など）</typeparam>
            /// <param name="value"> 変換したい値</param>
            /// <returns>変換後の文字列</returns>
            template<typename T>
            static std::string FormatToString(T&& value) {
                std::ostringstream oss;
                using DecayedType = std::decay_t<T>;

                if constexpr (std::is_same_v<DecayedType, const char*> || std::is_same_v<DecayedType, char*>) {
                    oss << value;  // 文字列として出力
                }
                else if constexpr (std::is_pointer_v<DecayedType>) {
                    if (value == nullptr) {
                        oss << "null";
                    }
                    else {
                        using PointeeType = std::remove_pointer_t<DecayedType>;
                        if constexpr (std::is_same_v<PointeeType, std::string>) {
                            oss << *value;
                        }
                        else if constexpr (std::is_class_v<PointeeType>) {
                            oss << typeid(*value).name();
                        }
                        else {
                            oss << *value;
                        }
                    }
                }
                else {
                    if constexpr (std::is_same_v<DecayedType, std::string>) {
                        oss << value;
                    }
                    else if constexpr (std::is_class_v<DecayedType>) {
                        oss << typeid(value).name();
                    }
                    else {
                        oss << value;
                    }
                }
                return oss.str();
            }


            /// <summary>
            /// 指定されたメッセージに対して、渡された引数を埋め込んでフォーマットされた文字列を返す関数.
            /// </summary>
            /// <typeparam name="...Args"> 埋め込む引数の型 </typeparam>
            /// <param name="message"> フォーマットする文字列（プレースホルダ `{}` を含む）    </param>
            /// <param name="...args"> 埋め込みたいデータ</param>
            /// <returns>フォーマットされた後の文字列</returns>
            template<typename... Args>
            static std::string FormatString(const std::string& message, Args&& ...args) {
                std::string result = message;
                std::vector<std::string> argsList = { FormatToString(std::forward<Args>(args))... };

                // プレースホルダー内の空白を無視する正規表現.
                // 1. 数字の前後の空白を無視し、
                // 2. コロン前後の空白も無視し、
                // 3. フォーマット指定の前後の空白も無視する。
                // 例. {0 : 0>5} -> [0]"{0 : 0>5}" [1]"0" [2] "0>5" ...
                std::regex formatRegex(R"(\{\s*(\d*)\s*:?\s*([^\}]*)\s*\})");
                std::smatch match;
                size_t argIndex = 0;

                while (std::regex_search(result, match, formatRegex)) {
                    std::string numStr = match[1].str();
                    std::string formatSpec = match[2].str();

                    size_t index = numStr.empty() ? argIndex++ : std::stoi(numStr);

                    if (index < argsList.size()) {
                        std::string formattedValue = ApplyFormat(argsList[index], formatSpec);
                        result.replace(match.position(0), match.length(0), formattedValue);
                    }
                    else {
                        result.replace(match.position(0), match.length(0), "");
                    }
                }

                return result;
            }
        private:
            // フォーマット指定のパース
            static void ParseFormatSpec(const std::string& formatSpec, char& alignment, int& width, char& fillChar, std::string& typeSpecifier, int& precision) {
                if (formatSpec.empty()) return;

                // 小数点以下の桁数の初期化
                precision = -1;

                // フォーマット指定の最初が '*' なら埋める文字を '*' に設定
                if (formatSpec[1] == '<' || formatSpec[1] == '^' || formatSpec[1] == '>') {
                    fillChar = formatSpec[0];
                    alignment = formatSpec[1];  // 配置方向（<, >, ^）
                    std::string widthStr = formatSpec.substr(2);  // 幅の指定
                    if (!widthStr.empty() && std::all_of(widthStr.begin(), widthStr.end(), ::isdigit)) {
                        width = std::stoi(widthStr);
                    }
                }
                else if (formatSpec.size() > 1 && (formatSpec[0] == '<' || formatSpec[0] == '>' || formatSpec[0] == '^' || formatSpec[0] == '-')) {
                    alignment = formatSpec[0];
                    // 幅の指定があれば、それを取得
                    std::string tmp = formatSpec.substr(1);
                    std::string widthStr;
                    for (char c : tmp) {
                        if (std::isdigit(c)) {
                            widthStr.push_back(c);  // 数字を幅に格納
                        }
                        else {
                            break;
                        }
                    }
                    if (!widthStr.empty() && std::all_of(widthStr.begin(), widthStr.end(), ::isdigit)) {
                        width = std::stoi(widthStr);
                    }
                }
                else if (std::regex_match(formatSpec, std::regex(R"(\d+)"))) {
                    width = std::stoi(formatSpec);
                }

                // 小数点以下の桁数とタイプ指定を処理
                size_t dotPos = formatSpec.find('.');
                if (dotPos != std::string::npos) {
                    // 小数点があれば、その後ろに続く数字を精度として扱う
                    size_t endPos = formatSpec.find_first_of("f", dotPos);
                    if (endPos != std::string::npos) {
                        precision = std::stoi(formatSpec.substr(dotPos + 1, endPos - dotPos - 1));
                    }
                }

                // 事前にセットを作成しておく
                static const std::set<char> validTypeSpecifiers =
                { 'd', 'f', 'x', 'X', 'o', 'b', 'E', 'e', 'g' };
                // 数値のタイプ指定
                if (validTypeSpecifiers.count(formatSpec.back())) {
                    typeSpecifier = formatSpec.back();
                }

            }

            // 数値処理（整数・浮動小数点・16進数・2進数・8進数）
            static std::string FormatNumber(const std::string& value, const std::string& typeSpecifier, int width, int precision = -1) {
                std::ostringstream oss;
                std::smatch match;
                std::string val = value;

                // 数値チェック (整数または浮動小数点)
                if (!std::regex_match(val, match, std::regex(R"(\d+(\.\d+)?$)"))) {
                    if (!val.empty()) {
                        val = std::to_string(static_cast<int>(val[0]));  // 文字列が数字以外のとき、最初の文字を整数として処理
                    }
                }

                // 整数の処理
                if (typeSpecifier == "d") {
                    int num = std::stoi(val);
                    oss << std::setw(width) << std::setfill(' ') << num;
                }
                // 浮動小数点数の処理
                else if (typeSpecifier == "f") {
                    double num = std::stod(val);  // double型で扱う
                    if (precision >= 0) {
                        oss << std::fixed << std::setprecision(precision) << std::setw(width) << num;
                    }
                    else {
                        oss << std::fixed << std::setprecision(6) << std::setw(width) << num;
                    }
                }
                else if (typeSpecifier == "e") {
                    float num = std::stof(val);
                    if (precision >= 0) {
                        oss << std::scientific << std::setprecision(precision) << std::setw(width) << num;
                    }
                    else {
                        oss << std::scientific << std::setw(width) << num;
                    }
                }
                else if (typeSpecifier == "E") {
                    double num = std::stof(val);
                    if (precision >= 0) {
                        oss << std::scientific << std::uppercase << std::setprecision(precision) << std::setw(width) << num;
                    }
                    else {
                        oss << std::scientific << std::uppercase << std::setw(width) << num;
                    }
                }
                else if (typeSpecifier == "g") {
                    float num = std::stof(val);
                    if (precision >= 0) {
                        oss << std::setprecision(precision) << std::setw(width) << num;
                    }
                    else {
                        oss << std::setprecision(6) << std::setw(width) << num;
                    }
                }
                // 16進数として処理
                else if (typeSpecifier == "x") {
                    int num = std::stoi(val);
                    oss << std::hex << num;
                }
                else if (typeSpecifier == "X") {
                    int num = std::stoi(val);
                    oss << std::uppercase << std::hex << std::setw(width) << num;
                }
                // 8進数として処理
                else if (typeSpecifier == "o") {
                    int num = std::stoi(val);
                    oss << std::oct << num;
                }
                // 2進数として処理
                else if (typeSpecifier == "b") {
                    int num = std::stoi(val);
                    oss << std::bitset<32>(num);  // 32ビット長で表示
                }

                return oss.str();
            }

            // ブール値の処理
            static std::string HandleBoolean(const std::string& value) {
                if (value == "1") {
                    return "true";
                }
                if (value == "0") {
                    return "false";
                }
                return value;
            }

            // 符号付き整数の処理
            static std::string HandleSignedInteger(const std::string& value, const std::string& formatSpec) {
                if (formatSpec == "+") {
                    std::ostringstream oss;
                    int num = std::stoi(value);
                    oss << std::showpos << num;
                    return oss.str();
                }
                return value;
            }

            // 寄せ・幅指定の処理
            static std::string ApplyAlignment(const std::string& value, char alignment, int width, char fillChar) {
                std::ostringstream oss;

                if (width > 0) {
                    // 幅指定がある場合、値を適切に整形
                    if (alignment == '<') {
                        oss << std::left << std::setw(width) << std::setfill(fillChar) << value;  // 左寄せ
                    }
                    else if (alignment == '>') {
                        oss << std::right << std::setw(width) << std::setfill(fillChar) << value;  // 右寄せ
                    }
                    else if (alignment == '^') {
                        // 中央寄せのために必要なパディングを計算
                        int padding = (int)((width - value.size()) / 2);
                        std::string leftPadding(padding, fillChar);
                        std::string rightPadding(width - padding - value.size(), fillChar);
                        oss << leftPadding << value << rightPadding;
                    }
                    else {
                        oss << std::setw(width) << value;  // デフォルトは右寄せ
                    }
                }
                else {
                    // 幅指定がない場合はそのまま値を出力
                    oss << value;
                }
                return oss.str();
            }

            // フォーマット指定を適用する
            static std::string ApplyFormat(const std::string& value, const std::string& formatSpec) {
                std::ostringstream oss;

                if (formatSpec.empty()) {
                    return value;
                }

                char alignment = ' ';
                int width = 0;
                char fillChar = ' ';
                std::string typeSpecifier;
                int precision = -1;

                // フォーマット指定のパース
                ParseFormatSpec(formatSpec, alignment, width, fillChar, typeSpecifier, precision);

                // 符号付き整数の処理
                std::string result = HandleSignedInteger(value, formatSpec);
                if (result != value) {
                    return result;
                }

                // 数値フォーマットの処理
                if (!typeSpecifier.empty()) {
                    return FormatNumber(value, typeSpecifier, width, precision);
                }

                // ブール値の処理
                if (value == "1" || value == "0") {
                    return HandleBoolean(value);
                }

                // 寄せ・幅指定の処理
                return ApplyAlignment(value, alignment, width, fillChar);
            }
        };

    } // namespace Utility 
} // namespace System 