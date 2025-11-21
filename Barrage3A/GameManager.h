#pragma once
#include "headers.h"
#include "GameObject.h"
#include "ScoreManager.h"
#include "PowerManager.h"
#include "LifeManager.h"
#include "BombManager.h"
#include "GrazeManager.h"

class GameManager {
private:
    // プレイヤー
    std::weak_ptr<GameObject> player;
    Vector2D playerStartPos     = { -150, -300 };
    Vector2D playerRespawnPos   = { -150, -500 };

    // 状態フラグ
    bool isGameOver = false;
    bool isPause    = false;
    // 各種ステータス管理クラス
    ScoreManager scoreManager;
    PowerManager powerManager;
    LifeManager lifeManager;
    BombManager bombManager;
    GrazeManager grazeManager;

    GameManager() = default;

public:
    static GameManager& GetInstance();

    // コピー防止
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    // --- Getter ---
    ScoreManager&   GetScoreManager() { return scoreManager; }
    PowerManager&   GetPowerManager() { return powerManager; }
    LifeManager&    GetLifeManager()  { return lifeManager; }
    BombManager&    GetBombManager()  { return bombManager; }
    GrazeManager&   GetGrazeManager() { return grazeManager; }


    std::shared_ptr<GameObject> GetPlayer() const;
    Vector2D GetPlayerPosition() const;
    Vector2D GetPlayerStartPos() const { return playerStartPos; }
    Vector2D GetPlayerRespawnPos() const { return playerRespawnPos; }

    void SetPlayer(const std::shared_ptr<GameObject> player);

	bool IsGameOver()   const { return isGameOver;  }
    bool IsPause()      const { return isPause;     }
    // --- 初期化 ---
    void ResetGameState();

    void OnPlayerDead();

    void OnPause();
    void SetPause(bool set);

    void Continue();
    void OnGameOver();

};
