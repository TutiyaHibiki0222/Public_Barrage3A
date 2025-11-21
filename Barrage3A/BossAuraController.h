/*
    ◆ BossAuraController.h

    クラス名        : BossAuraController クラス
    作成日          : 2025/07/27 (日) 22:45:50
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "Sprite3DRenderer.h"
// BossAuraController class and AppBase class
class BossAuraController : public AppBase {
private:    // 非公開.
	std::weak_ptr<Sprite3DRenderer> auraRenderer;
    float cycleXY   = 10.0f;
    float speedZ    = 60.0f;
	float time      = 0.0f;
    float maxX      = 48.0f;
    float maxY      = 32.0f;
public:     // 公開.

    // コンストラクタ.
    BossAuraController();
    BossAuraController(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Start()    override;
    void Update()   override;

    void SetAuraRenderer(std::shared_ptr<Sprite3DRenderer> renderer) {
        auraRenderer = renderer;
	}
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<BossAuraController>(*this);
    }
};