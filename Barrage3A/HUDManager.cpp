#include "HUDManager.h"
#include <algorithm>
#include "Mathf.h"
#include "Format.hpp"
#include "TileTransitor.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"

HUDManager& HUDManager::GetInstance() {
    static HUDManager instance;
    return instance;
}

//----------------------------------
// 初期化
//----------------------------------
void HUDManager::Initialize(
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
    std::shared_ptr<Text> _fpsText
) {
    hiText = hi;
    scoreText = score;
    lifes.assign(lifeImages.begin(), lifeImages.end());
    bombs.assign(bombImages.begin(), bombImages.end());
    lifeFragmentsText = lifeFrag;
    bombFragmentsText = bombFrag;
    reiryokuText = reiryoku;
    addScoreText = addScore;
    grazeCountText = grazeCount;
    totalGrazeCountText = totalGrazeCount;
    fpsText = _fpsText;

    if (auto fps = fpsText.lock()) {
        fps->SetSpriteFont(nullptr); // デフォルトフォント設定（必要なら調整）
    }
}

//----------------------------------
// ポーズ設定
//----------------------------------
void HUDManager::SetPause(const std::shared_ptr<GameObject> obj) {
    pauseObject = obj;
    if (obj) {
        obj->AddAppBase<FilterUI>();
    }

    // 初回キャッシュ: 通常メニュー & 確認メニュー 
    if (pauseMenuItems.empty()) {
        if (auto g = obj->transform->FindByPath("Pause/PauseMenu/Nums"))
        {
            for (auto img : g->GetAppBasesInChildren<Image>(true))
                pauseMenuItems.push_back(img);
        }
    }
    if (confirmMenuItems.empty())
    {
        if (auto g = obj->transform->FindByPath("Pause/PauseMenu/check")->GetGameObject())
        {
            auto yesImg = g->transform->FindByPath("Yes")->GetGameObject()->GetAppBase<Image>();
            auto noImg = g->transform->FindByPath("No")->GetGameObject()->GetAppBase<Image>();
            confirmMenuItems.push_back(yesImg);
            confirmMenuItems.push_back(noImg);
            g->SetActive(false); // 最初は非表示
        }
    }
}

//----------------------------------
// 毎フレーム更新
//----------------------------------
void HUDManager::Update() {
    UpdateScore();
    UpdateHiScore();
    UpdateLife();
    UpdateBomb();
    UpdatePower();
    UpdateAddScore();
    UpdateGraze();
    UpdateFPS();
    UpdatePause();
}

//----------------------------------
// スコア更新
//----------------------------------
void HUDManager::UpdateScore() {
    auto& scoreMgr = GameManager::GetInstance().GetScoreManager();
    if (auto score = scoreText.lock()) {
        int64_t target = scoreMgr.GetScore();
        int64_t diff = target - displayedScore;
        if (diff > 0) {
            int64_t speed = std::max(diff / 15, kScoreAnimSpeed);
            displayedScore += std::min(diff, speed);
        }
        score->SetText(std::to_string(displayedScore));
    }
}

//----------------------------------
// ハイスコア更新
//----------------------------------
void HUDManager::UpdateHiScore() {
    auto& scoreMgr = GameManager::GetInstance().GetScoreManager();

    if (displayedHiScore == 0) {
        displayedHiScore = scoreMgr.GetHiScore();
    }

    if (auto hi = hiText.lock()) {
        int64_t targetHi = scoreMgr.GetHiScore();
        int64_t diff = targetHi - displayedHiScore;
        if (diff > 0) {
            int64_t speed = std::max(diff / 15, kHiScoreAnimSpeed);
            displayedHiScore += std::min(diff, speed);
        }
        hi->SetText(std::to_string(displayedHiScore));
    }
}

//----------------------------------
// 残機表示更新
//----------------------------------
void HUDManager::UpdateLife() {
    auto& lifeMgr = GameManager::GetInstance().GetLifeManager();
    const int lifeCount = lifeMgr.GetLife();
    const int fragments = lifeMgr.GetFragments();
    const int maxFragments = lifeMgr.GetFragmentsToLife();

    std::string path = "UI/Item:";

    for (size_t i = 0; i < lifes.size(); ++i) {
        if (auto l = lifes[i].lock()) {
            if (static_cast<int>(i) < lifeCount) {
                l->SetSprite(Tex[path + "LifeCount_3"]);
            }
            else if (static_cast<int>(i) == lifeCount && fragments > 0) {
                int fragIndex = Mathf::Min(fragments, maxFragments);
                l->SetSprite(Tex[path + "LifeCount_" + std::to_string(fragIndex)]);
            }
            else {
                l->SetSprite(Tex[path + "LifeCount_0"]);
            }
        }
    }

    if (auto f = lifeFragmentsText.lock()) {
        f->SetText(std::to_string(fragments) + "/" + std::to_string(maxFragments));
    }
}

//----------------------------------
// ボム表示更新
//----------------------------------
void HUDManager::UpdateBomb() {
    auto& bombMgr = GameManager::GetInstance().GetBombManager();
    const int bombCount = bombMgr.GetBombs();
    const int fragments = bombMgr.GetFragments();
    const int maxFragments = bombMgr.GetMaxFragments();

    std::string path = "UI/Item:";

    for (size_t i = 0; i < bombs.size(); ++i) {
        if (auto b = bombs[i].lock()) {
            if (static_cast<int>(i) < bombCount) {
                b->SetSprite(Tex[path + "SpCount_3"]);
            }
            else if (static_cast<int>(i) == bombCount && fragments > 0) {
                int fragIndex = Mathf::Min(fragments, maxFragments);
                b->SetSprite(Tex[path + "SpCount_" + std::to_string(fragIndex)]);
            }
            else {
                b->SetSprite(Tex[path + "SpCount_0"]);
            }
        }
    }

    if (auto f = bombFragmentsText.lock()) {
        f->SetText(std::to_string(fragments) + "/" + std::to_string(maxFragments));
    }
}

//----------------------------------
// 霊力表示更新
//----------------------------------
void HUDManager::UpdatePower() {
    auto& powerMgr = GameManager::GetInstance().GetPowerManager();

    if (auto reiryoku = reiryokuText.lock()) {
        float currentPower = powerMgr.GetPower();
        float maxPower = powerMgr.GetMaxPower();

        int totalCurrent = Mathf::Round<int>(currentPower * 100);
        int currentInt = totalCurrent / 100;
        int currentFrac = totalCurrent % 100;

        int totalMax = Mathf::Round<int>(maxPower * 100);
        int maxInt = totalMax / 100;
        int maxFrac = totalMax % 100;

        std::ostringstream oss;
        oss << currentInt << ".<size=60>";
        if (currentFrac < 10) oss << "0";
        oss << currentFrac << "</size>/";
        oss << maxInt << ".<size=60>";
        if (maxFrac < 10) oss << "0";
        oss << maxFrac << "</size>";

        reiryoku->SetText(oss.str());
    }
}

//----------------------------------
// 加点表示更新
//----------------------------------
void HUDManager::UpdateAddScore() {
    auto& scoreMgr = GameManager::GetInstance().GetScoreManager();
    if (auto tokuten = addScoreText.lock()) {
        tokuten->SetText(std::to_string(scoreMgr.GetScorePerItem()));
    }
}

//----------------------------------
// グレイズ更新
//----------------------------------
void HUDManager::UpdateGraze() {
    auto& grazeMgr = GameManager::GetInstance().GetGrazeManager();

    if (auto graze = grazeCountText.lock()) {
        graze->SetText(std::to_string(grazeMgr.GetGrazeCount()));
    }
    if (auto total = totalGrazeCountText.lock()) {
        total->SetText(std::to_string(grazeMgr.GetTotalGraze()));
    }
}

//----------------------------------
// FPS表示更新
//----------------------------------
void HUDManager::UpdateFPS() {
    if (auto fps = fpsText.lock()) {
        float fpsValue = Time.GetFPS();
        fps->SetText(System::Utility::Format::FormatString("{0:.1f}fps", fpsValue));

        if (fpsValue >= 40) {
            fps->SetColor(Color(255, 255, 255));
        }
        else if (fpsValue >= 30) {
            fps->SetColor(Color(200, 200, 255));
        }
        else if (fpsValue >= 20) {
            fps->SetColor(Color(100, 100, 255));
        }
        else {
            fps->SetColor(Color(0, 0, 255));
        }
    }
}

//----------------------------------
// ポーズ画面更新 (キーリピート + ループ選択)
//----------------------------------
void HUDManager::UpdatePause() {
    if (isFlashing) return;

    auto& gm = GameManager::GetInstance();
    bool isPause    = gm.IsPause();
    bool isGameOver = gm.IsGameOver();
    bool isNoCancel = gm.GetLifeManager().IsContinueCheck();

    if (auto p = pauseObject.lock()) {
        if (!isPause) {
            if (!pauseInitialized) {
                pauseInitialized = true;
                pauseState = PauseState::Main;
                mainSelectionIndex = 0;
                confirmSelectionIndex = 0;
                time = 0;
                if (auto m = MusicController::GetInstance()) {
                    m->ResumeBGM();
                    m->SetGameOverClip(nullptr);
                }
            }
            p->SetActive(false);
            return;
        }

        if (pauseInitialized) {
            pauseInitialized = false;
            pauseState = PauseState::Main;
            mainSelectionIndex = 0;
            confirmSelectionIndex = 0;
            time = 0;
            p->SetActive(false);
            if (auto m = MusicController::GetInstance()) {
                if (!isGameOver) {
                    m->OneShotAudio("pause");
                    m->PauseBGM();
                }
                else {
                    m->SetGameOverClip(Sounds["bgm_gameOver"]);
                    m->TriggerGameOver();
                }
            }
        }

        // ----- モード選択 ----- 
        if (!isGameOver) {
            UpdatePauseMenu(p);
        }
        else {
            UpdateGameOverMenu(p);
        }
    }
}

void HUDManager::UpdatePauseMenu(std::shared_ptr<GameObject> p) {
 
    // 現在のメニュー配列
    std::vector<std::weak_ptr<Image>>& currentMenu =
        (pauseState == PauseState::Main) ? pauseMenuItems : confirmMenuItems;
    size_t& currentIndex = (pauseState == PauseState::Main) ? mainSelectionIndex : confirmSelectionIndex;

    // 入力処理
    if (HandleMenuInput(currentIndex, currentMenu.size())) {
        if (!currentMenu.empty()) {
            if (auto img = currentMenu[currentIndex].lock()) {
                System::CoroutineManager::GetInstance().StartCoroutine(ShakeImage(img, 0.1f, 3.0f, 25.0f));
            }
        }
    };

    // 決定キー
    if (Input.IsKeyDown(KeyCode::Z) || Input.IsPadButtonDown<0>(PadCode::A)) {
        
        if (auto music = MusicController::GetInstance()) {
            if (auto sound = Sounds["ui_ok"]->Clone()) {
                music->OneShotAudio(sound);
            }
        }

        if (pauseState == PauseState::Main) {
            switch (mainSelectionIndex) {
            case 0:
                GameManager::GetInstance().OnPause(); // 即時再開
                break;
            case 1:
            case 2:
                OnCheck();
                break;
            }
        }
        else if (pauseState == PauseState::Confirm) {
            auto currentMenu = confirmMenuItems;
            if (!currentMenu.empty()) {
                if (auto img = currentMenu[confirmSelectionIndex].lock()) {
                    if (!isFlashing) {
                        isFlashing = true;
                        System::CoroutineManager::GetInstance().StartCoroutine(
                            FlashAndExecute(img, [this]() {
                                isFlashing = false;
                                if (auto p = pauseObject.lock()) {
                                    if (confirmSelectionIndex == 0) {
                                        switch (mainSelectionIndex) {
                                        case 1:
                                            SceneManager::GetInstance().SetTransitor<TileTransitor>(30);
                                            SceneManager::GetInstance().LoadScene(SceneType::Title);
                                            break;
                                        case 2:
                                            SceneManager::GetInstance().SetTransitor<TileTransitor>(60);
                                            SceneManager::GetInstance().LoadScene(SceneType::Game, true);

                                            break;
                                        }
                                    }
                                    else {
                                        // No → 確認画面閉じる
                                        if (auto check = p->transform->FindByPath("Pause/PauseMenu/check"))
                                            check->GetGameObject()->SetActive(false);
                                        pauseState = PauseState::Main;
                                        time = 0.0f;
                                    }
                                }
                                })
                        );
                    }
                }
            }
        }
    }

    // ハイライト更新
    UpdateMenuHighlight(currentMenu, currentIndex);

    if (p->IsActive())
    {
        // ESCキャンセル
        if (Input.IsKeyDown(KeyCode::ESCAPE) || Input.IsPadButtonDown<0>(PadCode::Start)) {
            if (pauseState == PauseState::Confirm) {
                if (auto check = p->transform->FindByPath("Pause/PauseMenu/check"))
                    check->GetGameObject()->SetActive(false);
                pauseState = PauseState::Main;
                time = 0.0f;
            }
            else {
                GameManager::GetInstance().OnPause(); // ポーズ解除
            }
        }
        else if (pauseState == PauseState::Confirm){
            if (Input.IsKeyDown(KeyCode::X) || Input.IsPadButtonDown<0>(PadCode::B)){
               
                if (auto music = MusicController::GetInstance()) {
                    if (auto sound = Sounds["ui_no"]) {
                        music->OneShotAudio(sound->Clone());
                    }
                }

                if (confirmSelectionIndex == 1){
                    if (auto check = p->transform->FindByPath("Pause/PauseMenu/check")) {
                        check->GetGameObject()->SetActive(false);
                        pauseState = PauseState::Main;
                        time = 0.0f;
                    }
                }
                else {
                    confirmSelectionIndex = 1;
                }
            }
        }
    }
    else {
        p->SetActive(true);
    }
}

void HUDManager::UpdateGameOverMenu(std::shared_ptr<GameObject> p) {

    if (p->IsActive())
    {
        auto& life = GameManager::GetInstance().GetLifeManager();
        bool canContinue = life.IsContinueCheck();
        if (!canContinue && mainSelectionIndex == 0) {
            mainSelectionIndex = 1;
        }
        // 現在のメニュー配列
        std::vector<std::weak_ptr<Image>>& currentMenu = pauseMenuItems;
        size_t& currentIndex = mainSelectionIndex;

        // 入力処理
        if (HandleMenuInput(currentIndex, currentMenu.size())) {
            if (!currentMenu.empty()) {
                if (auto img = currentMenu[currentIndex].lock()) {
                    System::CoroutineManager::GetInstance().StartCoroutine(ShakeImage(img, 0.1f, 3.0f, 25.0f));
                }
            }
        };


        // 決定キー
        if (Input.IsKeyDown(KeyCode::Z) || Input.IsPadButtonDown<0>(PadCode::A)) {
            
            if (auto music = MusicController::GetInstance()) {
                if (auto sound = Sounds["ui_ok"]) {
                    music->OneShotAudio(sound->Clone());
                }
            }

            if (auto img = currentMenu[currentIndex].lock()) {
                if (!isFlashing) {
                    isFlashing = true;
                    System::CoroutineManager::GetInstance().StartCoroutine(
                        FlashAndExecute(img, [this,p]() {
                            isFlashing = false;
                            switch (mainSelectionIndex) {
                            case 0:
                                GameManager::GetInstance().Continue();
                                p->SetActive(false);
                                ChangeDefaultPauseMenu();
                                if (auto m = MusicController::GetInstance())
                                    m->ContinueFromGameOverAndResume();
                                break;
                            case 1:
                                SceneManager::GetInstance().SetTransitor<TileTransitor>(30);
                                SceneManager::GetInstance().LoadScene(SceneType::Title);
                                break;
                            case 2:
                                SceneManager::GetInstance().SetTransitor<TileTransitor>(60);
                                SceneManager::GetInstance().LoadScene(SceneType::Game, true);
                                break;
                            }
                        }
                        )
                    );
                }
            }
        }

        // ハイライト更新
        UpdateMenuHighlight(currentMenu, currentIndex);
    }
    else {
        p->SetActive(true);
    }
}

void HUDManager::OnCheck() {
    if (auto p = pauseObject.lock()) {
        auto& currentMenu = pauseMenuItems; // Main メニューを想定
        if (currentMenu.empty()) return;

        if (auto img = currentMenu[mainSelectionIndex].lock()) {
            if (isFlashing) return; // 点滅中は無視
            isFlashing = true;

            System::CoroutineManager::GetInstance().StartCoroutine(
                FlashAndExecute(img, [this]() {
                    isFlashing = false;
                    if (auto p = pauseObject.lock()) {
                        if (auto check = p->transform->FindByPath("Pause/PauseMenu/check")) {
                            check->GetGameObject()->SetActive(true);
                            pauseState = PauseState::Confirm;
                            confirmSelectionIndex = 1;
                            time = 0.0f;
                        }
                    }
                    })
            );
        }
    }
}


void HUDManager::UpdateMenuHighlight(const std::vector<std::weak_ptr<Image>>& menu, size_t selectionIndex) {
    time += Time.unscaledDeltaTime;
    for (size_t i = 0; i < menu.size(); ++i) {
        if (auto img = menu[i].lock()) {
            if (i == selectionIndex) {
                // 白→ピンクの色アニメ
                float t = (std::sin(time * 3.0f) + 1.0f) * 0.5f;
                uint8_t r = 255;
                uint8_t g = static_cast<uint8_t>(255 - (255 - 148) * t);
                uint8_t b = static_cast<uint8_t>(255 - (255 - 148) * t);
                img->SetColor(Color(r, g, b, 255));

                // 横振動
                ShakeImage(img, 0.2f, 3.0f, 25.0f);
            }
            else {
                img->SetColor(Color(100, 100, 100, 180));
            }
        }
    }
}

System::Coroutine HUDManager::FlashAndExecute(std::shared_ptr<Image> img, std::function<void()> callback, int flashCount, float interval) {
    if (!img) co_return;
    auto originalColor = img->GetColor();

    for (int i = 0; i < flashCount; ++i) {
        img->SetColor(Color::Gray()); // 点滅色
        _yield new System::WaitForSecondsRealtime(interval);
        img->SetColor(Color::White());
        _yield new System::WaitForSecondsRealtime(interval);
    }
    img->SetColor(originalColor);
    if (callback) callback();
}

bool HUDManager::HandleMenuInput(size_t& selectionIndex, size_t menuCount) {
    bool isEvent = false;
    // キーボード入力を処理
    if (HandleMenuKeyInput(selectionIndex, menuCount)) {
        time = 0.0f;
        isEvent = true;
    }
    // パッド入力を処理
    else if (HandleMenuPadInput(selectionIndex, menuCount)) {
        time = 0.0f;
        isEvent = true;
    }

    if (isEvent) {
        if (auto music = MusicController::GetInstance()) {
            if (auto sound = Sounds["ui_change"]) {
                music->OneShotAudio(sound->Clone());
            }
        }
    }

    return isEvent;
}

//----------------------------------
// キーボード入力処理
//----------------------------------
bool HUDManager::HandleMenuKeyInput(size_t& selectionIndex, size_t menuCount) {
    static KeyCode currentKey = KeyCode::None;
    static float keyRepeatTimer = 0.0f;
    static bool keyInitialRepeatDone = false;

    const float initialDelay = 0.5f;    // 最初の遅延時間
    const float repeatInterval = 0.1f;  // 連続入力間隔

    if (Input.IsKeyDown(KeyCode::UpArrow) || Input.IsKeyDown(KeyCode::DownArrow)) {
        int dir = Input.IsKeyDown(KeyCode::DownArrow) ? 1 : -1;
        MoveSelection(dir, selectionIndex, menuCount);

        currentKey = Input.IsKeyDown(KeyCode::DownArrow) ? KeyCode::DownArrow : KeyCode::UpArrow;
        keyRepeatTimer = 0.0f;
        keyInitialRepeatDone = false;
        return true;
    }
    else if (Input.IsKey(currentKey)) {
        keyRepeatTimer += Time.unscaledDeltaTime;
        if ((!keyInitialRepeatDone && keyRepeatTimer >= initialDelay) ||
            (keyInitialRepeatDone && keyRepeatTimer >= repeatInterval)) {
            int dir = (currentKey == KeyCode::DownArrow) ? 1 : -1;
            MoveSelection(dir, selectionIndex, menuCount);
            keyRepeatTimer = 0.0f;
            keyInitialRepeatDone = true;
            return true;
        }
    }
    else {
        currentKey = KeyCode::None;
        keyRepeatTimer = 0.0f;
        keyInitialRepeatDone = false;
    }

    return false;
}

//----------------------------------
// パッド入力処理
//----------------------------------
bool HUDManager::HandleMenuPadInput(size_t& selectionIndex, size_t menuCount) {
    static int currentPadAxis = 0;
    static float padRepeatTimer = 0.0f;
    static bool padInitialRepeatDone = false;

    const float initialDelay = 0.5f;
    const float repeatInterval = 0.1f;

    if (!Input.IsActivePad<0>()) {
        currentPadAxis = 0;
        padRepeatTimer = 0.0f;
        padInitialRepeatDone = false;
        return false;
    }

    auto stick = Input.GetLstick<0>(0.75f);
    int axis = 0;
    if (stick.y > 0.5f) axis = 1;
    else if (stick.y < -0.5f) axis = -1;

    if (Input.IsPadButton<0>(PadCode::Up))  axis = 1;
    if (Input.IsPadButton<0>(PadCode::Down)) axis = -1;

    if (axis != 0 && axis != currentPadAxis) {
        MoveSelection(-axis, selectionIndex, menuCount);
        currentPadAxis = axis;
        padRepeatTimer = 0.0f;
        padInitialRepeatDone = false;
        return true;
    }
    else if (axis != 0 && axis == currentPadAxis) {
        padRepeatTimer += Time.unscaledDeltaTime;
        if ((!padInitialRepeatDone && padRepeatTimer >= initialDelay) ||
            (padInitialRepeatDone && padRepeatTimer >= repeatInterval)) {
            MoveSelection(-axis, selectionIndex, menuCount);
            padRepeatTimer = 0.0f;
            padInitialRepeatDone = true;
            return true;
        }
    }
    else {
        currentPadAxis = 0;
        padRepeatTimer = 0.0f;
        padInitialRepeatDone = false;
    }

    return false;
}

//----------------------------------
// 選択移動（ループ）
//
// dir: +1 = 下へ, -1 = 上へ
//----------------------------------
void HUDManager::MoveSelection(int dir, size_t& selectionIndex, size_t count) {
    auto& life = GameManager::GetInstance().GetLifeManager();
    bool canContinue = life.IsContinueCheck();
    if (canContinue) {
        if (dir > 0) selectionIndex = (selectionIndex + 1) % count;
        else if (dir < 0) selectionIndex = (selectionIndex == 0) ? count - 1 : selectionIndex - 1;
    }
    else {
        size_t minIndex = 1;
        size_t maxIndex = count - 1;

        if (dir > 0) {
            selectionIndex++;
            if (selectionIndex > maxIndex) selectionIndex = minIndex;
        }
        else if (dir < 0) {
            if (selectionIndex <= minIndex) selectionIndex = maxIndex;
            else selectionIndex--;
        }
    }
}

System::Coroutine HUDManager::ShakeImage(std::shared_ptr<Image> img, float duration, float amplitudeX, float frequency) {
    if (!img) co_return;

    auto rectTransform = img->GetRectTransform();
    if (!rectTransform) co_return;

    Vector2D basePos = rectTransform->anchoredPosition;
    float elapsed = 0.0f;

    while (elapsed < duration) {
        // 横方向のみ振動
        float offsetX = amplitudeX * std::sin(elapsed * frequency * 2.0f * 3.14159f) + Random.Range(-1.0f, 1.0f);
        float offsetY = 0.0f; // Y方向は固定

        rectTransform->anchoredPosition = basePos + Vector2D(offsetX, offsetY);

        _yield null;
        elapsed += Time.unscaledDeltaTime;
    }

    rectTransform->anchoredPosition = basePos; // 元の位置に戻す
}

// 0. Title
// 1.... Menu類
static std::vector<std::string> gameOverImageKey = {
    "UI/Pause:pause_11",
    "UI/Pause:pause_12",
    "UI/Pause:pause_2",
    "UI/Pause:pause_7"
};

static std::vector<std::string> defaultImageKey = {
    "UI/Pause:pause_0",
    "UI/Pause:pause_1",
    "UI/Pause:pause_2",
    "UI/Pause:pause_7"
};

void HUDManager::ChangeDefaultPauseMenu() {
    if (auto p = pauseObject.lock())
    {
        if (auto title = p->transform->FindByPath("Pause/PauseMenu/Title"))
        {
            if (auto i = title->GetGameObject()->GetAppBase<Image>()) i->SetSprite(Tex[defaultImageKey[0]]);
        }

        pauseMenuItems.clear();

        if (auto g = p->transform->FindByPath("Pause/PauseMenu/Nums"))
        {
            auto children = g->GetChildren();

            size_t count = std::min<size_t>(3, children.size());

            for (size_t i = 0; i < count; i++)
            {
                if (auto img = children[i]->GetGameObject()->GetAppBase<Image>())
                {
                    img->SetSprite(Tex[defaultImageKey[i + 1]]);
                    pauseMenuItems.push_back(img);
                }
            }
        }
    }
}

void HUDManager::ChangeGameOverPauseMenu() {
    if (auto p = pauseObject.lock())
    {
        if (auto title = p->transform->FindByPath("Pause/PauseMenu/Title"))
        {
            if (auto i = title->GetGameObject()->GetAppBase<Image>()) i->SetSprite(Tex[gameOverImageKey[0]]);
        }

        pauseMenuItems.clear();  

        if (auto g = p->transform->FindByPath("Pause/PauseMenu/Nums"))
        {
            auto children = g->GetChildren();

            size_t count = std::min<size_t>(3, children.size());

            for (size_t i = 0; i < count; i++)
            {
                if (auto img = children[i]->GetGameObject()->GetAppBase<Image>())
                {
                    img->SetSprite(Tex[gameOverImageKey[i + 1]]); 
                    pauseMenuItems.push_back(img);
                }
            }
        }
    }
}

//----------------------------------
// リセット
//----------------------------------
void HUDManager::Reset() {
    hiText.reset();
    scoreText.reset();
    lifes.clear();
    lifeFragmentsText.reset();
    bombs.clear();
    bombFragmentsText.reset();
    reiryokuText.reset();
    addScoreText.reset();
    grazeCountText.reset();
    totalGrazeCountText.reset();
    fpsText.reset();

    pauseMenuItems.clear();
    confirmMenuItems.clear();
    pauseObject.reset();
    pauseInitialized = true;

    displayedHiScore = 0;
    displayedScore = 0;

    // キーリピート関係の初期化
    currentKey = KeyCode::None;
    keyRepeatTimer = 0.0f;
    keyInitialRepeatDone = false;
    if (auto m = MusicController::GetInstance())
        m->SetGameOverClip(nullptr);
}
