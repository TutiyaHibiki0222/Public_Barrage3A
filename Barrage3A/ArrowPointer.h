/*
    ◆ ArrowPointer.h

    クラス名        : ArrowPointer クラス
    作成日          : 2025/07/21 (月) 17:14:17
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "ItemBase.h"
#include "SpriteRenderer.h"
#include "Texture2D.h" 

// ArrowPointer class and AppBase class
class ArrowPointer : public AppBase {
private:    // 非公開.
    std::weak_ptr<ItemBase> targetItem;
	std::weak_ptr<SpriteRenderer> spriteRenderer;
    std::shared_ptr<GameEngine::Sprite> itemSprite;
    std::shared_ptr<GameEngine::Sprite> arrawSprite;
public:     // 公開.

    // コンストラクタ.
    ArrowPointer();
    ArrowPointer(std::shared_ptr<GameObject>);

    void SetTarget(std::shared_ptr<ItemBase> item);
    void SetArrowSprite(const std::shared_ptr<GameEngine::Sprite>& arrowSprite) {
        arrawSprite = arrowSprite;
	}
    void Update() override;
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<ArrowPointer>(*this);
    }
};