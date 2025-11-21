#pragma once

#include <iostream>
#include <string>
#include <windows.h>
#include "Mathf.h"
namespace GameEditor {

    class EditorUtility {
    public:
        // 色をenumで管理
        enum class ConsoleColor {
            Red,
            Green,
            Blue,
            White
        };

        // エラーメッセージボックス
        static void DisplayErrorMessage(const std::string& title, const std::string& message) {
            MessageBoxA(NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
        }

        // 複数ボタンのエラーダイアログ
        static int DisplayErrorMessageComplex(const std::string& title, const std::string& message) {
            int result = MessageBoxA(NULL, message.c_str(), title.c_str(), MB_ABORTRETRYIGNORE | MB_ICONERROR);
            switch (result) {
            case IDABORT:  return 0;  // Abortボタン
            case IDRETRY:  return 1;  // Retryボタン
            case IDIGNORE: return 2;  // Ignoreボタン
            default: return -1;
            }
        }

        // ダイアログボックスを表示する (OK, Cancel)
        static bool DisplayDialog(const std::string& title, const std::string& message, const std::string& ok, const std::string& cancel = "") {
            int result = MessageBoxA(NULL, message.c_str(), title.c_str(), MB_OKCANCEL | MB_ICONINFORMATION);
            return result == IDOK;  // OKが押された場合trueを返す
        }

        // 複数のボタンがあるダイアログを表示する (Yes, No, Cancel)
        static int DisplayDialogComplex(const std::string& title, const std::string& message, const std::string& ok, const std::string& cancel, const std::string& alt) {
            int result = MessageBoxA(NULL, message.c_str(), title.c_str(), MB_YESNOCANCEL | MB_ICONQUESTION);
            return result;  // 戻り値はユーザーが押したボタンのID
        }

        // コンソールに色付きでメッセージを表示する
        static void PrintColoredMessage(const std::string& message, ConsoleColor color = ConsoleColor::White) {
            // コンソールの色を変更する
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, GetConsoleColor(color));

            std::cout << message << std::endl;

            // 色をデフォルトに戻す
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        // ファイル操作（ファイルを開くなど）
        static void OpenFileWithDefaultApp(const std::string& filePath) {
            ShellExecuteA(NULL, "open", filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }

        // エクスプローラーでファイルを表示
        static void RevealInFinder(const std::string& filePath) {
            ShellExecuteA(NULL, "open", "explorer.exe", filePath.c_str(), NULL, SW_SHOW);
        }

        // 進捗バーの表示
        static void DisplayProgressBar(const std::string& title, float progress) {
            // 進捗が100%を超えないように制限
            progress = Mathf::Min(progress, 1.0f);

            int barWidth = 50;
            std::cout << title << ": [";
            int pos = (int)(barWidth * progress);
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << int(progress * 100.0f) << " %\r";
            std::cout.flush();
        }

        // 進捗バーをクリア
        static void ClearProgressBar() {
            std::cout << std::endl;
        }

        // コンソールをクリア
        static void ClearConsole() {
            system("cls");
        }

    private:
        // ConsoleColor を対応する Windows API の色コードに変換
        static WORD GetConsoleColor(ConsoleColor color) {
            switch (color) {
            case ConsoleColor::Red:     return FOREGROUND_RED | FOREGROUND_INTENSITY;
            case ConsoleColor::Green:   return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            case ConsoleColor::Blue:    return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            case ConsoleColor::White:   return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            default: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            }
        }
    };

};
