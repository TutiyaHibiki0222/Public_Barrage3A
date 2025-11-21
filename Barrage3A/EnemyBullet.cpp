/*
    ◆ EnemyBullet.cpp

    クラス名        : EnemyBullet クラス
    作成日          : 2025/11/16 (日) 16:45:27
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "EnemyBullet.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "GameManager.h"
#include "Prefab.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"
#include "BulletManager.h"
// コンストラクタ 
EnemyBullet::EnemyBullet(){

}
EnemyBullet::EnemyBullet(std::shared_ptr<GameObject> owner){

}

// AppBase Event.
void EnemyBullet::Awake() {
	BulletManager::Instance().AddBullet(shared_from_this());
}

void EnemyBullet::OnDestroy() {
    BulletManager::Instance().RemoveBullet(shared_from_this());
}

void EnemyBullet::OnCollisionEnter(GameObject* other) {
    std::string tag = other->GetTag();
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
    gameObject->SetActive(false);
    Destroy();
}