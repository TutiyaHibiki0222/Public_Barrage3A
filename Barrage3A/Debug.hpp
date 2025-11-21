/*
    Debug.hpp

    class-

    作成日         : 2025/03/25
    最終変更日     : 2025/03/27
*/

#pragma once
#include "Format.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <windows.h>
#include <cstdarg>
#include <cstdio>

using namespace System::Utility;

namespace GameEngine {

    enum class LogLevel : int {
        Info = 0,
        Warning,
        Error
    };

    class Debug {
    public:

        // Logメソッド
        template<typename... Args>
        static std::string Log(const std::string message, Args&&... args) {
            return PrintLog("INFO", message, LogLevel::Info, std::forward<Args>(args)...); 
        }
        
        template<typename T>
        static std::string Log(const T& value) {
            return PrintLog("INFO", "{}", LogLevel::Info, value);
        }

        // WarningLogメソッド
        template<typename... Args>
        static std::string WarningLog(const std::string message, Args&&... args) {
            return PrintLog("WARNING", message, LogLevel::Warning, std::forward<Args>(args)...); 
        }

        // ErrorLogメソッド
        template<typename... Args>
        static std::string ErrorLog(const std::string message, Args&&... args) {
            return PrintLog("ERROR", message,  LogLevel::Error, std::forward<Args>(args)...);
        }

        // Printメソッド.
        template<typename... Args>
        static std::string Print(const std::string message, Args&& ...args) {
            // 可変引数を処理してフォーマットする
            std::string formattedMessage = Format::FormatString(message, std::forward<Args>(args)...);

            // ログをコンソールに出力.
            std::cout << formattedMessage << std::endl;
            
            ResetConsoleColor(); // 色をリセット

            return formattedMessage;
        }

        template<typename T>
        static std::string Print(const T& value) {
            // 可変引数を処理してフォーマットする
            std::string formattedMessage = Format::FormatString("{}", value);

            // ログをコンソールに出力.
            std::cout << formattedMessage << std::endl;

            ResetConsoleColor(); // 色をリセット

            return formattedMessage;
        }
    private:
        // 可変引数を文字列にフォーマットしてログを出力
        template<typename... Args>
        static std::string PrintLog(const std::string& level, const std::string& message, LogLevel logLevel, Args&&... args) {
            // 文字列をVSPRINTF用に置換（{} を %d や %f に変換）
            std::string formattedMessage = Format::FormatString(message, std::forward<Args>(args)...);

            std::string logMessage = GetTimestamp() + " [" + level + "] : " + formattedMessage;  // ログメッセージ

            // コンソールに出力（色付け）
            SetConsoleTextColor(logLevel);

            std::cout << logMessage << std::endl;

            SetConsoleTextColor(LogLevel::Info);  // デフォルトの色に戻す
            return logMessage;
        }

        // タイムスタンプを取得
        static std::string GetTimestamp() {
            std::time_t now = std::time(nullptr);
            struct tm timeInfo;
            localtime_s(&timeInfo, &now);
            char buf[64];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);
            return std::string(buf);
        }

        // ログレベルに応じたコンソールの文字色を設定
        static void SetConsoleTextColor(LogLevel logLevel) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            switch (logLevel) {
            case LogLevel::Error:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                break;
            case LogLevel::Warning:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                break;
            case LogLevel::Info:
            default:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                break;
            }
        }
        // コンソールの色を設定（RGB）
        static void SetConsoleTextColor(int r, int g, int b) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            WORD color = RGBToColorAttribute(r, g, b);
            SetConsoleTextAttribute(hConsole, color);
        }
        // RGBをコンソールの色属性に変換
        static WORD RGBToColorAttribute(int r, int g, int b) {
            // RGB値を16進数に変換
            r = (r / 32) & 0x7;  // 0-255のRGBを0-7のビットにマッピング
            g = (g / 32) & 0x7;
            b = (b / 32) & 0x7;
            return (r << 0) | (g << 4) | (b << 8);
        }
        // コンソールの色をリセット
        static void ResetConsoleColor() {
            SetConsoleTextColor(LogLevel::Info);
        }
        // ログファイルに出力（再起動時に削除してから書き込み）
        static void WriteLogToFile(const std::string& logMessage) {
            const std::string filename = "debug_log.txt";
            // ログをファイルに出力
            std::ofstream logFile(filename, std::ios::app);
            if (logFile.is_open()) {
                logFile << logMessage << std::endl;
                logFile.close();
            }
        }
    };

    template<typename...Args>
    inline std::string print(std::string message, Args&&... args) { return Debug::Print(message, std::forward<Args>(args)...); };
    template<typename T>
    inline std::string print(const T& value) { return Debug::Print(value); };
    template<typename...Args>
    inline std::string loger(std::string message, Args&&... args) { 
        return Debug::Log(message, std::forward<Args>(args)...); 
    };
    template<typename...Args>
    inline std::string loger(LogLevel level,std::string message,Args&&... args) {
        std::string ms = "";
        switch (level)
        {
        case GameEngine::LogLevel::Info:
            ms = Debug::Log(message, std::forward<Args>(args)...);
            break;
        case GameEngine::LogLevel::Warning:
            ms = Debug::WarningLog(message, std::forward<Args>(args)...);
            break;
        case GameEngine::LogLevel::Error:
            ms = Debug::ErrorLog(message, std::forward<Args>(args)...);
            break;
        default:
            break;
        }
        return ms;
    };

};
