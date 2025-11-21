/*
    ◆ TouhouBase.cpp

    クラス名        : TouhouBase クラス
    作成日          : 2025/11/14 (金) 19:37:38
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "TouhouBase.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "GameManager.h"

// コンストラクタ 
TouhouBase::TouhouBase() : AppBase("TouhouBase"){

}
TouhouBase::TouhouBase(std::shared_ptr<GameObject> owner) : AppBase("TouhouBase", owner) {

}

void TouhouBase::FixedUpdate() {
    if (!IsGamePaused()) TickFixed();
}

void TouhouBase::Update() {
    if (!IsGamePaused()) Tick();
}

void TouhouBase::LateUpdate() {
    if (!IsGamePaused()) TickLate();
}

bool TouhouBase::IsGamePaused() const {
    return GameManager::GetInstance().IsPause();
}
