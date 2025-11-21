#pragma once
#include "BulletScriptManager.h"
#include "ShotScriptManager.h"
#include "EnemyScriptManager.h"
#include "BulletInstruction.h"
#include "ShotInstruction.h"
#include "EnemyInstruction.h"
#include "EnemyScript.h"
#include "MyBullet.h"
#include "ShotScript.h"
#include "Define.h"
void RegisterBulletScripts() {
    {
        auto bulletScrip = std::make_shared<BulletScript>();
        bulletScrip->AddInstruction(BulletInstruction::LerpVelocity(0, 120));
        bulletScrip->AddInstruction(BulletInstruction::Wait(30));
        bulletScrip->AddInstruction(BulletInstruction::RandomizeAngle(180));
        bulletScrip->AddInstruction(BulletInstruction::SetPlayerSE("enemy_bullet_change", 0.35f));
        bulletScrip->AddInstruction(BulletInstruction::LerpVelocity(5, 180));
        bulletScrip->AddInstruction(BulletInstruction::AddRotateTo(10, 60));
        BulletScriptManager::GetInstance().RegisterBulletScript("bullet01", bulletScrip);
    }
    {
        auto bulletScript = std::make_shared<BulletScript>();
        bulletScript->AddInstruction(BulletInstruction::LerpVelocity(3.f, 60));
        bulletScript->AddInstruction(BulletInstruction::LerpVelocity(0.f, 60));
        bulletScript->AddInstruction(BulletInstruction::AimAtPlayer());
        bulletScript->AddInstruction(BulletInstruction::Wait(30));
        bulletScript->AddInstruction(BulletInstruction::LerpVelocity(6.f, 120));
        bulletScript->AddInstruction(BulletInstruction::End());
        BulletScriptManager::GetInstance().RegisterBulletScript("bullet02", bulletScript);
    }
    {
        auto bulletScript = std::make_shared<BulletScript>();
        bulletScript->AddInstruction(BulletInstruction::Wait(30));
        bulletScript->AddInstruction(BulletInstruction::LerpVelocity(1.f, 120));       // 少し減速しつつ
        bulletScript->AddInstruction(BulletInstruction::AddRotateTo(45, 120));         // 120フレームで15度回転（ゆっくり波打つイメージ）
        bulletScript->AddInstruction(BulletInstruction::Wait(10));                     // 持続時間
        bulletScript->AddInstruction(BulletInstruction::AddRotateTo(-45, 120));        // 逆方向へ回転（往復運動）
        bulletScript->AddInstruction(BulletInstruction::Wait(10));
        bulletScript->AddInstruction(BulletInstruction::LoopStart(3));                 // 波を3回繰り返す
        bulletScript->AddInstruction(BulletInstruction::AddRotateTo(45, 120));
        bulletScript->AddInstruction(BulletInstruction::Wait(10));
        bulletScript->AddInstruction(BulletInstruction::AddRotateTo(-45, 120));
        bulletScript->AddInstruction(BulletInstruction::Wait(10));
        bulletScript->AddInstruction(BulletInstruction::LoopEnd());
        bulletScript->AddInstruction(BulletInstruction::LerpVelocity(6.f, 60));       // 加速しつつ終了へ
        bulletScript->AddInstruction(BulletInstruction::End());
        BulletScriptManager::GetInstance().RegisterBulletScript("bullet_wave_rotate", bulletScript);
    }

    {
        auto bulletScript = std::make_shared<BulletScript>();
        bulletScript->AddInstruction(BulletInstruction::Wait(60));
        bulletScript->AddInstruction(BulletInstruction::LerpVelocity(1.f, 60));
        bulletScript->AddInstruction(BulletInstruction::End());
        BulletScriptManager::GetInstance().RegisterBulletScript("slowBullet", bulletScript);
    }
}

void RegisterShotScripts() {
    {
        auto shotScript = std::make_shared<ShotScript>();
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::Crystal, BulletColor::VividRed)); // デフォルト弾設定       
        shotScript->AddInstruction(ShotInstruction::SetBulletScript("bullet01"));
        shotScript->AddInstruction(ShotInstruction::LoopStart(10));
        shotScript->AddInstruction(ShotInstruction::SetPlayerAngle());
        shotScript->AddInstruction(ShotInstruction::FireMultiSpeed(0, { 2,3,4,5,6 }));
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.6f));
        shotScript->AddInstruction(ShotInstruction::Wait(5));
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::End()); // スクリプト終了
        ShotScriptManager::GetInstance().RegisterShotScript("shot_aim", shotScript);
    }
    {
        auto shotScript = std::make_shared<ShotScript>();
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::Knife, BulletColor::Gray));
        shotScript->AddInstruction(ShotInstruction::SetPlayerAngle());
        shotScript->AddInstruction(ShotInstruction::SetBulletScript("bullet02"));
        shotScript->AddInstruction(ShotInstruction::LoopStart(1));
        shotScript->AddInstruction(ShotInstruction::FireSpreadCentered(3, 0.f, 30.f, 5.f));  // 3発、中心角0度、30度範囲、速度5
		shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.6f));
		shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::End());
        ShotScriptManager::GetInstance().RegisterShotScript("simple_3way_shot", shotScript);
    }
    {
        auto shotScript = std::make_shared<ShotScript>();
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::BigB4, BulletColor::Blue));
        shotScript->AddInstruction(ShotInstruction::SetBulletScript("bullet03")); // 各弾の動き（回転 or 波）を別スクリプトで
        shotScript->AddInstruction(ShotInstruction::LoopStart(20));     // 20回発射
        shotScript->AddInstruction(ShotInstruction::FireSpreadCentered(5, -0.f, 60.f, 3.f)); // 上方向に60度扇状に5発
		shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.5f));
		shotScript->AddInstruction(ShotInstruction::AddBaseAngle(-10));  // 毎回10度回転
        shotScript->AddInstruction(ShotInstruction::Wait(4));
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::End());
        ShotScriptManager::GetInstance().RegisterShotScript("rotating_5way_shot", shotScript);
    }
    {
        auto shotScript = std::make_shared<ShotScript>();
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::B5, BulletColor::Red)); // 好みで調整
        shotScript->AddInstruction(ShotInstruction::SetBulletScript("bullet_nway")); // NWay用のBullet動作（加速/回転など）
        shotScript->AddInstruction(ShotInstruction::SetPlayerAngle()); // プレイヤー方向を中心軸に
        shotScript->AddInstruction(ShotInstruction::FireSpreadCentered(5, 0.f, 60.f, 3.f)); // 5発、中心0度、60度扇、速度3
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.5f));
        shotScript->AddInstruction(ShotInstruction::End());
        ShotScriptManager::GetInstance().RegisterShotScript("nway_5_shot", shotScript);
    }
    {
        auto shotScript = std::make_shared<ShotScript>();
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::Crystal, BulletColor::VividRed));
        shotScript->AddInstruction(ShotInstruction::SetBulletScript("bullet_aimed")); // 弾挙動（直進 or 波打ちなど）
        shotScript->AddInstruction(ShotInstruction::SetPlayerAngle());
        shotScript->AddInstruction(ShotInstruction::Fire(0,4.5f)); // 単発 弾速4.5
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.4f));
        shotScript->AddInstruction(ShotInstruction::End());
        ShotScriptManager::GetInstance().RegisterShotScript("aimed_shot", shotScript);
    }
    {
        auto shotScript = std::make_shared<ShotScript>();
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::B4, BulletColor::Purple));
        shotScript->AddInstruction(ShotInstruction::SetBulletScript("bullet_wave_rotate")); // 波＋回転スクリプト
        shotScript->AddInstruction(ShotInstruction::LoopStart(30));     // 30回ループ
        shotScript->AddInstruction(ShotInstruction::AddBaseAngle(8));   // 毎回8度回転
        shotScript->AddInstruction(ShotInstruction::FireSpreadCentered(7, 90.f, 70.f, 2.5f)); // 上方向に7way発射
		shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.6f));
		shotScript->AddInstruction(ShotInstruction::Wait(4));
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::End());
        ShotScriptManager::GetInstance().RegisterShotScript("wave_rotating_7way", shotScript);
    }
    {
        auto shotScript = std::make_shared<ShotScript>();
        // 弾種はBigB4青でかため、回転しながら多方向に撃つイメージ
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::BigB4, BulletColor::Blue));
        //shotScript->AddInstruction(ShotInstruction::SetBulletScript("bullet_wave_rotate"));  // 波打ち＋回転動作  
        shotScript->AddInstruction(ShotInstruction::LoopStart(10));          // 30回ループ
        shotScript->AddInstruction(ShotInstruction::FireSpreadCentered(36, 0.f, 360.f, 4.0f));  // 9way 90度扇 速度3.0f
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.5f));
        shotScript->AddInstruction(ShotInstruction::AddBaseAngle(1.5f));        // 毎回5度回転
        shotScript->AddInstruction(ShotInstruction::Wait(5));
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::LoopStart(10));          // 30回ループ
        shotScript->AddInstruction(ShotInstruction::FireSpreadCentered(36, 0.f, 360.f, 4.0f));  // 9way 90度扇 速度3.0f
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.5f));
        shotScript->AddInstruction(ShotInstruction::AddBaseAngle(-1.5f));        // 毎回5度回転
        shotScript->AddInstruction(ShotInstruction::Wait(5));
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::End());
        ShotScriptManager::GetInstance().RegisterShotScript("elite_enemy_rotating_wave_36way", shotScript);
    }
    {
        auto shotScript = std::make_shared<ShotScript>();
        // 弾種はBigB4青でかため、回転しながら多方向に撃つイメージ
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::BigB4, BulletColor::Blue));
        //shotScript->AddInstruction(ShotInstruction::SetBulletScript("bullet_wave_rotate"));  // 波打ち＋回転動作  
        shotScript->AddInstruction(ShotInstruction::LoopStart(10));          // 30回ループ
        shotScript->AddInstruction(ShotInstruction::FireSpreadCentered(16, 0.f, 360.f, 4.0f));  // 9way 90度扇 速度3.0f
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.5f));
        shotScript->AddInstruction(ShotInstruction::AddBaseAngle(1.5f));        // 毎回5度回転
        shotScript->AddInstruction(ShotInstruction::Wait(5));
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::LoopStart(10));          // 30回ループ
        shotScript->AddInstruction(ShotInstruction::FireSpreadCentered(16, 0.f, 360.f, 4.0f));  // 9way 90度扇 速度3.0f
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.5f));
        shotScript->AddInstruction(ShotInstruction::AddBaseAngle(-1.5f));        // 毎回5度回転
        shotScript->AddInstruction(ShotInstruction::Wait(5));
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::End());
        ShotScriptManager::GetInstance().RegisterShotScript("elite_enemy_rotating_wave_16way", shotScript);
    }
    {
        auto shotScript = std::make_shared<ShotScript>();
        // 弾種はBigB4青でかため、回転しながら多方向に撃つイメージ
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::BigB4, BulletColor::Blue));
        shotScript->AddInstruction(ShotInstruction::SetBulletScript("slowBullet")); // 徐々に減速する弾
        shotScript->AddInstruction(ShotInstruction::SetPlayerAngle()); // プレイヤー方向を中心軸に
        shotScript->AddInstruction(ShotInstruction::LoopStart(6));
        shotScript->AddInstruction(ShotInstruction::FireSpreadCentered(5, 0.f, 10.f, 2.5f)); // 5発 x 6回 = 30発
        shotScript->AddInstruction(ShotInstruction::AddBaseAngle(60)); // 回転させる
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.5f));
        shotScript->AddInstruction(ShotInstruction::Wait(30));
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::Star, BulletColor::LemonYellow));
        shotScript->AddInstruction(ShotInstruction::SetBulletScript("bullet01"));
        shotScript->AddInstruction(ShotInstruction::SetPlayerAngle()); // プレイヤー方向に向ける
        shotScript->AddInstruction(ShotInstruction::LoopStart(4));
        shotScript->AddInstruction(ShotInstruction::LoopStart(12));
        shotScript->AddInstruction(ShotInstruction::FireMultiSpeed(0, { 1.f, 2.0f, 3.0f, 4.0f }));
        shotScript->AddInstruction(ShotInstruction::AddBaseAngle(30)); // 回転していく
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.5f));
		shotScript->AddInstruction(ShotInstruction::Wait(15));
        shotScript->AddInstruction(ShotInstruction::AddBaseAngle(15)); // 回転していく
        shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::End());
        ShotScriptManager::GetInstance().RegisterShotScript("elite_enemy", shotScript);
    }
    {
        auto shotScript = std::make_shared<ShotScript>();
        shotScript->AddInstruction(ShotInstruction::SetBulletDefault(BulletParentID::Crystal, BulletColor::Cyan));
        shotScript->AddInstruction(ShotInstruction::LoopStart(30));
        shotScript->AddInstruction(ShotInstruction::SetPlayerAngle());
        shotScript->AddInstruction(ShotInstruction::Fire(0, 5.0f));
        shotScript->AddInstruction(ShotInstruction::SetPlayerSE("enemy_shot", 0.4f));
		shotScript->AddInstruction(ShotInstruction::Wait(15));
		shotScript->AddInstruction(ShotInstruction::LoopEnd());
        shotScript->AddInstruction(ShotInstruction::End());
        ShotScriptManager::GetInstance().RegisterShotScript("30LoopShot", shotScript);
    }
}

void RegisterEnemyScripts() {
    {
        // スクリプト構築
        std::shared_ptr<EnemyScript> enemy01 = std::make_shared<EnemyScript>();
        enemy01->Add(EnemyInstruction::SetHp(40));
        enemy01->Add(EnemyInstruction::AddDropItem(ItemType::Power, 1));
        enemy01->Add(EnemyInstruction::MoveTo(Vector2D(-150, 250), 120));
        enemy01->Add(EnemyInstruction::Wait(30)); 
        enemy01->Add(EnemyInstruction::Fire("shot_aim"));
        enemy01->Add(EnemyInstruction::Wait(30));
        enemy01->Add(EnemyInstruction::MoveTo(Vector2D(100, 190), 120));
        enemy01->Add(EnemyInstruction::LoopStart(5));
        enemy01->Add(EnemyInstruction::Fire("simple_3way_shot"));
        enemy01->Add(EnemyInstruction::Wait(30));
        enemy01->Add(EnemyInstruction::LoopEnd());
        enemy01->Add(EnemyInstruction::Wait(30));
        enemy01->Add(EnemyInstruction::Accelerate({ -0.1f,0 }));
        enemy01->Add(EnemyInstruction::End());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("Enemy01", enemy01);
    }
    {

        std::shared_ptr<EnemyScript> enemy02 = std::make_shared<EnemyScript>();
        enemy02->Add(EnemyInstruction::SetHp(60));
        enemy02->Add(EnemyInstruction::AddDropItem(ItemType::Power, 2));
        // 画面上から出現 → 中央へ
        enemy02->Add(EnemyInstruction::MoveTo(Vector2D(0, 250), 60));
        enemy02->Add(EnemyInstruction::Wait(20));
        // 左右に移動しながら射撃を繰り返す
        enemy02->Add(EnemyInstruction::LoopStart(3));
        enemy02->Add(EnemyInstruction::MoveTo(Vector2D(-120, 200), 60));
        enemy02->Add(EnemyInstruction::Fire("nway_5_shot"));  // 5WAY弾発射
        enemy02->Add(EnemyInstruction::Wait(20));
        enemy02->Add(EnemyInstruction::MoveTo(Vector2D(120, 200), 60));
        enemy02->Add(EnemyInstruction::Fire("nway_5_shot"));
        enemy02->Add(EnemyInstruction::Wait(20));
        enemy02->Add(EnemyInstruction::LoopEnd());
        // 少しホーミング移動して離脱
        enemy02->Add(EnemyInstruction::Accelerate(Vector2D(0.0f, -0.05f)));
        enemy02->Add(EnemyInstruction::Wait(60));
        enemy02->Add(EnemyInstruction::Accelerate(Vector2D(0.0f, 0.2f)));  // 画面下方向へ加速
        enemy02->Add(EnemyInstruction::End());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("Enemy02", enemy02);
    }
    {
        std::shared_ptr<EnemyScript> enemy03 = std::make_shared<EnemyScript>();
        enemy03->Add(EnemyInstruction::SetHp(50));
        enemy03->Add(EnemyInstruction::AddDropItem(ItemType::Power, 1));
        enemy03->Add(EnemyInstruction::AddDropItem(ItemType::Score, 2));

        // 左上から、中心（x = -150）に向かって斜めに入場
        enemy03->Add(EnemyInstruction::MoveTo(Vector2D(-150, 260), 60));
        enemy03->Add(EnemyInstruction::Wait(20));

        // その場で5連射（60F間隔で）
        enemy03->Add(EnemyInstruction::LoopStart(5));
        enemy03->Add(EnemyInstruction::Fire("aimed_shot"));  // プレイヤー狙い単発
        enemy03->Add(EnemyInstruction::Wait(60));
        enemy03->Add(EnemyInstruction::LoopEnd());

        // 少し後退してフェードアウト or 下へ流す
        enemy03->Add(EnemyInstruction::MoveTo(Vector2D(-180, 300), 45));
        enemy03->Add(EnemyInstruction::Accelerate(Vector2D(0, 0.2f)));  // 下に加速

        enemy03->Add(EnemyInstruction::End());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("Enemy03", enemy03);
    }
    {
        std::shared_ptr<EnemyScript> enemy04 = std::make_shared<EnemyScript>();
        enemy04->Add(EnemyInstruction::SetHp(70));
        enemy04->Add(EnemyInstruction::AddDropItem(ItemType::Power, 2));
        enemy04->Add(EnemyInstruction::AddDropItem(ItemType::Score, 3));

        // 左斜め上から x = -150 に入場
        enemy04->Add(EnemyInstruction::MoveTo(Vector2D(-150, 260), 60));
        enemy04->Add(EnemyInstruction::Wait(15));

        // その場で回転5WAYばら撒き（rotating_5way_shotを使用）
        enemy04->Add(EnemyInstruction::LoopStart(1));
        enemy04->Add(EnemyInstruction::Fire("rotating_5way_shot")); // 20発 × 5WAY × 回転
        enemy04->Add(EnemyInstruction::Wait(90));  // 撃ち切るまで待つ
        enemy04->Add(EnemyInstruction::LoopEnd());

        // 少し左に抜けてから下へ流れる
        enemy04->Add(EnemyInstruction::MoveTo(Vector2D(-180, 300), 45));
        enemy04->Add(EnemyInstruction::Accelerate(Vector2D(0, 0.15f)));

        enemy04->Add(EnemyInstruction::End());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("Enemy04", enemy04);
    }
    {
        std::shared_ptr<EnemyScript> enemy05 = std::make_shared<EnemyScript>();
        enemy05->Add(EnemyInstruction::SetHp(80));
        enemy05->Add(EnemyInstruction::AddDropItem(ItemType::Power, 1));
        enemy05->Add(EnemyInstruction::AddDropItem(ItemType::Score, 3));

        // 左上から中央 x = -150 に移動
        enemy05->Add(EnemyInstruction::MoveTo(Vector2D(-150, 240), 60));
        enemy05->Add(EnemyInstruction::Wait(20));

        // 波＋回転弾をばら撒く
        enemy05->Add(EnemyInstruction::Fire("wave_rotating_7way"));
        enemy05->Add(EnemyInstruction::Wait(90)); // 撃ち切り待ち

        // 照準弾で締め
        enemy05->Add(EnemyInstruction::LoopStart(3));
        enemy05->Add(EnemyInstruction::Fire("aimed_shot"));
        enemy05->Add(EnemyInstruction::Wait(45));
        enemy05->Add(EnemyInstruction::LoopEnd());

        // 少し左下に移動してから退散
        enemy05->Add(EnemyInstruction::MoveTo(Vector2D(-180, 290), 45));
        enemy05->Add(EnemyInstruction::Accelerate(Vector2D(0, 0.2f))); // 下へ加速退場

        enemy05->Add(EnemyInstruction::End());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("Enemy05", enemy05);
    }

    {
        std::shared_ptr<EnemyScript> enemy06 = std::make_shared<EnemyScript>();
        enemy06->Add(EnemyInstruction::SetHp(20));
        enemy06->Add(EnemyInstruction::AddDropItem(ItemType::Power, 1));
        enemy06->Add(EnemyInstruction::FollowPath(4, 600));
        enemy06->Add(EnemyInstruction::Die());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("Enemy06", enemy06);
    }

    {
        std::shared_ptr<EnemyScript> enemyL = std::make_shared<EnemyScript>();
        enemyL->Add(EnemyInstruction::SetHp(20));
        enemyL->Add(EnemyInstruction::AddDropItem(ItemType::Power, 1));
        enemyL->Add(EnemyInstruction::FollowPath(5, 600));
        enemyL->Add(EnemyInstruction::Die());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("EnemyL", enemyL);
    }

    {
        std::shared_ptr<EnemyScript> enemyR = std::make_shared<EnemyScript>();
        enemyR->Add(EnemyInstruction::SetHp(20));
        enemyR->Add(EnemyInstruction::AddDropItem(ItemType::Power, 1));
        enemyR->Add(EnemyInstruction::FollowPath(6, 600));
        enemyR->Add(EnemyInstruction::Die());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("EnemyR", enemyR);
    }
    {
        std::shared_ptr<EnemyScript> enemyL = std::make_shared<EnemyScript>();
        enemyL->Add(EnemyInstruction::SetHp(40));
        enemyL->Add(EnemyInstruction::AddDropItem(ItemType::Power, 2));
        enemyL->Add(EnemyInstruction::Fire("30LoopShot"));
        enemyL->Add(EnemyInstruction::FollowPath(5, 400));
        enemyL->Add(EnemyInstruction::Die());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("EnemyL1", enemyL);
    }

    {
        std::shared_ptr<EnemyScript> enemyR = std::make_shared<EnemyScript>();
        enemyR->Add(EnemyInstruction::SetHp(40));
        enemyR->Add(EnemyInstruction::AddDropItem(ItemType::Power, 2));
        enemyR->Add(EnemyInstruction::Fire("30LoopShot"));
        enemyR->Add(EnemyInstruction::FollowPath(6, 400));
        enemyR->Add(EnemyInstruction::Die());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("EnemyR1", enemyR);
    }
    {
        std::shared_ptr<EnemyScript> enemy07 = std::make_shared<EnemyScript>();
        enemy07->Add(EnemyInstruction::SetHp(45));
        enemy07->Add(EnemyInstruction::AddDropItem(ItemType::Power, 1));

        // ====== 登場 ======
        enemy07->Add(EnemyInstruction::Teleport(Vector2D(-200, 500))); // 左外下寄りから出現
        enemy07->Add(EnemyInstruction::MoveTo(Vector2D(-150, 180), 90)); // 斜めに入場
        enemy07->Add(EnemyInstruction::Wait(20));

        // ====== 狙い撃ち ======
        enemy07->Add(EnemyInstruction::Wait(15));
        enemy07->Add(EnemyInstruction::Fire("shot_aim"));
        enemy07->Add(EnemyInstruction::Wait(25));

        // ====== 左右往復しながら弾を撒く ======
        enemy07->Add(EnemyInstruction::LoopStart(4));
        // 左へ
        enemy07->Add(EnemyInstruction::MoveTo(Vector2D(-280, 200), 80));
        enemy07->Add(EnemyInstruction::Fire("simple_3way_shot"));
        enemy07->Add(EnemyInstruction::Wait(20));

        // 右へ
        enemy07->Add(EnemyInstruction::MoveTo(Vector2D(-20, 200), 80));
        enemy07->Add(EnemyInstruction::Fire("simple_5way_shot")); // 右ではちょっと強化
        enemy07->Add(EnemyInstruction::Wait(20));
        enemy07->Add(EnemyInstruction::LoopEnd());

        // ====== 退場 ======
        enemy07->Add(EnemyInstruction::Wait(20));
        enemy07->Add(EnemyInstruction::Accelerate({ 0.0f, 0.15f })); // 下方向に加速して退場
        enemy07->Add(EnemyInstruction::End());

        EnemyScriptManager::GetInstance().RegisterEnemyScript("Enemy07", enemy07);
    }


}

void RegisterEliteEnemyScripts() {
    // エリート敵のスクリプト登録
    {
        std::shared_ptr<EnemyScript> eliteEnemy = std::make_shared<EnemyScript>();
        eliteEnemy->Add(EnemyInstruction::SetHp(500));
        eliteEnemy->Add(EnemyInstruction::Teleport(Vector2D(-150, 400)));
        eliteEnemy->Add(EnemyInstruction::AddDropItem(ItemType::PowerBig, 3));
        eliteEnemy->Add(EnemyInstruction::AddDropItem(ItemType::Score   ,10));
        eliteEnemy->Add(EnemyInstruction::AddDropItem(ItemType::ScoreBig, 3));

        eliteEnemy->Add(EnemyInstruction::MoveTo(Vector2D(-150, 200), 120));
        eliteEnemy->Add(EnemyInstruction::LoopStart(3));
        eliteEnemy->Add(EnemyInstruction::Wait(30));
        eliteEnemy->Add(EnemyInstruction::Fire("elite_enemy"));
        eliteEnemy->Add(EnemyInstruction::Wait(200));
		eliteEnemy->Add(EnemyInstruction::LoopEnd());
        eliteEnemy->Add(EnemyInstruction::Accelerate(Vector2D(0, -0.01f)));
        eliteEnemy->Add(EnemyInstruction::End());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("EliteEnemy01", eliteEnemy);
	}

    {
        std::shared_ptr<EnemyScript> eliteEnemy = std::make_shared<EnemyScript>();
        eliteEnemy->Add(EnemyInstruction::SetHp(600));
        eliteEnemy->Add(EnemyInstruction::AddDropItem(ItemType::PowerBig, 3));
        eliteEnemy->Add(EnemyInstruction::AddDropItem(ItemType::Score,   12));
        eliteEnemy->Add(EnemyInstruction::AddDropItem(ItemType::ScoreBig, 3));
        eliteEnemy->Add(EnemyInstruction::SetVelocity(Vector2D(0, -3.f)));
        eliteEnemy->Add(EnemyInstruction::Wait(90));
        eliteEnemy->Add(EnemyInstruction::SetVelocity(Vector2D(0, 0)));
        eliteEnemy->Add(EnemyInstruction::Wait(30));
        eliteEnemy->Add(EnemyInstruction::Fire("elite_enemy_rotating_wave_16way"));
        eliteEnemy->Add(EnemyInstruction::Wait(400));
        eliteEnemy->Add(EnemyInstruction::Fire("elite_enemy_rotating_wave_16way"));
        eliteEnemy->Add(EnemyInstruction::Wait(460));
        eliteEnemy->Add(EnemyInstruction::Accelerate(Vector2D(0, -0.02f)));
        eliteEnemy->Add(EnemyInstruction::End());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("EliteEnemy02", eliteEnemy);
    }
    {
        std::shared_ptr<EnemyScript> eliteEnemy = std::make_shared<EnemyScript>();
        eliteEnemy->Add(EnemyInstruction::SetHp(600));
        eliteEnemy->Add(EnemyInstruction::AddDropItem(ItemType::PowerBig, 3));
        eliteEnemy->Add(EnemyInstruction::AddDropItem(ItemType::Score, 12));
        eliteEnemy->Add(EnemyInstruction::AddDropItem(ItemType::ScoreBig, 3));
        eliteEnemy->Add(EnemyInstruction::SetVelocity(Vector2D(0, -3.f)));
        eliteEnemy->Add(EnemyInstruction::Wait(90));
        eliteEnemy->Add(EnemyInstruction::SetVelocity(Vector2D(0, 0)));
        eliteEnemy->Add(EnemyInstruction::Wait(30));
        eliteEnemy->Add(EnemyInstruction::Fire("elite_enemy_rotating_wave_36way"));
        eliteEnemy->Add(EnemyInstruction::Wait(400));
        eliteEnemy->Add(EnemyInstruction::Fire("elite_enemy_rotating_wave_36way"));
        eliteEnemy->Add(EnemyInstruction::Wait(460));
        eliteEnemy->Add(EnemyInstruction::Accelerate(Vector2D(0, -0.02f)));
        eliteEnemy->Add(EnemyInstruction::End());
        EnemyScriptManager::GetInstance().RegisterEnemyScript("EliteEnemy03", eliteEnemy);
    }
}