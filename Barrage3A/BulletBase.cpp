/*
    ◆ BulletBase.cpp

    クラス名        : BulletBase クラス
    作成日          : 2025/11/16 (日) 15:41:10
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "BulletBase.h"
#include "GameObject.h"
#include "GameObjectMgr.h"

// AppBase Event.

void BulletBase::Start() {
	collider = gameObject->GetAppBase<Collider2D>();
    Initialize();
}

void BulletBase::Tick() {
    // 角度を加算
    angle += rotVel;

    // 速度を加速
    speed += accel;

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

void BulletBase::SetBulletType(BulletParentID id, BulletColor color) {
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