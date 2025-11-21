#include "GameManager.h"
#include "Mathf.h"
#include "HUDManager.h"

GameManager& GameManager::GetInstance() {
    static GameManager instance;
    return instance;
}

void GameManager::SetPlayer(const std::shared_ptr<GameObject> p) {
    player = p;
}

std::shared_ptr<GameObject> GameManager::GetPlayer() const {
    return player.lock();
}

Vector2D GameManager::GetPlayerPosition() const {
    if (auto p = GetPlayer()) {
        return p->transform->position;
    }
    return Vector2D();
}

// --- 初期化 ---

void GameManager::ResetGameState() {
    isGameOver = false;
    isPause = false;
    Time.timeScale = 1;
    scoreManager.Reset();
    powerManager.Reset();
    lifeManager.Reset();
    bombManager.Reset();
    grazeManager.Reset();
    player.reset();
}

System::Coroutine Change() {
    _yield new System::WaitForFrames(20);
    HUDManager::GetInstance().ChangeGameOverPauseMenu();
    GameManager::GetInstance().OnGameOver();
}

void GameManager::OnPlayerDead() {
    // ライフを1減らす
    lifeManager.OnDeath();
	powerManager.Add(-1);
    // ゲームオーバー判定
    if (lifeManager.GetLife() < 0 && !isGameOver) {
        isGameOver = true;
        System::CoroutineManager::GetInstance().StartCoroutine(Change());
    }
    else {
        bombManager.ResetOnRespawn();
    }
}

void GameManager::OnPause() {
    isPause = !isPause;

    if (isPause) Time.timeScale = 0;
    else         Time.timeScale = 1;
}

void GameManager::SetPause(bool set) {
    isPause = set;
}

void GameManager::Continue()
{
    isGameOver  = false;
    isPause     = false;
    Time.timeScale = 1;
    lifeManager.Continue();
    bombManager.ResetOnRespawn();
    powerManager.SetPower(powerManager.GetMaxPower());
}

void GameManager::OnGameOver() {
    Time.timeScale = 0;
    isPause = true;
}