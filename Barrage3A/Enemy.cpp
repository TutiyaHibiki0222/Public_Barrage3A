/*
    ◆ Enemy.cpp

    クラス名        : Enemy クラス
    作成日          : 2025/07/17 (木) 9:41:58
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/
#include "Enemy.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "EnemyManager.h"
#include "EnemyScript.h"
#include "GameEngine.h"
#include "BulletManager.h"
#include "ShotScriptManager.h"
#include "PlayerBullet.h"
#include "ItemManager.h"
#include "Prefab.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"
#include "GameManager.h"
Enemy::Enemy() : AppBase("Enemy") {}
Enemy::Enemy(std::shared_ptr<GameObject> o) : AppBase("Enemy", o) {}

void Enemy::Awake() {
    EnemyManager::GetInstance().AddEnemyObj(shared_from_this());
}

void Enemy::Start() {
	previousPosition = transform->position;
	animator = gameObject->GetAppBase<Animator>();
	spriteRenderer = gameObject->GetAppBase<SpriteRenderer>();
}

void Enemy::Update() {

    if (GameManager::GetInstance().IsPause()) return;

    if (script && !script->IsFinished()) 
        script->Update(*this);
    else if (IsOutsideScreen(transform->position))
        Destroy();

    // === 向きの更新処理 ===
    Vector2D currentPosition = transform->position;
    float dx = currentPosition.x - previousPosition.x;
    if (animator) {
        if (spriteRenderer) {
            bool moving = false;
            if (dx > 0.5f) {
                spriteRenderer->FlipX(false); // 右向き
                moving = true;
            }
            else if (dx < -0.5f) {
                spriteRenderer->FlipX(true);  // 左向き
                moving = true;
            }
            // パラメータは一度だけセット
            animator->SetBool("isMoving", moving);
        }
    }

    if (damageFlashTimer > 0) {
        --damageFlashTimer;

        if (spriteRenderer) {
            spriteRenderer->SetColor(Color(100, 100,  175, 255)); // 青黒目に
        }

        if (damageFlashTimer == 0 && spriteRenderer) {
            spriteRenderer->SetColor(Color(255, 255, 255, 255)); // 元に戻す
        }
    }

    previousPosition = currentPosition;

    velocity += acceleration;

    transform->position += velocity;

    // 弾発射スクリプトもあればUpdate
    if (shotScript && !shotScript->IsFinished()) {
        shotScript->Update(transform->position);
    }
}

void Enemy::Damage(int _damage) {
    status.hp -= _damage;
	damageFlashTimer = maxDamageFlashFrame; // ダメージを受けたら点滅開始
    if (status.hp <= 0 && gameObject->IsActive()) {
        gameObject->SetActive(false);
        DropItem();
		PrefabMgr.Instantiate("EnemyDestroyEffect")[0]->transform->position = transform->position;
        if (auto sound = Sounds["enemy_destroy"]) {
			MusicController::GetInstance()->OneShotAudio(sound->Clone(), 0.75f);
        }
        Destroy();
    }
}

void Enemy::DropItem() {
    for (auto data : dropItems) {
        for (int i = 0; i < data.quantity; i++) {
            auto pos = transform->position;
            // 1以外だったらランダム.
            if (data.quantity != 1) {
                pos.x += Random.Range(-32.0f, 32.0f);
                pos.y += Random.Range(-32.0f, 32.0f);
            }
            ItemManager::GetInstance().CreateItem(data.type, pos);
        }
    }
}

void Enemy::OnDestroy() {
    EnemyManager::GetInstance().RemoveEnemyObj(shared_from_this());
}

Vector2D Enemy::GetPosition() const { return transform->position; }
void Enemy::SetPosition(const Vector2D& p) { transform->position = p; }
Vector2D Enemy::GetVelocity() const { return velocity; }
void Enemy::SetVelocity(const Vector2D& v) { velocity = v; }
float Enemy::GetRotation() const { return rotation; }
void Enemy::SetRotation(float r) { rotation = r; }
bool Enemy::IsOutsideScreen(const Vector2D& pos) {
    auto size = Window::GetInstance().GetMaxVector2D() / 2;
    constexpr float margin = 0; // 画面外に出たら即消えるのを防ぐ余白

    return  pos.x < -size.x - margin || pos.x > size.x + margin - 300 ||
            pos.y < -size.y - margin || pos.y > size.y + margin;
}
std::shared_ptr<AppBase> Enemy::Clone() const {
    return std::make_shared<Enemy>(*this);
}

void Enemy::OnCollisionEnter(GameObject* other) {
    if (other->GetTag() == "PlayerBullet") {
        if (auto app = other->GetAppBase<PlayerBullet>()) {
            Damage(app->GetDamage());
        }
    }
}

float Enemy::GetAngle() const {
    return rotation;
}
void Enemy::SetAngle(float angle) {
    rotation = angle;
}

void Enemy::SetHp(int hp) {
    status.hp = hp;
}

void Enemy::SetHitboxEnable(bool enabled) {
    // 当たり判定のON/OFF処理
    // 例: hitboxEnabled = enabled;
}

void Enemy::AddDropItem(ItemType type, int quantity) {
    dropItems.push_back({ type, quantity });
}

const std::vector<DropItemInfo>& Enemy::GetDropItems() const {
    return dropItems;
}

void Enemy::ClearDropItems() {
    dropItems.clear();
}

void Enemy::Die() {
    Destroy();
}

void Enemy::Accelerate(const Vector2D& a) {
    acceleration = a;
}
void Enemy::Rotate(float deltaAngle) {
    rotation += deltaAngle;
}
void Enemy::FireShot(const std::string& shotScriptID) {
    // 仮実装: ShotManagerなどと連携して実装する予定
    printf("[Enemy] FireShot: %s\n", shotScriptID.c_str());
    shotScript = ShotScriptManager::GetInstance().CloneShotScript(shotScriptID);
}
void Enemy::SetType(int templateId, int colorId) {
    // 仮実装: 弾テンプレートと色を設定
    printf("[Enemy] SetType: templateId=%d, colorId=%d\n", templateId, colorId);
}
void Enemy::EmitEffect(int effectId) {
    // エフェクトマネージャーなどと連携予定
    printf("[Enemy] EmitEffect: effectId=%d\n", effectId);
}
void Enemy::SetOffset(int slot, const Vector2D& offset) {
    // 発射スロットのオフセット保存など
    printf("[Enemy] SetOffset: slot=%d, offset=(%.2f, %.2f)\n", slot, offset.x, offset.y);
}
