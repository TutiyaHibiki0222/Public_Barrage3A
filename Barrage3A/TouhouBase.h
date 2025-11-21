/*
    ◆ TouhouBase.h

    クラス名        : TouhouBase クラス
    作成日          : 2025/11/14 (金) 19:37:38
    最終変更日      : 
    作成者          : 
    概要            : 東方 の 基礎クラス
*/
#pragma once
#include "headers.h"
#include "AppBase.h"

// TouhouBase class and AppBase class
class TouhouBase : public AppBase {
private:    // 非公開.

protected:

    // コンストラクタ.
    TouhouBase();
    TouhouBase(std::shared_ptr<GameObject>);

    // AppBase 関数.
	void FixedUpdate()  override;
    void Update()       override;
	void LateUpdate()   override;

    virtual void TickFixed() {}
    virtual void Tick() {}
    virtual void TickLate() {}
private:
    bool IsGamePaused() const;
protected:
    virtual std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<TouhouBase>(*this);
    }
};