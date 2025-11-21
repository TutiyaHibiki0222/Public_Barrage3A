#include "Project.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TitleScene.h"
#include "GameScene.h"

SceneManager& SceneManager::GetInstance() {
    static SceneManager instance;
    return instance;
}

// 指定されたシーンに切り替える.
std::unique_ptr<Scene> SceneManager::CreateScene(SceneType type) {
    switch (type) {
    case SceneType::Title:
        return std::make_unique<TitleScene>();
    case SceneType::Game:
        return std::make_unique<GameScene>();
    case SceneType::Settings:
        return nullptr;
    case SceneType::GameOver:
        return nullptr;
    default:
        return nullptr;
    }
}

void SceneManager::Init(SceneType type) {
    // まずは 開いているsceneを解放.
    if (currentScene) {
        currentScene->Release();
    }
    // 次のシーンを切り替える.
    currentScene = CreateScene(type);
    // 切り替えたシーンを初期化.
    if (currentScene) {
        currentScene->Init();
    }

    scene = newScene;
    forceReload = false;
}

void SceneManager::LoadScene(SceneType type, bool reload) {
    if (type == newScene && !reload)        return;
    if(transitor && transitor->IsStart())   return;
    newScene    = type;
    forceReload = reload;
}

void SceneManager::ReLoadScene() {
    if (currentScene) {
		forceReload = true;
    }
}


void SceneManager::Update() {
    if (currentScene) currentScene->Update();
}

void SceneManager::UpdateTransitor() {
    if (transitor && transitor->IsStart()) {
        transitor->Update();

        if (transitor->IsEnd()) {
            transitor.reset();
            transitor = nullptr;
        }
    }
}

void SceneManager::Draw() {

    if (currentScene)   currentScene->Draw();


    if (scene != newScene || forceReload) {
        if (transitor) if (!transitor->IsStart()) transitor->Start(); // ここで描画開始
    }

    if (scene != newScene || forceReload) {
        Init(newScene);
    }
}

void SceneManager::DrawTransitor() {
    if (transitor) if (transitor->IsStart()) transitor->Draw();
}

void SceneManager::Release() {
    if (currentScene) {
        currentScene->Release();
    }
}