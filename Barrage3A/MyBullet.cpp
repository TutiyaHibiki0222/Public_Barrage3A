/*
    ◆ MyBulletBase.cpp

    クラス名        : MyBulletBase クラス
    作成日          : 2025/07/11 (金) 12:29:32
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/

/*

#include "MyBullet.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "EnemyManager.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"
#include "GameManager.h"
#include "Prefab.h"
#include "BulletManager.h"

// コンストラクタ 
MyBullet::MyBullet() : AppBase("MyBullet") {

}

MyBullet::MyBullet(std::shared_ptr<GameObject> owner) : AppBase("MyBullet", owner) {

}

// AppBase Event.
void MyBullet::Awake() {
    if(gameObject->GetLayer() != Layer::PlayerBullet) {
        BulletManager::Instance().AddBullet(shared_from_this());
	}
}

void MyBullet::Update() {

    if (GameManager::GetInstance().IsPause()) return;

    // 角度を加算
    transform->rotation += rotationRate;

    // 速度を加速
    speed += acceleration;

    // カスタム挙動（優先される）
    if (controller) controller->Update(transform, this);

    // 向きに基づいて移動
    Vector2D direction = Vector2D::FromAngle(transform->rotation);
    transform->position += direction * speed;

    // 画面外チェック
    Vector2D pos = transform->position;
    Vector2D wid = Window::GetInstance().GetMaxVector2D() / 2;

    if (pos.x < -wid.x || pos.x > wid.x - 300 ||
        pos.y < -wid.y || pos.y > wid.y) {
        Destroy();
    }
}

void MyBullet::OnDestroy() {

    if (gameObject->GetLayer() == Layer::EnemyBullet) {
       auto obj = PrefabManager::GetInstance().Instantiate("EnemyBulletEffect")[0];
       obj->transform->position = transform->position;
    }

    if (gameObject->GetLayer() != Layer::PlayerBullet) {
        BulletManager::Instance().RemoveBullet(shared_from_this());
    }
}

void MyBullet::OnCollisionEnter(GameObject* other) {
    std::string tag = other->GetTag();
    // Layerコライダーで Graze vs EnemyBulletのみになっている.
    if (tag == "Graze") {
        if (!isGraze) {
            isGraze = true;
            auto music = MusicController::GetInstance();
            if (music)
            {
                music->OneShotAudio(Sounds["graze"]->Clone());
            }
            // 弾とGrazeの中間点にエフェクトを配置する（演出的に自然）
            auto obj = PrefabManager::GetInstance().Instantiate("GrazeEffect")[0];
            obj->transform->position = transform->position;
            GameManager::GetInstance().GetGrazeManager().Add(1);
            
        }
        return;
    }
     // 敵との当たり判定
    if (tag == "Enemy" && other->IsActive()) {
        auto& scoreMgr  = GameManager::GetInstance().GetScoreManager();
        scoreMgr.OnEnemyHit();
        auto music      = MusicController::GetInstance();
        if (music)
        {
            music->OneShotAudio(Sounds["enemy_damage"]->Clone());
        }
    }
    // Graze以外の対象には通常通り消える
    gameObject->SetActive(false);
    Destroy();
}

void MyBullet::SetBulletType(BulletParentID id, BulletColor color) {
    auto collider = gameObject->GetAppBase<CircleCollider>();
    auto renderer = gameObject->GetAppBase<SpriteRenderer>();
    
    if (!collider || !renderer) return;

    auto& typeManager = BulletTypeManager::GetInstance();

    // データ取得
    if (!typeManager.HasBulletType(id)) {
        std::cerr << "Unknown BulletParentID: " << static_cast<int>(id) << std::endl;
        return;
    }

    const auto& data = typeManager.GetBulletType(id);

    // イメージインデックス取得
    int imageIndex = typeManager.GetImageIndex(id, color);
    if (imageIndex == -1) {
        std::cerr << "BulletColor not found: " << static_cast<int>(color) << std::endl;
        return;
    }

    // テクスチャ取得と設定
    auto texture = Tex[data.imagePath + "_" + std::to_string(imageIndex)];
    if (texture) {
        renderer->SetSprite(texture);
    }
    else {
        std::cerr << "Texture not loaded: " << data.imagePath << "_" << imageIndex << std::endl;
    }

    // ヒットボックス半径を変更
    float radius = data.hitboxSize;
    collider->SetRadius(radius);
}
*/