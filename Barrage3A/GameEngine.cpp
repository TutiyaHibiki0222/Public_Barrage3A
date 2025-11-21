#include "Project.h"
#include "Vector.h"
#include "GameEngine.h"
#include "InputSystem.h"
#include "GameObjectMgr.h"
#include "ColliderManager.h"
#include "RendererManager.h"
#include "GameWorldManager.hpp"
#include "Invoke.hpp"
#if (_MSVC_LANG >= 202002L)
#include "Coroutine.hpp"
#endif
#include "Debug.hpp"
#include "PlayerPrefs.h"
using namespace GameEngine;
/////////// WindowSize class ////////////////

Window::Window()
	: vec2Min(0,0),vec2Max(WIDTH, HEIGHT)
{}

Window::~Window() {
    ExitWindow();
}

void Window::InitWindow() {
	ChangeWindowMode(TRUE);
    SetGraphMode((int)vec2Max.x, (int)vec2Max.y, 32);
}

void Window::SetWindow(float _maxW, float _maxH) {
    vec2Max.x = _maxW; vec2Max.y = _maxH;
	SetWindowSize((int)vec2Max.x, (int)vec2Max.y);
}

// ウィンドウのアイコンを設定 (HICON を直接指定)
void Window::SetWindowIcon(HICON icon) {
    DxLib::SetWindowIconHandle(icon);
}
void Window::SetWindowIcon(int iconID) {
    DxLib::SetWindowIconID(iconID);
}
// アイコンファイル (.ico) からウィンドウのアイコンを設定
void Window::SetWindowIconFromFile(const char* iconPath) {
    HICON icon = static_cast<HICON>(
        LoadImageA(
        nullptr,
        iconPath,
        IMAGE_ICON,
        0, 0,
        LR_LOADFROMFILE | LR_DEFAULTSIZE
    ));
    if (icon) {
        SetWindowIcon(icon);
        DestroyIcon(icon);
    }
    else {
        GameEngine::Debug::WarningLog("アイコンの読み込みに失敗しました");
    }
}

// アイコンをリセット（デフォルトアイコンに戻す）
void Window::ResetWindowIcon() {
    SetWindowIcon(nullptr); // nullptr を渡すことでデフォルトに戻る
}

void Window::ExitWindow() {
    Release();
    DisableConsole();
    DxLib::DxLib_End();
}

void Window::EnableConsole() {
    static bool isConsoleAllocated = false;
    if (!isConsoleAllocated) {
        if (!AllocConsole()) {
            std::cerr << "コンソールの作成に失敗しました" << std::endl;
            MessageBox(nullptr, "コンソールの作成に失敗しました", "Error", MB_OK | MB_ICONERROR);
            FreeConsole();
            return;
        }
        isConsoleAllocated = true;
    }

    FILE* fpOut = nullptr;
    FILE* fpIn = nullptr;

    if (freopen_s(&fpOut, "CONOUT$", "w", stdout) != 0 ||
        freopen_s(&fpIn , "CONIN$" , "r", stdin) != 0) {
        MessageBox(nullptr, "コンソールのリダイレクトに失敗しました", "Error", MB_OK | MB_ICONERROR);
        FreeConsole();
        return;
    }

    FILE* fpErr = nullptr;
    freopen_s(&fpErr, "CONOUT$", "w", stderr);

    SetConsoleTitle("Debug Console");

    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (consoleHandle != INVALID_HANDLE_VALUE) {
        COORD bufferSize = { 120, 300 };
        SetConsoleScreenBufferSize(consoleHandle, bufferSize);

        SMALL_RECT windowSize = { 0, 0, bufferSize.X - 1, 30 }; // 30 行だけ表示するように設定
        SetConsoleWindowInfo(consoleHandle, TRUE, &windowSize);
    }

    // 【追加】日本語出力対応設定
    SetConsoleOutputCP(932); // 出力コードページを UTF-8 に設定
    std::locale::global(std::locale("japanese"));
    std::wcout.imbue(std::locale());

}

void Window::DisableConsole() {
    // 標準出力・入力のリセット
    fclose(stdout);
    fclose(stdin);

    // コンソールを解放
    FreeConsole();
}

Vector2D Window::GetRandomVector2D() {
    return Vector2D(Random.Range(vec2Min.x, vec2Max.x), Random.Range(vec2Min.y, vec2Max.y));
}

Vector2D Window::GetMaxVector2D() {
    return vec2Max;
}

/////////// StageSize class ////////////////
StageSize* StageSize::m_stageSize = nullptr;

StageSize::StageSize()
	: minAreaX(0), minAreaY(0), maxAreaX(0), maxAreaY(0)
{}

void StageSize::SetStageSize(float minX, float minY, float maxX, float maxY) {
	minAreaX = minX;
	minAreaY = minY;
	maxAreaX = maxX;
	maxAreaY = maxY;
}

/////////// FPS class ////////////////

FPS::FPS(int FPS) : fps(FPS), f(FPS, 0), fps_count(0), count0t(0), ave(0.0) {}

void FPS::Fps_wait() {
	int term, i, gnt;
	static int t = 0;
	if (fps_count == 0) {
		if (t == 0) {
			term = 0;
		}
		else {
			term = count0t + 1000 - GetNowCount();
		}
	}
	else {
		//待つべき時間=現在あるべき時刻-現在の時刻.
		term = (int)(count0t + fps_count * (1000.0 / fps)) - GetNowCount();
	}
	if (term > 0) {
		Sleep(term);
	}
	gnt = GetNowCount();
	if (fps_count == 0) {//60フレームに1度基準を作る.
		count0t = gnt;
	}
	f[fps_count] = gnt - t;//１周した時間を記録.
	t = gnt;
	//平均計算.
	if (fps_count == fps - 1) {
		ave = 0;
		for (i = 0; i < fps; i++)
			ave += f[i];
		ave /= fps;
	}
	fps_count = (++fps_count) % fps;
}

void FPS::Draw_fps(int x, int y) {
	if (ave != 0) {
		DrawFormatString(x, y, 0xFFFFFF, "[%.1f]", 1000 / ave);
	}
}

//////////// Engine //////////////////

bool Engine::InitGame() {

    SetUseDirect3DVersion(DX_DIRECT3D_9EX);//これを入れないとシェーダーが使えない
    if (DxLib_Init()) return false;	// DXライブラリの初期化、エラーで終了.
    SetDrawScreen(DX_SCREEN_BACK);		// 裏画面へ描画（ダブルバッファ).
    return true;
}

bool Engine::LoopProcess() {
    // ループ終了類.
    if (ProcessMessage()) {
        return false;
    }
    return GameEngine::Application::GetInstanse().isPlaying;
}

void Engine::Initialize() {
    Object.ProcessNewObjects();
    Object.AllGameObjectStart();
}

void Engine::FixedUpdate() {
    Object.AllGameObjectFixedUpdate();
    CollisionManager::GetInstance().CheckCollisions();
#if (_MSVC_LANG >= 202002L)
    CoManager.FixedUpdate();
#endif
}

void Engine::ProcessInput() {
    Input.Update();
}

#if (_MSVC_LANG >= 202002L)
void Engine::UpdateCoroutine() {
    CoManager.Update();
}
#endif

void Engine::UpdateGameLogic() {
	GameWorldManager::GetInstance().Update();
    Object.AllGameObjectUpdate();
    System::InvokeManager::GetInstance().Update();
    SceneManager::GetInstance().Update();
}

void Engine::OnStateMachineEnter() {
// ステート遷移に伴う初期化や処理を書く
// 例: ゲーム状態の変更、音楽や効果音の切り替えなど
}

void Engine::OnStateMachineExit() {
// ステート終了時に必要なクリーンアップや処理を書く
// 例: ゲームポーズ解除、リソースの解放など
}

void Engine::OnAnimationIK() {
// アニメーション逆運動学を使ってキャラクターのポーズを調整する処理を書く
// 例: キャラクターの手足を目標位置に移動させる
}

void Engine::LateUpdate() {
    Object.AllGameObjectLateUpdate();
}

void Engine::DrawScreen() {
    SceneManager::GetInstance().UpdateTransitor();
	ClearDrawScreen();                              // 画面をクリア.
    RendererManager::GetInstance().Render();
    SceneManager::GetInstance().Draw();
    SceneManager::GetInstance().DrawTransitor();
}

void Engine::ScreenFlip() {
    // 不要なObject削除.
    Object.ProcessDestroyQueue();

    DxLib::ScreenFlip();
}

void Engine::DebugLogic() {
    DisplayFPS();
}

void Engine::GameEngineExit() {
    Object.AllOnApplicationQuit();
    Object.AllDestroyGameObject();
    Object.ProcessDestroyQueue();
    CoManager.StopAllCoroutines();
    System::InvokeManager::GetInstance().Clear();

    PlayerPrefs::Save();
}