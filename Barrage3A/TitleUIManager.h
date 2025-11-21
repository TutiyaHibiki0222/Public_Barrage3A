/*
    ◆ TitleUIManager.h

    クラス名        : TitleUIManager クラス
    作成日          : 2025/07/15 (火) 21:48:15
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "Image.h"
#include "Texture2DShortcut.hpp"
#include "AudioResourceShortcut.hpp"
#include "IDraw.h"

struct SelectPanel {
    std::weak_ptr<Image> panelImage;
    std::string normalSpriteKey;
    std::string selectedSpriteKey;
    bool isSelected = false;
    bool isNoSelected = false;

    float time = 1.0f;

    // 選択時アクションコールバック
    std::function<void()> onSelectAction;

    void UpdateVisual(const Color& selectedColor, const Color& normalColor, const Color& ngSelectedColor) {
        auto panel = panelImage.lock();
        if (!panel) return;

        if (isNoSelected) {
            panel->SetSprite(Tex[normalSpriteKey]);
            panel->SetColor(ngSelectedColor);
        }
        else if (isSelected) {
            panel->SetSprite(Tex[selectedSpriteKey]);
            panel->SetColor(selectedColor);
        }
        else {
            panel->SetSprite(Tex[normalSpriteKey]);
            panel->SetColor(normalColor);
        }
    }

    // フラッシュ用描画
    void FlashVisual(const Color& flashColor) const {
        auto panel = panelImage.lock();
        if (!panel) return;
        // 選択中スプライトは selectedSpriteKey を使用
        panel->SetSprite(Tex[selectedSpriteKey]);
        panel->SetColor(flashColor);
    }
};


// TitleUIManager class and AppBase class
class TitleUIManager : public AppBase {
private:    // 非公開.
    // キーボード入力用
    KeyCode currentKey = KeyCode::None;
    float keyRepeatTimer = 0.0f;
    bool keyInitialRepeatDone = false;

    // パッド入力用
    int currentPadAxis = 0;
    float padRepeatTimer = 0.0f;
    bool padInitialRepeatDone = false;

    float initialDelay = 0.5f;
    float repeatInterval = 0.1f;

    bool inputDetected = false;
    std::vector<SelectPanel> panels;

    KeyCode lastKey = KeyCode::None;

    Color ngSelectedColor   = Color(0x383838);

    Color selectedColor     = Color(0x20D0FF);      // 選択中のcolor.
    Color shotColor         = Color(0xFFF220);      // 決定時のcolor.
    Color normalColor       = Color(0xFFFFFF);      // 通常時のcolor.

    // フラッシュ設定
    bool  isShot = false;
    float flashDuration = 0.5f;
    float flashInterval = 0.05f;

    int index = 0;
public:     // 公開.

    // コンストラクタ.
    TitleUIManager();
    TitleUIManager(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Start()    override;
    void Update()   override;

    void AddSelectPanel(SelectPanel);
    void AddPanel(const std::shared_ptr<Image>&, const std::string&,const std::string&,const std::function<void()> = nullptr ,bool = false);

    // 確定処理
    void HandleConfirm();
    System::Coroutine ConfirmSelection(const SelectPanel& panel);
    void ExecuteSelectedAction();

    System::Coroutine Move(std::shared_ptr<Image>);
private:
    void MoveSelection(int direction);
    void UpdatePanelsSelection();

    bool HandleKeyboardInput();
    bool HandlePadInput();

protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<TitleUIManager>(*this);
    }
};