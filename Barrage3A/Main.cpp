//// すべてのヘッダーファイルたち.
#include "headers.h"
#include "resource.h"
#include "GameObject.h"
#include "Prefab.h"
#include "Coroutine.hpp"
#include "EditorUtility.hpp"
#include "GameWorldManager.hpp"

#include "Texture2DManager.hpp"
#include "LayerManager.h"
#include "Header/BulletBase.h"
// test
#include "Text.h"
#include "common.h"
#include "RendererManager.h"
#include "WipeTransitor.h"
#include "TileTransitor.h"
#include "AudioResource.hpp"
#include "ParticleSystem.h"
#include "PathManager.h"
#include "BulletType.h"
#include "Script.h"
#include "EnemyFarm.h"
#include "Dx3DCamera.h"
#include "CreateAnimation.h"
using namespace GameEngine;
using namespace GameEditor;
using namespace System;


System::Coroutine Test() {
	float time = 0.0f;
    while (true)
    {
        if (Input.IsKey(KeyCode::T)) {
            SceneManager::GetInstance().SetTransitor<TileTransitor>(30);
            SceneManager::GetInstance().LoadScene(SceneType::Title);
        }
        if (Input.IsKey(KeyCode::G)) {
            SceneManager::GetInstance().SetTransitor<WipeTransitor>(30);
            SceneManager::GetInstance().LoadScene(SceneType::Game);
        }

        if(Input.IsPadButton<0>(PadCode::Start) || Input.IsKey(KeyCode::ESCAPE)) {
			time += Time.deltaTime;
            if (time >= 3.0f) {
				Application::GetInstanse().Quit();
            }
		}
        else {
			time = 0.0f;
        }

        _yield null;
    }
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance, LPSTR lpCmdLine, int nCmdShow)
{
    auto& windows = Window::GetInstance();
    windows.SetWindowName("東方響影夢");
    windows.InitWindow();
#if !defined(_DEBUG)
    windows.SetFullScreen(true);
#endif
    windows.MaximizeWindow();
    windows.SetAlwaysRunFlag(true);
    windows.SetWaitVSyncFlag(false);
    windows.SetWindowIcon(IDI_MYAPP_ICON);
    Application::GetInstanse().targetFrameRate = 60;
    
#if _DEBUG
    windows.EnableConsole();
#endif
    GameWorldManager::GetInstance().SetWorldPosition(windows.GetMaxVector2D() / 2);
    if (!Engine::Instance().InitGame()) return -1;
    {
        Texture2DManager::GetInstance().LoadFileCsv(LoadBulletImageCSV.string());
        Texture2DManager::GetInstance().LoadFileCsv(LoadEffectImageCSV.string());
        Texture2DManager::GetInstance().LoadFileCsv(LoadPlayerImageCSV.string());
        Texture2DManager::GetInstance().LoadFileCsv(LoadEnemyImageCSV.string());
        Texture2DManager::GetInstance().LoadFileCsv(LoadStageImageCSV.string());
        Texture2DManager::GetInstance().LoadFileCsv(LoadUIImageCSV.string());
        Texture2DManager::GetInstance().LoadFileCsv(LoadItemImageCSV.string());

        // fontTexture読み込み.
        {
            auto spriteFont = std::make_shared<SpriteFont>();
            spriteFont->LoadFontTextureWithMap("Resources/Images/UI/Font/number_main.png", "0123456789/.sx,", 32, 32, 16);
            spriteFont->SetCharKerning(',', -20.0f);
            spriteFont->SetCharKerning('.', -20.0f);
            spriteFont->SetCharKerning('/', -15.0f);
            spriteFont->SetCharKerning("0123456789", -7.5f);
            spriteFont->SetCharOffset(',', 2, 6);
            spriteFont->SetCharOffset('.', -2, 0);
			Texture2DManager::GetInstance().AddSpriteFont("number_main", spriteFont);
        }
        {
            auto spriteFont = std::make_shared<SpriteFont>();
            spriteFont->LoadFontTextureWithMap("Resources/Images/UI/Font/number.png", "0123456789", 12, 11, 10);
            Texture2DManager::GetInstance().AddSpriteFont("scoreNumber", spriteFont);
        }
        
        AudioResource::GetInstance().LoadFromCSV(LoadAudioSE_CSV.string());
        AudioResource::GetInstance().LoadSetFromCSV(LoadAudioBGM_CSV.string());
    }

    // タイトルシーンに切り替え.
    SceneManager::GetInstance().Init(SceneType::Title);

    //Bullet生成
    var bullets = PrefabMgr.CreatePrefab("Bullet_Base");
    bullets->AddAppBase<BulletControl>();

    {
        //Engine::Instance().SetFixed(50);
        CollisionManager::GetInstance().SetColliderCheckMode(CollisionCheckMode::Layer_Vs_Layer);
        
        // 全てのレイヤー同士の衝突を無効化.
		LayerManager::GetInstance().AllCollisioneMatrix(false);
		// 各レイヤーのみ衝突を有効化.
		LayerManager::GetInstance().SetCollisionMask(Layer::Player,         Layer::EnemyBullet  , true);
        LayerManager::GetInstance().SetCollisionMask(Layer::Player,         Layer::Enemy        , true);
        LayerManager::GetInstance().SetCollisionMask(Layer::Player,         Layer::Item         , true);
        LayerManager::GetInstance().SetCollisionMask(Layer::PlayerBullet,   Layer::Enemy        , true);
        LayerManager::GetInstance().SetCollisionMask(Layer::Graze       ,   Layer::EnemyBullet  , true);
        LayerManager::GetInstance().SetCollisionMask(Layer::PlayerBullet,   Layer::EnemyBullet  , false);
    }
    // 敵バレットの設定.
    {
        BulletTypeManager::GetInstance().LoadJson(LoadBulletTypeDataJson.string());
    }
    // グレイズEffect.
    {
        var grazePrefab = PrefabMgr.CreatePrefab("GrazeEffect", "Graze");
        var particle = grazePrefab->AddAppBase<ParticleSystem>();
        particle->SetSprite(Sprite::MakeCircle(8));
        particle->SetEndDestroy(true);
        particle->GetSortingLayer().layer = 3;
        var& seting = particle->GetParticleSettings();
        seting.maxParticles = 10;
        seting.burstCount   = 5;
        seting.lifetimeMin  = 0.25f;
        seting.lifetimeMax  = 0.5f;
        seting.startColor   = Color(255, 255, 255, 255);
    }
    // 敵の弾が消えた再のエフェクト.
    {
        var effectPrefab = PrefabMgr.CreatePrefab("EnemyBulletEffect", "Effect");
        var particle = effectPrefab->AddAppBase<ParticleSystem>();
        particle->SetSprite(Sprite::MakeStar(16));
        particle->SetEndDestroy(true);
        particle->GetSortingLayer().layer = 6;
        var& seting = particle->GetParticleSettings();
        seting.maxParticles = 10;
        seting.burstCount   = 5;
        seting.lifetimeMin  = 0.3f;
        seting.lifetimeMax  = 0.5f;
        seting.sizeMin      = 1;
        seting.sizeMax      = 2.5f;
        seting.speedMin     = 0;
        seting.speedMax     = 0;
        seting.startRotationMax = { 0,0,360 };
        seting.useRandomStartColor = true;
    }
    // 敵が撃破 再のエフェクト.
    {
        var effectPrefab = PrefabMgr.CreatePrefab("EnemyDestroyEffect", "Effect");
        // 葉っぱ　パーティクル.
        {
            var particle = effectPrefab->AddAppBase<ParticleSystem>();
            particle->SetSprite(Tex["Effects/Leaf:Leaf01"]);
            particle->SetEndDestroy(true);
            particle->GetSortingLayer().layer = 3;
            var& seting = particle->GetParticleSettings();
            seting.maxParticles = 10;
            seting.burstCount = 10;
            seting.lifetimeMin = 1.2f;
            seting.lifetimeMax = 1.2f;
            seting.sizeMin = 0.25;
            seting.sizeMax = 0.75f;
            seting.speedMin = 125.0f;
            seting.speedMax = 300.0f;
            seting.startRotationMin = { -180,-180,-180 };
            seting.startRotationMax = { 180, 180, 180 };
            seting.rotationSpeedMin = { -180,-180,-180 };
            seting.rotationSpeedMax = { 180, 180, 180 };
            seting.useRandomStartColor = true;
            seting.startColorMin = { 0,  0,  0,100 };
            seting.startColorMax = { 255,255,255,200 };
            seting.customUpdate = PARTICLE_FUNC {
                // -------------------------
                // 残り寿命に応じた進行度 u
                // -------------------------
                float total = (p.totalLife > 1e-6f) ? p.totalLife : 1e-6f;
                float elapsed = p.totalLife - p.life;
                float u = elapsed / total;
                u = std::clamp(u, 0.0f, 1.0f);

                // -------------------------
                // 位置更新
                // -------------------------
                float maxU = 0.9f;
                float t = std::clamp(u / maxU, 0.0f, 1.0f); // 0~1 に正規化
                float moveFactor = 1.0f - t;                 // 0→1 で減速
                Vector2D move = p.vc * moveFactor * Time.deltaTime;
                p.pos += move;

                p.rot.x += p.rotSpeed.x * Time.deltaTime;
                p.rot.y += p.rotSpeed.y * Time.deltaTime;
                p.rot.z += p.rotSpeed.z * Time.deltaTime;

                // -------------------------
                // サイズ変化: 60% -> 100% にかけて 0 へ
                // -------------------------
                float sizeStartU = 0.6f;
                if (u <= sizeStartU) {
                    p.sizeCurrent = 0.8f * p.size;
                }
                else {
                    float t = (u - sizeStartU) / (1.0f - sizeStartU);
                    p.sizeCurrent = 0.8f * p.size * (1.0f - t);
                }
            };
        }
		// 光の輪　パーティクル.
        {
            var particle = effectPrefab->AddAppBase<ParticleSystem>();
            particle->SetCustomDraw([](const Particle& p, const Sprite& sp) {
                SetDrawBlendMode(DX_BLENDMODE_ADD, p.color.A255());
                DrawCircleAA(p.pos.x, p.pos.y, p.sizeCurrent * 20, 100, p.color.ToPackedRGB(), false);
            });
            particle->GetSortingLayer().layer = 3;
            var& seting = particle->GetParticleSettings();
            seting.maxParticles     = 4;
            seting.spawnRadiusMax   = 12.0f;
			seting.burstCount       = 4;
            seting.lifetimeMin      = 0.5f;
            seting.lifetimeMax      = 0.5f;
            seting.sizeMin          = 0.0f;
            seting.sizeMax          = 0.0f;
            seting.endSizeMin       = 1.0f;
			seting.endSizeMax       = 5.0f;
            seting.useRandomStartColor = true;
            seting.startColorMin = { 0  ,0  ,0  ,175 };
            seting.startColorMax = { 255,255,255,175 };
            seting.customUpdate = PARTICLE_FUNC{
                float total = (p.totalLife > 1e-6f) ? p.totalLife : 1e-6f;
                float elapsed = p.totalLife - p.life;
                float u = elapsed / total;
                u = Mathf::Clamp(u, 0.0f, 1.0f);

                // イージング
                auto EaseOutQuad = [](float t) -> float {
                    return 1.0f - (1.0f - t) * (1.0f - t);
                };
                float eased = EaseOutQuad(u);

                // 補間して現在サイズ
                p.sizeCurrent = p.size + (p.targetSize - p.size) * eased;

                float alphaMul = 1.0f;
                const float fadeStart = 0.75f;
                if (u > fadeStart) {
                    float fadeT = (u - fadeStart) / (1.0f - fadeStart); // 0 -> 1
                    fadeT = Mathf::Clamp(fadeT, 0.0f, 1.0f);
                    alphaMul = 1.0f - fadeT;
                }
                p.color.a = p.startColor.a * alphaMul;
            };
        }
    }
    /*
    {
		var effectObj = GameObject::Instantiate("Effect");
        effectObj->transform->position = Vector2D(-200, 0);
        var effect = effectObj->AddAppBase<ParticleSystem>();
        effect->GetSortingLayer().layer = 1;
        effect->SetSprite(Tex["Effects/Leaf:Leaf01"]);
        effect->GetParticleSettings().rotationSpeedMax.z = 360;
        effect->GetParticleSettings().rotationSpeedMax.x = 360;
        effect->GetParticleSettings().startRotationMax.z = 360;
        effect->GetParticleSettings().emissionDuration   = 0.75f;
        effect->GetParticleSettings().emissionInterval   = 0.05f;
        effect->GetParticleSettings().lifetimeMin        = 0.8f;
        effect->GetParticleSettings().lifetimeMax        = 0.8f;
        effect->GetParticleSettings().sizeMin            = 0.5f;
        effect->GetParticleSettings().sizeMax            = 1.0f;
        effect->GetParticleSettings().spawnRadiusMin     = 200;
        effect->GetParticleSettings().spawnRadiusMax     = 500;
        ParticleFunc func = PARTICLE_FUNC {
            float dt = Time.deltaTime;

            float dirX = pos.x - p.pos.x;
            float dirY = pos.y - p.pos.y;
            float dist = sqrtf(dirX * dirX + dirY * dirY);

            if (dist > 0.01f) {
                dirX /= dist;

                dirY /= dist;
                // 残り寿命で割った距離に基づく速度で移動（寿命内に必ず中心へ）
                float speed = dist / p.life;

                p.pos.x += dirX * speed * dt;
                p.pos.y += dirY * speed * dt;
            }

            float t = p.life / p.totalLife;

            // フェードイン・フェードアウト
            if (t > 0.5f) {
                // 前半：フェードイン
                p.color.a = (1.0f - (t - 0.5f) * 2.0f);
            }
            else {
                // 後半：フェードアウト
                p.color.a = (t * 2.0f);
            }

            p.sizeCurrent = p.size;
            p.rot.x += p.rotSpeed.x * dt;
            p.rot.y += p.rotSpeed.y * dt;
            p.rot.z += p.rotSpeed.z * dt;
            };
        effect->GetParticleSettings().customUpdate = func;
        //effect->GetParticleSettings().emissionDuration = 10;
        //effect->SetEndDestroy(true);
    }
    */
    StartCoroutine(Test());

    {
        // PathManagerに登録（ID=1で例示）
        PathManager::GetInstance().AddJsonPath(0, "Resources/LoadFile/EnemyMovePath/path0.json");
        PathManager::GetInstance().AddJsonPath(1, "Resources/LoadFile/EnemyMovePath/path_0.json");
        PathManager::GetInstance().AddJsonPath(2, "Resources/LoadFile/EnemyMovePath/path_1.json");
        PathManager::GetInstance().AddJsonPath(3, "Resources/LoadFile/EnemyMovePath/path2_1.json");
        PathManager::GetInstance().AddJsonPath(4, "Resources/LoadFile/EnemyMovePath/path4.json");
        PathManager::GetInstance().AddJsonPath(5, "Resources/LoadFile/EnemyMovePath/pathL.json");
        PathManager::GetInstance().AddJsonPath(6, "Resources/LoadFile/EnemyMovePath/pathR.json");
    }

    {
        RegisterBulletScripts();
        RegisterShotScripts();
		RegisterEnemyScripts();
        RegisterEliteEnemyScripts();
        CreatePlayerAnimator();
        CreateBulletAnimator();
    }

    Engine::Instance().Run();

	DxLib::DxLib_End();	//DXライブラリの終了処理.
	return 0;		//終了.
}
