/*
    ◆ PlayerBullet.cpp

    クラス名        : PlayerBullet クラス
    作成日          : 2025/07/09 (水) 14:59:43
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/
#include "PlayerBullet.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "GameManager.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"

// コンストラクタ 
PlayerBullet::PlayerBullet(){

}
PlayerBullet::PlayerBullet(std::shared_ptr<GameObject> owner) {

}

// AppBase Event.

void PlayerBullet::Initialize() {
	animator = gameObject->GetAppBase<Animator>();

    // デフォルトの当たり演出をセット
    if (!onHitLambda) {
        onHitLambda = [](PlayerBullet* b) {
            if (b->animator) {
                b->animator->SetBool("isHit", true);
                if (auto s = b->animator->GetOwnerRenderer()) {
                    s->SetLayer(6);
                    s->GetColor().a = 1.0f;
                }
                b->speed *= 0.2f;
                b->Destroy(0.5f);
            }
            else {
                b->Destroy();
            }
        };
    }
}

void PlayerBullet::OnCollisionEnter(GameObject* other) {
    if (!other || !other->IsActive()) return;

    std::string tag = other->GetTag();
    if (tag == "Enemy") {
        ClearController();

        // スコアやサウンド
        GameManager::GetInstance().GetScoreManager().OnEnemyHit();
        if (auto music = MusicController::GetInstance()) {
            music->OneShotAudio(Sounds["enemy_damage"]->Clone());
        }
        
        if (collider) collider->SetEnabled(false);

        // カスタムラムダを呼ぶ
        if (onHitLambda) onHitLambda(this);
        else Destroy();
    }
}
