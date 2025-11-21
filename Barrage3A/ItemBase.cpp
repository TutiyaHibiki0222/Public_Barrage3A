/*
    ◆ ItemBase.cpp

    クラス名        : ItemBase クラス
    作成日          : 2025/07/18 (金) 19:19:47
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "ItemBase.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "ItemManager.h"
#include "GameManager.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"
#include "Texture2DManager.hpp"
#include "ScoreEffect.h"
using namespace GameEngine;

// コンストラクタ 
ItemBase::ItemBase() : AppBase("ItemBase"){

}
ItemBase::ItemBase(std::shared_ptr<GameObject> owner) : AppBase("ItemBase", owner) {

}

// AppBase Event.

void ItemBase::Awake() {
    // 初速はY方向にのみ与える（東方らしい落下）
    velocity = Vector2D(0, 3.0f);             // 初速だけ、あとは重力で自然落下
    ItemManager::GetInstance().AddItem(shared_from_this());
}

void ItemBase::Start() {
	sprite = gameObject->GetAppBase<SpriteRenderer>();
}

void ItemBase::Update() {
    if (GameManager::GetInstance().IsPause()) return;

    auto player = GameManager::GetInstance().GetPlayer();

    if (!player) return;
    if (!transform) return;

    const auto playerPos = player->transform->position;
    Vector2D toPlayer = playerPos - transform->position;
    float distSqr = toPlayer.LengthSquared();

    constexpr float collectDistance = 16.0f;
    constexpr float attractRange = 50.0f;
    constexpr float attractStrength = 0.5f;
    // ----------------------------
    // 回転アニメーション（減速付き）
    // ----------------------------
    if (spinTimer > 0.0f) {
        transform->rotation += initialSpinSpeed;
        spinTimer -= 1.0f / 60.0f;  // 1フレームごとに減少（60FPS基準）
        if (spinTimer < 0.0f) {
            spinTimer = 0.0f;
			transform->rotation = 0.0f;  // 回転をリセット
        }
    }
    // ----------------------------
    // 1. 上昇経験の記録
    // ----------------------------
    if (velocity.y < 0.0f) {
        hasRisen = true;
    }

    // ----------------------------
    // 2. 吸収判定（完全吸収）
    // ----------------------------
    if (distSqr < collectDistance * collectDistance) {
        OnItemEvent();
        Destroy();
        return;
    }

    // ----------------------------
    // 3. 吸収挙動
    // ----------------------------
    if (isAutoCollect && hasRisen) {
        if (distSqr > 0.001f) {
            toPlayer.Normalize();
            transform->position += toPlayer * collectSpeed;
        }
    }
    else {
        // ▼ 重力落下
        velocity.y = Mathf::Max(velocity.y + gravity, maxFallSpeed);
        transform->position += velocity;

        // ▼ 近距離吸引（吸引範囲内ならじわじわ引っ張られる）
        if (distSqr < attractRange * attractRange) {
            isAutoCollect = true;
        }
    }

    // ----------------------------
    // 4. 画面外（下）に落ちたら削除
    // ----------------------------
    float halfScreenHeight = Window::GetInstance().GetMaxVector2D().y / 2;
    if (transform->position.y < -halfScreenHeight - 32) {
        Destroy();
    }
}



void ItemBase::OnDestroy() {
    ItemManager::GetInstance().RemoveItem(shared_from_this());
}

void ItemBase::OnItemEvent() {
    auto& gm = GameManager::GetInstance();
    auto  player = gm.GetPlayer();
    float bonus = 1.0f;

    float fullPointLine = 160.0f;

    if (player) {
        float playerY = player->transform->position.y;
        if (playerY >= fullPointLine) {
            bonus = 2.0f; // FULL点
        }
        else {
            float minY = -Window::GetInstance().GetMaxVector2D().y;
            float t = std::clamp((playerY - minY) / (fullPointLine - minY), 0.0f, 1.0f);
            bonus = 1.0f + t * 0.99f;
        }
    }

    const Color fullScoreColor(255, 255, 0);

    switch (itemType) {
    case ItemType::Score: {
        int base = 6250;
        int pts = int(base * bonus);
        gm.GetScoreManager().Add(pts);

        auto effect = GameObject::Instantiate("ScoreEffect")->AddAppBase<ScoreEffect>();
        effect->SetScore(pts);
        effect->GetGameObject()->transform->position = transform->position;
        effect->SetFont(Texture2DManager::GetInstance().GetSpriteFont("scoreNumber"));
        if (player && player->transform->position.y >= fullPointLine)
            effect->SetColor(fullScoreColor);
        break;
    }
    case ItemType::ScoreBig: {
        int base = 40000;
        int pts = int(base * bonus);
        gm.GetScoreManager().Add(pts);

        auto effect = GameObject::Instantiate("ScoreEffect")->AddAppBase<ScoreEffect>();
        effect->SetScore(pts);
        effect->GetGameObject()->transform->position = transform->position;
        effect->SetFont(Texture2DManager::GetInstance().GetSpriteFont("scoreNumber"));
        if (player && player->transform->position.y >= fullPointLine)
            effect->SetColor(fullScoreColor);
        break;
    }     
    case ItemType::Power:
        {
            GameManager::GetInstance().GetPowerManager().Add(0.01f);
            break;
        }
    case ItemType::PowerBig:
        {
            GameManager::GetInstance().GetPowerManager().Add(0.1f);
            break;
		}
    case ItemType::LifePiece:
        {
            GameManager::GetInstance().GetLifeManager().AddFragments(1);
            break;
        }
    case ItemType::LifeFull:
        {
            GameManager::GetInstance().GetLifeManager().AddLives(1);
            break;
        }
    case ItemType::BombPiece:
        {
            GameManager::GetInstance().GetBombManager().AddFragments(1);
            break;
        }
    case ItemType::BombFull:
        {
            GameManager::GetInstance().GetBombManager().AddBombs(1);
            break;
        }
        break;
    case ItemType::PointExtend:
        {
            
        }
        break;
    case ItemType::Custom:
        break;
    default:
        break;
    }

    if (auto audio = Sounds["item_get"]) {
        MusicController::GetInstance()->OneShotAudio(audio->Clone(),0.5f);
    }
}

void ItemBase::AutoCollect() {
    isAutoCollect = true;
}

void ItemBase::SetType(ItemType type) {
    itemType = type;
}

void ItemBase::SetSpinDuration(float time) {
    spinTimer = time;
}
