/*
    ◆ ReimuHakurei.h

    クラス名        : ReimuHakurei クラス
    作成日          : 2025/11/14 (金) 23:00:52
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "PlayerBase.h"

// ReimuHakurei class and AppBase class
class ReimuHakurei : public PlayerBase {
private:    // 非公開.

public:     // 公開.

    // コンストラクタ.
    ReimuHakurei();
    ReimuHakurei(std::shared_ptr<GameObject>);

    void Initialize()   override;
    void Shoot()        override;
    void OptionShot(const bool isFocused, const std::vector<Vector2D>& poss) override;
    void Bomb()         override;
    void OnHit()        override;

protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<ReimuHakurei>(*this);
    }
};