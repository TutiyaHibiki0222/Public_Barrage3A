/*
    Application.hpp

    作成日         : 2025/04/25
    最終変更日     : 2025/04/25
*/
#pragma once

namespace GameEngine {
    class Application {
    private:
        Application() {};
    public:
        static Application& GetInstanse() {
            static Application instanse;
            return instanse;
        }
        // フレームレートを固定 ( 0 以下は無制限).
        int targetFrameRate = -1;
        // ゲーム ループ実態関数.
        bool isPlaying = true;
        // ゲームエンジン終了関数.
        void Quit() {
            isPlaying = false;
        }

        size_t THRESHOLD = 3000;
    };
}

