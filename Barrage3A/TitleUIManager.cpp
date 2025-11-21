/*
    ◆ TitleUIManager.cpp

    クラス名        : TitleUIManager クラス
    作成日          : 2025/07/15 (火) 21:48:15
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "TitleUIManager.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "MusicController.h"
// コンストラクタ 
TitleUIManager::TitleUIManager() : AppBase("TitleUIManager"){

}
TitleUIManager::TitleUIManager(std::shared_ptr<GameObject> owner) : AppBase("TitleUIManager", owner) {

}


// キーボード入力処理を分割
bool TitleUIManager::HandleKeyboardInput() {
    using namespace GameEngine::Utilities;
    if (Input.IsKeyDown(KeyCode::UpArrow) || Input.IsKeyDown(KeyCode::DownArrow)) {
        int dir = Input.IsKeyDown(KeyCode::DownArrow) ? 1 : -1;
        MoveSelection(dir);

        currentKey = Input.IsKeyDown(KeyCode::DownArrow) ? KeyCode::DownArrow : KeyCode::UpArrow;
        keyRepeatTimer = 0.0f;
        keyInitialRepeatDone = false;
        return true;
    }
    else if (Input.IsKey(currentKey)) {
        keyRepeatTimer += Time.deltaTime;
        if ((!keyInitialRepeatDone && keyRepeatTimer >= initialDelay) ||
            (keyInitialRepeatDone && keyRepeatTimer >= repeatInterval)) {
            int dir = (currentKey == KeyCode::DownArrow) ? 1 : -1;
            MoveSelection(dir);
            keyRepeatTimer = 0.0f;
            keyInitialRepeatDone = true;
            return true;
        }
    }
    else {
        // リセット
        currentKey = KeyCode::None;
        keyRepeatTimer = 0.0f;
        keyInitialRepeatDone = false;
    }
    return false;
}

// パッド入力処理を分割
bool TitleUIManager::HandlePadInput() {
    using namespace GameEngine::Utilities;
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

    if(Input.IsPadButton<0>(PadCode::Up)) {
        axis = 1;
    }
    else if(Input.IsPadButton<0>(PadCode::Down)) {
        axis = -1;
	}

    if (axis != 0 && axis != currentPadAxis) {
        MoveSelection(-axis);
        currentPadAxis = axis;
        padRepeatTimer = 0.0f;
        padInitialRepeatDone = false;
        return true;
    }
    else if (axis != 0 && axis == currentPadAxis) {
        padRepeatTimer += Time.deltaTime;
        if ((!padInitialRepeatDone && padRepeatTimer >= initialDelay) ||
            (padInitialRepeatDone && padRepeatTimer >= repeatInterval)) {
            MoveSelection(-axis);
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

void TitleUIManager::AddSelectPanel(SelectPanel panle) {
    panels.push_back(panle);
}

void TitleUIManager::AddPanel(const std::shared_ptr<Image>& panel, const std::string& normalKey, const std::string& selectedKey, const std::function<void()> onFunction, bool isNoSelect) {
    SelectPanel sp;
    sp.panelImage = panel;
    sp.normalSpriteKey = normalKey;
    sp.selectedSpriteKey = selectedKey;
    sp.isNoSelected = isNoSelect;
    sp.onSelectAction = onFunction;
    panels.push_back(sp);
}

void TitleUIManager::MoveSelection(int direction) {
    if (panels.empty()) return;

    int size = (int)panels.size();
    int startIndex = index;

    do {
        index = (index + direction + size) % size;
        if (!panels[index].isNoSelected) break;
    } while (index != startIndex);

    UpdatePanelsSelection();
}

void TitleUIManager::UpdatePanelsSelection() {
    for (auto& panel : panels) {
        panel.isSelected = false;
    }

    if (!panels[index].isNoSelected) {
        panels[index].isSelected = true;
        StartCoroutine(Move(panels[index].panelImage.lock()));
    }

    for (auto& panel : panels) {
        panel.UpdateVisual(selectedColor,normalColor, ngSelectedColor);
    }
}

// 確定(Enter/Aボタン)の入力を検出
void TitleUIManager::HandleConfirm() {
    using namespace GameEngine::Utilities;
    if (Input.IsKeyDown(KeyCode::ENTER) || Input.IsKeyDown(KeyCode::Z) ||
        Input.IsPadButtonDown<0>(PadCode::A)) {
        if (auto music = MusicController::GetInstance()) {
            if (auto sound = Sounds["ui_ok"]) {
                music->OneShotAudio(sound);
            }
        }
        isShot = true;
        const auto& panel = panels[index];
        StartCoroutine(ConfirmSelection(panel));
    }
}


// フラッシュと完了コールバック
System::Coroutine TitleUIManager::ConfirmSelection(const SelectPanel& panel) {
    auto img = panel.panelImage.lock();
    if (!img) co_return;
    float elapsed = 0.0f;
    bool toggle = false;
    while (elapsed < panel.time) {
        panel.FlashVisual(toggle ? shotColor : normalColor);
        toggle = !toggle;
        float inner = 0.0f;
        while (inner < flashInterval) {
            _yield null;
            inner += Time.deltaTime;
            elapsed += Time.deltaTime;
        }
    }
    // 元に戻す
    panels[index].UpdateVisual(selectedColor, normalColor, ngSelectedColor);
    
    isShot = false;
    // コールバック実行
    if (panel.onSelectAction) panel.onSelectAction();
    co_return;
}


// 実際の選択時アクション
void TitleUIManager::ExecuteSelectedAction() {
    // ここに選択確定後の処理を記述
    // 例: シーン遷移やメニュー実行など
}


System::Coroutine TitleUIManager::Move(std::shared_ptr<Image> img) {

    if(!img) co_return;

    auto rectTransform = img->GetRectTransform();
    if (!rectTransform) co_return;

    Vector2D basePos = rectTransform->anchoredPosition;
    float t = 0.0f;

    while (t < 0.1f) {
        float offsetX = Random.Range(-3.0f, 3.0f); // X方向の振動（振幅2px）
        float offsetY = Random.Range(-2.5f, 1.5f); // Y方向の振動（振幅1.5px）
        rectTransform->anchoredPosition = basePos + Vector2D(offsetX, offsetY);
        _yield null;
        t += Time.deltaTime;
    }

    rectTransform->anchoredPosition = basePos;
}

// AppBase Event.
void TitleUIManager::Start() {
    index = 0;
    UpdatePanelsSelection();
}


void TitleUIManager::Update() {
    using namespace GameEngine::Utilities;

    if (isShot) return;

    bool inputDetected = false;

    // キーボード入力
    if (HandleKeyboardInput()) {
        inputDetected = true;

        // SE
        if (auto music = MusicController::GetInstance()) {
            if (auto sound = Sounds["ui_change"]) {
                music->OneShotAudio(sound);
            }
        }
    }
    // パッド入力
    else if constexpr (has_maxPad<0>) {
        if (HandlePadInput()) {
            inputDetected = true;
            // SE
            if (auto music = MusicController::GetInstance()) {
                if (auto sound = Sounds["ui_change"]) {
                    music->OneShotAudio(sound);
                }
            }
        }
    }

    HandleConfirm();
}
