#pragma once
/*
	GameEngine.h
*/
#include "Application.hpp"
//画面サイズ.
#define WIDTH			(960)		//横  (x).
#define HEIGHT			(720)		//高さ(y).


#define MY_MAX(a, b)				(((a) > (b)) ? (a) : (b))
#define MY_MIN(a, b)				(((a) < (b)) ? (a) : (b))

#define var     auto

#include <thread>
/// <summary>
/// ウィンドウの画面サイズ管理クラス.
/// シングルトンパターンを用いて、ウィンドウの初期化や管理を行う.
/// </summary>
class Window {
private:
    Vector2D vec2Min;               // ウィンドウの最小サイズ
    Vector2D vec2Max;               // ウィンドウの最大サイズ
    Window();                       // コンストラクタ.
    ~Window();
public:
    // シングルトンインスタンスを取得するメソッド.
    static Window& GetInstance() {
        static Window window;
        return window;
    }
    // ウィンドウの初期化処理を行う.
    void InitWindow();
    // ウィンドウのサイズを設定する.
    void SetWindow(float _maxX = WIDTH, float _maxY = HEIGHT);
    // ウィンドウの名前（タイトルバーのテキスト）を設定する.
    void SetWindowName(const char* text = "DxLib") { SetWindowText(text); }
    // ウィンドウの背景色を設定する.
    void SetBackGraundColor(int Red, int Green, int Blue) { SetBackgroundColor(Red, Green, Blue); }

    //  ウィンドウの最小サイズを取得する.
    void GetMinSize(float& _minX, float& _minY) { _minX = vec2Min.x; _minY = vec2Min.y; }
    // ウィンドウの最大サイズを取得する.
    void GetMaxSize(float& _maxX, float& _maxY) { _maxX = vec2Max.x; _maxY = vec2Max.y; }
    // ウィンドウを最大化する.
    void MaximizeWindow() { DxLib::SetWindowSizeChangeEnableFlag(TRUE, TRUE); }
    // ウィンドウを最小化する.
    void MinimizeWindow() { DxLib::SetWindowStyleMode(1); }
    // ウィンドウを元の状態に戻す.
    void RestoreWindow() { DxLib::SetWindowStyleMode(3); }
    // ウィンドウのフルスクリーンモードを設定する.
    void SetFullScreen(bool enable) { DxLib::ChangeWindowMode(!enable); }
    // ウィンドウがフォーカスされているかどうかを確認する.
    bool IsWindowFocused() { return DxLib::GetActiveFlag() == TRUE; }
    // マウスカーソルの表示・非表示を設定する.
    void ShowCursor(bool show) { DxLib::SetMouseDispFlag(show ? TRUE : FALSE); }
    // 裏でも処理される 有効・非有効を設定する.
    void SetAlwaysRunFlag(bool show) { DxLib::SetAlwaysRunFlag(show ? TRUE : FALSE); }
    // V-Syncの待機フラグを有効・非有効を設定する.
    void SetWaitVSyncFlag(bool _set) { DxLib::SetWaitVSyncFlag(_set ? TRUE : FALSE); }
    // ウィンドウのアイコンを HICON を使用して設定する.
    void SetWindowIcon(HICON icon);

    void SetWindowIcon(int iconID);
    // アイコンファイル (.ico) からウィンドウのアイコンを設定する.
    void SetWindowIconFromFile(const char* iconPath);
    // ウィンドウのアイコンをデフォルトアイコンにリセットする.
    void ResetWindowIcon();

    // ウィンドウのリソースを解放する.
    void Release() { vec2Min = vec2Max = Vector2D(); }
    // ウィンドウを終了する.
    void ExitWindow();
    // コンソールを有効化する関数
    void EnableConsole();
    void DisableConsole();

    Vector2D  GetRandomVector2D();
    Vector2D  GetMaxVector2D();
};
inline Window& GetWindow() { return Window::GetInstance(); }
/// <summary>
/// ステージのサイズ.
/// </summary>
class StageSize {
public:
	float minAreaX, minAreaY; 
	float maxAreaX, maxAreaY;
	StageSize();
	static StageSize* m_stageSize;
public:
	static StageSize* GetInstance() {
		if (m_stageSize == nullptr) {
			m_stageSize = new StageSize;
		}
		return m_stageSize;
	}
	void SetStageSize(float minX = 0, float minY = 0, float maxX = 0, float maxY = 0);
	void GetMinSize(float& _minX, float& _minY) { _minX = minAreaX; _minY = minAreaY; }
	void GetMaxSize(float& _maxX, float& _maxY) { _maxX = maxAreaX; _maxY = maxAreaY; }
	void Release() { minAreaX = minAreaY = maxAreaX = maxAreaY = NULL; }
};
inline StageSize* GetStage() { return StageSize::GetInstance(); }
/// <summary>
///  <para>FPS制御を行うクラス.FPS(FPS値)を設定し。				</para>
///  <para>Fps_wait()呼び出すことで制御ができます。				</para>
///  <para>Draw_fps(x,y)呼び出すことで現在FPSが表示されます。	</para>
/// </summary>
class FPS {
private:
	int fps;
	std::vector<int> f;
	int fps_count, count0t;
	double ave;
public:
	FPS(int FPS = 60);
	int GetFps() const { return fps; }
	/// <summary>
	/// FPSを計算・制御.
	/// </summary>
	void Fps_wait();
	/// <summary>
	/// x,yの位置にfpsを表示
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	void Draw_fps(int x, int y);
};

/// <summary>
/// 時間処理.
/// </summary>
class Timer {
    friend class Engine;
private:
    Timer() : 
        deltaTime(0), unscaledDeltaTime(0), time(0), unscaledTime(0), 
        timeScale(1.0f), fixeFps(60), fixTime(0), fpsCount(0), 
        smoothDeltaTime(0), lastDeltaTime(0), isFixUpdate(false), isFps(false)
        ,fps(0), fpsTime(0), frameCount(0)
    {
        lastFrameTime  = std::chrono::high_resolution_clock::now();
        nextFrameTime  = std::chrono::high_resolution_clock::now();
    }

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;
	std::chrono::high_resolution_clock::time_point lastFrameTime;
    std::chrono::high_resolution_clock::time_point nextFrameTime;
private:
    int fixeFps;
    float fixTime;
    int fpsCount;     
    float smoothDeltaTime;
    float lastDeltaTime;

    bool isFixUpdate;

    bool isFps;
    float fps;               // 現在の
    float fpsTime;           // 現在のFPS
public:
	// Timeクラスを取得する.
	static Timer& Instance() {
		static Timer instance;
		return instance;
	}

	// 時間関連のメンバ変数.
    float deltaTime;            // 前のフレームと現在のフレームの間の経過時間
    float unscaledDeltaTime;    // 前のフレームと現在のフレームの間の経過時間（timeScale無視）
    float time;                 // ゲーム開始からの経過時間（timeScale適用）
    float unscaledTime;         // ゲーム開始からの経過時間（timeScale無視）
    float timeScale;            // ゲーム内の時間の進行速度

    int frameCount;             // フレームカウント.       

	void Reset() {
        nextFrameTime = std::chrono::high_resolution_clock::now();
		lastFrameTime = std::chrono::high_resolution_clock::now();
		deltaTime = 0.0f;
		unscaledDeltaTime = 0.0f;
		time = 0.0f;
		unscaledTime = 0.0f;
		timeScale = 1.0f;       // デフォルトのタイムスケール.
        fixTime = 0.0f;
        fpsCount = 0;
        smoothDeltaTime = 0.0f;
        lastDeltaTime = 0.0f;
        frameCount = 0;
	}
	/// <summary>
	/// Time classの時間更新.
	/// </summary>
	void Update() {
        isFps = false;
        isFixUpdate = false;

		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> deltaTimeDuration = now - lastFrameTime;
		deltaTime = deltaTimeDuration.count() * timeScale;
		unscaledDeltaTime = deltaTimeDuration.count();
		lastFrameTime = now;

		time += deltaTime;
		unscaledTime += unscaledDeltaTime;

        // FixedUpdateタイミングの更新.
        fixTime += deltaTime;
        if (fixTime >= (1.0f / fixeFps)) {
            fixTime -= (1.0f / fixeFps);
            isFixUpdate = true;
        }
        // フレームカウントの更新.
        fpsTime += unscaledDeltaTime;
        fpsCount++;
        if (fpsTime >= 1.0f) {
            fps = fpsCount / fpsTime;
            fpsCount = 0;
            fpsTime = 0;
            isFps = true;
        }
        // SmoothDeltaTimeの更新（平滑化）.
        smoothDeltaTime = (smoothDeltaTime * 0.9f) + (deltaTime * 0.1f);

        // 最後のDeltaTimeを保存.
        lastDeltaTime = deltaTime;

        // フレームカウント.
        frameCount++;
	}

    bool IsFixUpdate() {
        return isFixUpdate;
    }
	void SetTimeScale(float scale) { timeScale = scale; }
    int GetFpsCount() const { return fpsCount; }
    float GetSmoothDeltaTime() const { return smoothDeltaTime; }
    float GetRealtimeSinceStartup() const { return unscaledTime; }
    float GetFPS() const { return fps; }
    bool IsUpdateFps() const { return isFps; }
protected:
    void SetFixed(int _fps) {
        if (_fps > 120) fixeFps = 120;
        else if (_fps > 30) fixeFps = _fps;
        else fixeFps = 30;
    }

    void SleepAppFPS() {
        int targetFps = GameEngine::Application::GetInstanse().targetFrameRate;
        if (targetFps <= 0) return;

        float targetFrameTime = 1.0f / targetFps;
        nextFrameTime += std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
            std::chrono::duration<float>(targetFrameTime)
        );

        auto now = std::chrono::high_resolution_clock::now();
        if (nextFrameTime > now) {
            std::this_thread::sleep_for(nextFrameTime - now);
        }
        else {
            nextFrameTime = now;
        }
    }

};
// get Timer .class. Instance();
#define Time Timer::Instance()

class Engine {
private:
    Engine() : targetFrameRate(0), targetFrameTime(0) {};

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
private:// メンバ変数.

    int targetFrameRate;// Loop (デフォルト 0 <制限なし>)
    float targetFrameTime;

private: // 関数.  

    // ループOKか.
    bool LoopProcess();
    // 初期化.
    void Initialize();
    // 定期的.
    void FixedUpdate();
    // 入力系.
    void ProcessInput();
    // コルーチン
#if (_MSVC_LANG >= 202002L)
    void UpdateCoroutine();
#endif
    // Update.
    void UpdateGameLogic();
    // Animation
    void OnStateMachineEnter();
    void OnStateMachineExit();
    void OnAnimationIK();
    // 更新後.
    void LateUpdate();
    // 描画.
    void DrawScreen();
    // 切り替え.
    void ScreenFlip();

    void DebugLogic();

    // FPS表示のための関数
    void DisplayFPS() {
        if (Time.IsUpdateFps()) {
            std::cout << "FPS: " << Time.GetFPS() << std::endl;
        }
    }

    void GameEngineExit();
public:

    static Engine& Instance() {
        static Engine instance;
        return instance;
    }

    bool InitGame();

    void Run() {       
        do
        {
            clsDx();
          
            Time.Update();

            Initialize();

            // 物理処理.
            if (Time.IsFixUpdate()) {
                FixedUpdate();
            }

            // 入力更新.
            ProcessInput();
#if (_MSVC_LANG >= 202002L)
            // コルーチン更新.
            UpdateCoroutine();
#endif
            // update.
            UpdateGameLogic();

            OnStateMachineEnter();
            OnStateMachineExit();
            OnAnimationIK();

            // Update後
            LateUpdate();

            DrawScreen();
            // 切り替え.
            ScreenFlip();

#if _DEBUG
            DebugLogic();
#endif
            Time.SleepAppFPS();
        } 
        while (LoopProcess());

        GameEngineExit();

    }

    void SetTargetFrameRate(int _fps) {
        if (_fps > 0) targetFrameRate = _fps;
        else          targetFrameRate = 0;
    }

    void SetFixed(int _fps) {
        Time.SetFixed(_fps);
    }

};
