/*
    ◆ Pseudo3DBackgroundManager.h

    クラス名        : Pseudo3DBackgroundManager クラス
    作成日          : 2025/07/24 (木) 2:57:48
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "IDraw.h"
#include "AbstractBackground.hpp"

// Pseudo3DBackgroundManager class and AppBase class
class Pseudo3DBackgroundManager : public AppBase ,public IRendererDraw,public std::enable_shared_from_this<Pseudo3DBackgroundManager>
{
private:    // 非公開.
    std::shared_ptr<AbstractBackground> background;
public:     // 公開.

    // コンストラクタ.
    Pseudo3DBackgroundManager();
    Pseudo3DBackgroundManager(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Awake() override;
    void Start()    override;
    void Update()   override;
    void OnDestroy() override;

    void Draw() override;
    int GetSortingOrder() const override { return -999999999; }

    void SetBackground(std::shared_ptr<AbstractBackground> bg) {
        background = bg;
        if (background) background->Init();
    }
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Pseudo3DBackgroundManager>(*this);
    }
};