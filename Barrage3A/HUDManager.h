#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <sstream>
#include "GameManager.h"
#include "Text.h"
#include "Image.h"
#include "Texture2DShortcut.hpp"
#include "GameObject.h"
#include "FilterUI.h"

class HUDManager {
private:
    // UI references (weak so HUDManager doesn't own them)
    std::weak_ptr<Text> hiText;
    std::weak_ptr<Text> scoreText;
    std::vector<std::weak_ptr<Image>> lifes;
    std::weak_ptr<Text> lifeFragmentsText;
    std::vector<std::weak_ptr<Image>> bombs;
    std::weak_ptr<Text> bombFragmentsText;
    std::weak_ptr<Text> reiryokuText;
    std::weak_ptr<Text> addScoreText;
    std::weak_ptr<Text> grazeCountText;
    std::weak_ptr<Text> totalGrazeCountText;
    std::weak_ptr<Text> fpsText;
    std::weak_ptr<GameObject> pauseObject;

    // スコア表示の補間用
    int64_t displayedHiScore = 0;
    int64_t displayedScore = 0;
    static constexpr int64_t kHiScoreAnimSpeed = 1000LL;
    static constexpr int64_t kScoreAnimSpeed = 1000LL;

    // メニュー配列
    std::vector<std::weak_ptr<Image>> pauseMenuItems;
    std::vector<std::weak_ptr<Image>> confirmMenuItems;

    // 選択インデックスを別管理
    size_t mainSelectionIndex = 0;
    size_t confirmSelectionIndex = 0;

    // 入力リピート
    KeyCode currentKey = KeyCode::None;
    float keyRepeatTimer = 0.0f;
    bool keyInitialRepeatDone = false;
    static constexpr float initialDelay = 0.35f;
    static constexpr float repeatInterval = 0.12f;

    // 色アニメ用時間
    float time = 0.0f;

    // メニュー状態
    enum class PauseState { Main, Confirm };
    PauseState pauseState = PauseState::Main;

    // 一度だけ初期化フラグ
    bool pauseInitialized = true;
    bool isFlashing = false;
    // コンストラクタはシングルトン的に非公開
    HUDManager() {}

    // 更新の細かい機能
    void UpdateScore();
    void UpdateHiScore();
    void UpdateLife();
    void UpdateBomb();
    void UpdatePower();
    void UpdateAddScore();
    void UpdateGraze();
    void UpdateFPS();

    void UpdatePause();
    void UpdatePauseMenu(std::shared_ptr<GameObject> p);
    void UpdateGameOverMenu(std::shared_ptr<GameObject> p);

    void OnCheck();
    // 選択移動（ループ）
    void MoveSelection(int dir, size_t& selectionIndex, size_t count);
    System::Coroutine ShakeImage(std::shared_ptr<Image> img, float duration = 0.1f, float amplitudeX = 2.0f, float frequency = 25.0f);
    System::Coroutine FlashAndExecute(std::shared_ptr<Image> img, std::function<void()> callback, int flashCount = 8, float interval = 0.025f);
    void UpdateMenuHighlight(const std::vector<std::weak_ptr<Image>>& menu, size_t selectionIndex);
    bool HandleMenuInput(size_t& selectionIndex, size_t menuCount);

    bool HandleMenuKeyInput(size_t& selectionIndex, size_t menuCount);
    bool HandleMenuPadInput(size_t& selectionIndex, size_t menuCount);
public:
    static HUDManager& GetInstance();

    void Initialize(
        std::shared_ptr<Text> hi,
        std::shared_ptr<Text> score,
        const std::vector<std::shared_ptr<Image>>& lifeImages,
        std::shared_ptr<Text> lifeFrag,
        const std::vector<std::shared_ptr<Image>>& bombImages,
        std::shared_ptr<Text> bombFrag,
        std::shared_ptr<Text> reiryoku,
        std::shared_ptr<Text> addScore,
        std::shared_ptr<Text> grazeCount,
        std::shared_ptr<Text> totalGrazeCount,
        std::shared_ptr<Text> _fpsText = nullptr
    );

    void SetPause(const std::shared_ptr<GameObject> obj);
    void Update();
    void Reset();

    void ChangeGameOverPauseMenu();
    void ChangeDefaultPauseMenu();

};
