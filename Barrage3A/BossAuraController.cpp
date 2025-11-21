/*
    ◆ BossAuraController.cpp

    クラス名        : BossAuraController クラス
    作成日          : 2025/07/27 (日) 22:45:50
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "BossAuraController.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "GameWorldManager.hpp"
// コンストラクタ 
BossAuraController::BossAuraController() : AppBase("BossAuraController"){

}
BossAuraController::BossAuraController(std::shared_ptr<GameObject> owner) : AppBase("BossAuraController", owner) {

}

// AppBase Event.
void BossAuraController::Start() {

}

void BossAuraController::Update() {

	transform->position = GameWorldManager::GetInstance().ScreenToWorld(Input.GetMousePoint_F());

	if (!auraRenderer.lock()) return;

    time += Time.deltaTime;
    float loopSpeed = 2.0f * DX_PI_F / cycleXY;
    float angle = time * loopSpeed;
    Vector3D rot;
    rot.x = (Mathf::Sin(angle) * 0.5f + 0.5f) * maxX;
    rot.y = -Mathf::Cos(angle) * maxY;
    rot.z = fmodf(auraRenderer.lock()->GetRotation().z + speedZ * Time.deltaTime, 360.0f);
	auraRenderer.lock()->SetRotation(rot);
}