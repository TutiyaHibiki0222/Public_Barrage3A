/*
    ◆ Pseudo3DBackgroundManager.cpp

    クラス名        : Pseudo3DBackgroundManager クラス
    作成日          : 2025/07/24 (木) 2:57:48
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "Pseudo3DBackgroundManager.h"
#include "RendererManager.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "GameManager.h"
// コンストラクタ 
Pseudo3DBackgroundManager::Pseudo3DBackgroundManager() : AppBase("Pseudo3DBackgroundManager"){

}
Pseudo3DBackgroundManager::Pseudo3DBackgroundManager(std::shared_ptr<GameObject> owner) : AppBase("Pseudo3DBackgroundManager", owner) {

}

// AppBase Event.

void Pseudo3DBackgroundManager::Awake() {
    // RendererManager に登録
    RendererManager::GetInstance().AddRenderer(shared_from_this());
}

void Pseudo3DBackgroundManager::Start() {
    if (background) {
        //background->Init();
    }
}

void Pseudo3DBackgroundManager::Update() {
    if (GameManager::GetInstance().IsPause()) return;
    if (background) {
        background->Update();
    }
}

void Pseudo3DBackgroundManager::OnDestroy() {
    RendererManager::GetInstance().Remove(shared_from_this());
}

void Pseudo3DBackgroundManager::Draw() {
    if (background) {
        background->Draw();
    }
}