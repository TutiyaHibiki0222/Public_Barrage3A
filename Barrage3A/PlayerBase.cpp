/*
    ◆ PlayerBase.cpp

    クラス名        : PlayerBase クラス
    作成日          : 2025/11/14 (金) 21:38:23
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "PlayerBase.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "GameManager.h"
#include "ExplosionEffect.h"
#include "BulletManager.h"
#include "EnemyManager.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"

// コンストラクタ 
PlayerBase::PlayerBase() {}

void PlayerBase::Move() {
    float s = speed;
    if (IsFocus()) s /= 2.0f;

    Vector2D move = Input.GetLstick<0>();

    if (Input.IsAnyKey(KeyCode::A, KeyCode::LeftArrow)  || Input.IsPadButton<0>(PadCode::Left)) move.x = -1;
    if (Input.IsAnyKey(KeyCode::D, KeyCode::RightArrow) || Input.IsPadButton<0>(PadCode::Right))move.x =  1;
    if (Input.IsAnyKey(KeyCode::W, KeyCode::UpArrow)    || Input.IsPadButton<0>(PadCode::Up))   move.y =  1;
    if (Input.IsAnyKey(KeyCode::S, KeyCode::DownArrow)  || Input.IsPadButton<0>(PadCode::Down)) move.y = -1;

    transform->position += move.GetNormalize() * s * Time.deltaTime;

    if (animator) {
        animator->SetBool("isLeft"  , move.x < 0);
        animator->SetBool("isRight" , move.x > 0);
    }

    // 画面外に収める.
    ClampToScreen(transform->position, size, transform->scale);
}

void PlayerBase::ShotBom() {
    auto& bomMgr = GameManager::GetInstance().GetBombManager();
    if (bomMgr.IsBom()) {
        bomMgr.UseBomb();
        Bomb();
    }
}

void PlayerBase::Shot() {
    if (Input.IsKeyDown(KeyCode::X) || Input.IsPadButtonDown<0>(PadCode::B)) {
        ShotBom();
    }

    if (shotCount > 0) shotCount--;
    if (mgtmCount > 0) mgtmCount--;

    if ((Input.IsKey(KeyCode::Z) || Input.IsPadButton<0>(PadCode::A))) {
        if (shotCount == 0) Shoot();
        if (mgtmCount == 0) {
            OptionShot(IsFocus(), magatama->GetMagatamaPositions());
        }
    }
}

bool PlayerBase::IsFocus() const {
    return Input.IsKey(KeyCode::LeftShift) || Input.IsPadButton<0>(PadCode::RightBumper);
}

void PlayerBase::ClampToScreen(Vector2D& pos, const Vector2DInt& size, const Vector2D& scale) {
    const float LEFT_MARGIN     = 64.0f;
    const float RIGHT_MARGIN    = 350.0f;
    const float TOP_MARGIN      = 50.0f;
    const float BOTTOM_MARGIN   = 50.0f;

    float halfWidth     = size.x * 0.5f * scale.x;
    float halfHeight    = size.y * 0.5f * scale.y;

    pos.x = Mathf::Clamp(pos.x, -WIDTH  / 2 + LEFT_MARGIN   + halfWidth , WIDTH  / 2 - RIGHT_MARGIN - halfWidth );
    pos.y = Mathf::Clamp(pos.y, -HEIGHT / 2 + BOTTOM_MARGIN + halfHeight, HEIGHT / 2 - TOP_MARGIN   - halfHeight);
}

void PlayerBase::UpdateInvincibleColor(float time) {
    if (auto a = sprite.lock()) {
        bool flash = ((int)(time * 20) % 2 == 0);
        Color color = flash ? Color(0, 0, 255, 255) : Color(255, 255, 255, 255);
        a->GetColor() = color;
    }
}

void PlayerBase::UpdateNormal() {
    Move();
    Shot();
}

void PlayerBase::UpdateHit() {
    if (lifeCount > 0) {
        lifeCount--;

        // ボムで回避
        if (Input.IsKeyDown(KeyCode::X) || Input.IsPadButton<0>(PadCode::B)) {
            lifeCount = 6;
            ShotBom();
        }
    }
    else {
        HandleDeath();  // ライフが尽きたら死亡処理
    }
}

void PlayerBase::UpdateInvincible() {
    invincibleTimer += Time.deltaTime;
    UpdateInvincibleColor(invincibleTimer);
    if (isRespawning) {

        float t = Mathf::Min(invincibleTimer / invincibleTime, 1.0f);
        auto& mg = GameManager::GetInstance();
        transform->position = Vector2D::Lerp(mg.GetPlayerRespawnPos(), mg.GetPlayerStartPos(), t);
        if (t >= 0.3) {
            BulletManager::Instance().AllDestroyLayer(Layer::EnemyBullet);
            if (t <= 0.4) {
                EnemyManager::GetInstance().AllEnemyDamage(20);
            }
        }
        if (t >= 1.0f) {
            isRespawning = false;
            invincibleTimer = 0.0f;
        }
    }
    else {

        Move();
        Shot();

        if (invincibleTimer >= 5.0f) {
            state = NORMAL;
            invincibleTimer = 0.0f;
            lifeCount = 6;
            if (auto a = sprite.lock()) {
                a->GetColor() = Color(255, 255, 255, 255);
            }
        }
    }

}

// 死亡時の処理をまとめる関数
void PlayerBase::HandleDeath() {
    if (gameObject->IsActive()) {
        // 爆発エフェクト生成
        GameObject::Instantiate("ExplosionEffect")
            ->AddAppBase<ExplosionEffect>()
            ->SetCenterPosition(transform->position);
        // プレイヤーの位置をリスポーン位置にセット
        transform->position = GameManager::GetInstance().GetPlayerRespawnPos();
        // 無敵状態開始準備
        invincibleTimer = 0.0f;
        isRespawning = true;
        state = INVINCIBLE;
        // マガタマをリセット
        if (magatama) {
            magatama->SetMagatamaCount(0);
        }
        prevCount = -1;
        // コントローラー振動
        Input.SetPadVibration<0>(500, 0.5f);

        // アニメーションの左右フラグリセット
        if (auto a = animator.lock()) {
            a->SetBool("isLeft", false);
            a->SetBool("isRight", false);
        }
        GameManager::GetInstance().OnPlayerDead();
    }
}

void PlayerBase::Awake() {
    gameObject->AddAppBase<CircleCollider>()->SetRadius(3);
    magatama = gameObject->AddAppBase<MagatamaController>();
    prevCount = (int)GameManager::GetInstance().GetPowerManager().GetPower();
}

void PlayerBase::Start() {
    sprite = gameObject->GetAppBase<SpriteRenderer>();
    size = sprite.lock()->GetSize();
    animator = gameObject->GetAppBase<Animator>();
    Initialize();
    magatama->SetMagatamaCount(prevCount);
}

void PlayerBase::Tick() {

    if (Input.IsKeyDown(KeyCode::ESCAPE) || Input.IsPadButtonDown<0>(PadCode::Start)) {
        GameManager::GetInstance().OnPause();
    }

    int currentCount = (int)GameManager::GetInstance().GetPowerManager().GetPower();
    if (currentCount != prevCount && !isRespawning) {
        if (magatama) {
            magatama->SetMagatamaCount(currentCount);
        }
        prevCount = currentCount;
    }

    switch (state) {
    case NORMAL:     UpdateNormal();     break;
    case HIT:        UpdateHit();        break;
    case INVINCIBLE: UpdateInvincible(); break;
    }
}

void PlayerBase::OnCollisionEnter(GameObject* other) {
    Debug::Print("ヒット");
    std::string tag = other->GetTag();

    if ((tag == "Enemy" || tag == "EnemyBullet") && state != INVINCIBLE) {
        state = HIT;
        lifeCount = 0;
        // 被弾音再生
        auto ms = MusicController::GetInstance();
        if (ms) {
            ms->OneShotAudio(Sounds["playerHit"]);
        }
        OnHit();
    }
}