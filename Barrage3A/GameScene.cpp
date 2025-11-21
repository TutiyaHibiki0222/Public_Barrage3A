#include "GameScene.h"

#include "GameManager.h"
#include "BuildUI.hpp"
#include "common.h"

#include "ParticleSystem.h"
#include "AudioResourceShortcut.hpp"

#include "ShiftAura.h"
#include "BarrelDistortShader.h"
#include "Sprite3DRenderer.h"
#include "BossAuraController.h"
#include "Pseudo3DBackgroundManager.h"

// ステージ関係.
#include "Stage3.h"
#include "Tmp.h"

#include "TileTransitor.h"

#include "Prefab.h"
#include "MusicController.h"
#include "HUDManager.h"
#include "EnemyManager.h"
#include "ItemManager.h"

#include "HpGauge.h"
#include "ResourceManager.h"
#include "AnimatorOverride.h"
#include "ReimuHakurei.h"

void GameScene::Init() {
	Sounds["#?gameOver"];
	GameManager::GetInstance().ResetGameState();
	GameManager::GetInstance().GetScoreManager().LoadHiScore();
	// UI生成.
	{
		auto canvas = CreateUI::CreateUiJsonData(System::IO::Path::Combine(LoadFilePath, "Canvas", "ui_data.json"));
		for (auto tex : canvas->transform->GetAppBasesInChildren<Text>()) {
			tex->SetSpriteFont(Texture2DManager::GetInstance().GetSpriteFont("number_main"));
		}
		auto hud = canvas->transform->FindByPath("gameUi/Boders");

		HUDManager::GetInstance().Initialize(
			hud->FindName("HiScore")->GetAppBaseInChildren<Text>(),
			hud->FindName("Score")->GetAppBaseInChildren<Text>(),
			hud->FindName("Lifes")->GetAppBasesInChildren<Image>(),
			hud->FindName("LifeFragmentsText")->GetAppBaseInChildren<Text>(),
			hud->FindName("Boms")->GetAppBasesInChildren<Image>(),
			hud->FindName("BomFragmentsText")->GetAppBaseInChildren<Text>(),
			hud->FindName("reiryoku")->GetAppBaseInChildren<Text>(),
			hud->FindName("MaxAddScore")->GetAppBaseInChildren<Text>(),
			hud->FindName("GrazeCount")->GetAppBaseInChildren<Text>(),
			hud->FindName("TotalGrazeCount")->GetAppBaseInChildren<Text>(),
			canvas->transform->FindName("fpsText")->GetAppBaseInChildren<Text>()
		);

		HUDManager::GetInstance().SetPause(canvas->transform->FindByPath("gameUi/Pause")->GetGameObject());
	}

	EnemyManager::GetInstance().Init();
	std::vector<std::string> enemyCsvs = {
	LoadStage1CSV.string(),
	LoadStage2CSV.string()
	};
	EnemyManager::GetInstance().AddCSVList(enemyCsvs);

	{
		var obj = GameObject::Instantiate("Back");
		var mgr = obj->AddAppBase<Pseudo3DBackgroundManager>();
		mgr->SetBackground(std::make_shared<Stage3>());
	}

	{
		var player = GameObject::Instantiate("Player");
		player->transform->scale = { 1.25f,1.25f };
		player->SetTag("Player");
		player->AddAppBase<ReimuHakurei>();
		player->transform->position = Vector2D(-150, -300);
		auto sp = player->AddAppBase<SpriteRenderer>();
		player->SetLayer(Layer::Player);

		GameManager::GetInstance().SetPlayer(player);

		auto p = Texture2DManager::GetInstance().GetTexture2D("Player", "霊夢");
		auto anime = player->AddAppBase(Resource::GetPtrClone<Animator>("PlayerAnimator"));
		anime->SetOverrideController(Resource::GetPtrClone<AnimatorDef::AnimatorOverride>("ReimuAnimationData"));
		if (!PrefabMgr.HasPrefab("Body")) {

			// プレハブ作成
			auto bodyPrefab = PrefabMgr.CreatePrefab("Body");

			// Border1
			auto show1 = bodyPrefab->AddPrefab(PrefabMgr.CreatePrefabObj("Border"));
			{
				auto sp1 = show1->AddAppBase<SpriteRenderer>();
				sp1->SetLayer(2);
				sp1->SetSprite(Tex("Player/Slow/Body")->GetTextureData("Border"));

				auto s = show1->AddAppBase<ShiftAura>();
				s->SetAlphaRange(0, 0.5f);
				s->SetRotation(0, 200.0f, 270);
				s->SetScaleRange(0.2f, 1);
				s->SetAppearTime(0.3f);
			}

			// Border2
			auto show2 = bodyPrefab->AddPrefab(PrefabMgr.CreatePrefabObj("Border"));
			{
				auto sp2 = show2->AddAppBase<SpriteRenderer>();
				sp2->SetLayer(2);
				sp2->SetSprite(Tex("Player/Slow/Body")->GetTextureData("Border"));

				auto s = show2->AddAppBase<ShiftAura>();
				s->SetAlphaRange(0, 0.5f);
				s->SetRotation(0, -75.0f, -270);
				s->SetScaleRange(0.2f, 1);
				s->SetAppearTime(0.3f);
			}

			// Dot
			auto dot = bodyPrefab->AddPrefab(PrefabMgr.CreatePrefabObj("Dot"));
			{
				auto sp3 = dot->AddAppBase<SpriteRenderer>();
				sp3->SetLayer(5);
				sp3->SetSprite(Tex("Player/Slow/Body")->GetTextureData("Dot"));

				auto s = dot->AddAppBase<ShiftAura>();
				s->SetAlphaRange(0, 180);
				s->SetAppearTime(0.3f);
			}

			// Graze
			auto graze = bodyPrefab->AddPrefab(PrefabMgr.CreatePrefabObj("Graze"));
			{
				graze->SetLayer(Layer::Graze);
				graze->SetTag("Graze");
				graze->AddAppBase<CircleCollider>()
					->SetRadius((float)(Tex("Player/Slow/Body")->GetTextureData("Border")->width / 2));
			}
		}

		auto body = PrefabMgr.Instantiate("Body");
		for (auto& go : body) {
			go->transform->SetParent(player->transform, false);
		}
	}
	{
		// Prefab.
		if (!PrefabMgr.HasPrefab("Enemy01"))
		{
			var prefab = PrefabMgr.CreatePrefab("Enemy01");
			prefab->SetTag("Enemy");
			prefab->SetLayer(Layer::Enemy);
			prefab->AddAppBase<BoxCollider>()->SetSize({ 8,8 });
			auto sp = prefab->AddAppBase<SpriteRenderer>();
			sp->SetSprite(Tex["Enemys/Mini:Enemy_1"]);
			sp->SetLayer(3);

			// 通常向き.
			auto clipIdle = std::make_shared<AnimatorDef::AnimationClip>();
			clipIdle->duration = 1.0f;
			clipIdle->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRangePingPong("Enemy", 0, 4);
			clipIdle->loop = true;

			auto clipTurnRight = std::make_shared<AnimatorDef::AnimationClip>();
			clipTurnRight->duration = 0.4f;
			clipTurnRight->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRange("Enemy", 4, 7);
			clipTurnRight->loop = false;

			auto clipTurnReturn = std::make_shared<AnimatorDef::AnimationClip>();
			clipTurnReturn->duration = 0.4f;
			clipTurnReturn->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRange("Enemy", 7, 4);
			clipTurnReturn->loop = false;

			auto clipMoveRight = std::make_shared<AnimatorDef::AnimationClip>();
			clipMoveRight->duration = 1.0f;
			clipMoveRight->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRangePingPong("Enemy", 8, 11);
			clipMoveRight->loop = true;  // 移動はループ想定

			// ステート定義
			AnimatorDef::AnimatorState idleState{ "Idle", clipIdle };

			AnimatorDef::AnimatorState turnEffectState{ "TurnEffect", clipTurnRight, {}, };

			AnimatorDef::AnimatorState moveState{ "Move", clipMoveRight, {}, };

			AnimatorDef::AnimatorState turnReturnState{ "TurnReturn", clipTurnReturn, {} };

			// 遷移作成
			// Idle → TurnEffect（isMoving==true）
			AnimatorDef::AnimatorTransition idleToTurnEffect;
			idleToTurnEffect.toStateName = "TurnEffect";
			idleToTurnEffect.AddBoolCondition("isMoving", true);

			// TurnEffect → Move（クリップ終了時に遷移）
			AnimatorDef::AnimatorTransition turnEffectToMove;
			turnEffectToMove.toStateName = "Move";
			turnEffectToMove.hasExitTime = true; // クリップ終了で遷移

			// Move → TurnReturn（isMoving==false）
			AnimatorDef::AnimatorTransition moveToTurnReturn;
			moveToTurnReturn.toStateName = "TurnReturn";
			moveToTurnReturn.AddBoolCondition("isMoving", false);

			// TurnReturn → Idle（クリップ終了時に遷移）
			AnimatorDef::AnimatorTransition turnReturnToIdle;
			turnReturnToIdle.toStateName = "Idle";
			turnReturnToIdle.hasExitTime = true;

			// 各ステートに遷移を追加
			idleState.transitions.push_back(idleToTurnEffect);
			turnEffectState.transitions.push_back(turnEffectToMove);
			moveState.transitions.push_back(moveToTurnReturn);
			turnReturnState.transitions.push_back(turnReturnToIdle);

			// Animatorに登録
			auto animator = prefab->AddAppBase<Animator>();
			animator->AddState(idleState);
			animator->AddState(turnEffectState);
			animator->AddState(moveState);
			animator->AddState(turnReturnState);

			// パラメータ追加
			animator->AddParameter("isMoving", AnimatorDef::AnimatorParameterType::Bool);


		}
		if (!PrefabMgr.HasPrefab("Enemy02"))
		{
			var prefab = PrefabMgr.CreatePrefab("Enemy02");
			prefab->SetTag("Enemy");
			prefab->SetLayer(Layer::Enemy);
			prefab->AddAppBase<BoxCollider>()->SetSize({ 8,8 });
			auto sp = prefab->AddAppBase<SpriteRenderer>();
			sp->SetSprite(Tex["Enemys/Mini:Enemy_2"]);
			sp->SetLayer(3);
			// 通常向き.
			auto clipIdle = std::make_shared<AnimatorDef::AnimationClip>();
			clipIdle->duration = 1.0f;
			clipIdle->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRangePingPong("Enemy", 12, 15);
			clipIdle->loop = true;

			auto clipTurnRight = std::make_shared<AnimatorDef::AnimationClip>();
			clipTurnRight->duration = 0.4f;
			clipTurnRight->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRange("Enemy", 16, 19);
			clipTurnRight->loop = false;

			auto clipTurnReturn = std::make_shared<AnimatorDef::AnimationClip>();
			clipTurnReturn->duration = 0.4f;
			clipTurnReturn->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRange("Enemy", 19, 16);
			clipTurnReturn->loop = false;

			auto clipMoveRight = std::make_shared<AnimatorDef::AnimationClip>();
			clipMoveRight->duration = 1.0f;
			clipMoveRight->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRangePingPong("Enemy", 20, 23);
			clipMoveRight->loop = true;  // 移動はループ想定

			// ステート定義
			AnimatorDef::AnimatorState idleState{ "Idle", clipIdle };

			AnimatorDef::AnimatorState turnEffectState{ "TurnEffect", clipTurnRight, {}, };

			AnimatorDef::AnimatorState moveState{ "Move", clipMoveRight, {}, };

			AnimatorDef::AnimatorState turnReturnState{ "TurnReturn", clipTurnReturn, {} };

			// 遷移作成
			// Idle → TurnEffect（isMoving==true）
			AnimatorDef::AnimatorTransition idleToTurnEffect;
			idleToTurnEffect.toStateName = "TurnEffect";
			idleToTurnEffect.AddBoolCondition("isMoving", true);

			// TurnEffect → Move（クリップ終了時に遷移）
			AnimatorDef::AnimatorTransition turnEffectToMove;
			turnEffectToMove.toStateName = "Move";
			turnEffectToMove.hasExitTime = true; // クリップ終了で遷移

			// Move → TurnReturn（isMoving==false）
			AnimatorDef::AnimatorTransition moveToTurnReturn;
			moveToTurnReturn.toStateName = "TurnReturn";
			moveToTurnReturn.AddBoolCondition("isMoving", false);

			// TurnReturn → Idle（クリップ終了時に遷移）
			AnimatorDef::AnimatorTransition turnReturnToIdle;
			turnReturnToIdle.toStateName = "Idle";
			turnReturnToIdle.hasExitTime = true;

			// 各ステートに遷移を追加
			idleState.transitions.push_back(idleToTurnEffect);
			turnEffectState.transitions.push_back(turnEffectToMove);
			moveState.transitions.push_back(moveToTurnReturn);
			turnReturnState.transitions.push_back(turnReturnToIdle);

			// Animatorに登録
			auto animator = prefab->AddAppBase<Animator>();
			animator->AddState(idleState);
			animator->AddState(turnEffectState);
			animator->AddState(moveState);
			animator->AddState(turnReturnState);

			// パラメータ追加
			animator->AddParameter("isMoving", AnimatorDef::AnimatorParameterType::Bool);
		}
		if (!PrefabMgr.HasPrefab("EliteEnemy01"))
		{
			var prefab = PrefabMgr.CreatePrefab("EliteEnemy01");
			prefab->SetTag("Enemy");
			prefab->SetLayer(Layer::Enemy);
			prefab->AddAppBase<CircleCollider>()->SetRadius(16);
			auto sp = prefab->AddAppBase<SpriteRenderer>();
			sp->SetSprite(Tex["Enemys/Elite:EliteEnemy01_2"]);
			sp->SetLayer(3);
			// 通常向き.
			auto clipIdle = std::make_shared<AnimatorDef::AnimationClip>();
			clipIdle->duration = 1.0f;
			clipIdle->frames = Tex("Enemys/Elite")->GetTexturesByPrefixRangePingPong("EliteEnemy01", 0, 3);
			clipIdle->loop = true;

			auto clipTurnRight = std::make_shared<AnimatorDef::AnimationClip>();
			clipTurnRight->duration = 0.4f;
			clipTurnRight->frames = Tex("Enemys/Elite")->GetTexturesByPrefixRange("EliteEnemy01", 4, 7);
			clipTurnRight->loop = false;

			auto clipTurnReturn = std::make_shared<AnimatorDef::AnimationClip>();
			clipTurnReturn->duration = 0.4f;
			clipTurnReturn->frames = Tex("Enemys/Elite")->GetTexturesByPrefixRange("EliteEnemy01", 7, 4);
			clipTurnReturn->loop = false;

			auto clipMoveRight = std::make_shared<AnimatorDef::AnimationClip>();
			clipMoveRight->duration = 1.0f;
			clipMoveRight->frames = Tex("Enemys/Elite")->GetTexturesByPrefixRangePingPong("Enemy", 20, 23);
			clipMoveRight->loop = true;  // 移動はループ想定

			// ステート定義
			AnimatorDef::AnimatorState idleState{ "Idle", clipIdle };

			AnimatorDef::AnimatorState turnEffectState{ "TurnEffect", clipTurnRight, {}, };

			AnimatorDef::AnimatorState moveState{ "Move", clipMoveRight, {}, };

			AnimatorDef::AnimatorState turnReturnState{ "TurnReturn", clipTurnReturn, {} };

			// 遷移作成
			// Idle → TurnEffect（isMoving==true）
			AnimatorDef::AnimatorTransition idleToTurnEffect;
			idleToTurnEffect.toStateName = "TurnEffect";
			idleToTurnEffect.AddBoolCondition("isMoving", true);

			// TurnEffect → Move（クリップ終了時に遷移）
			AnimatorDef::AnimatorTransition turnEffectToMove;
			turnEffectToMove.toStateName = "Move";
			turnEffectToMove.hasExitTime = true; // クリップ終了で遷移

			// Move → TurnReturn（isMoving==false）
			AnimatorDef::AnimatorTransition moveToTurnReturn;
			moveToTurnReturn.toStateName = "TurnReturn";
			moveToTurnReturn.AddBoolCondition("isMoving", false);

			// TurnReturn → Idle（クリップ終了時に遷移）
			AnimatorDef::AnimatorTransition turnReturnToIdle;
			turnReturnToIdle.toStateName = "Idle";
			turnReturnToIdle.hasExitTime = true;

			// 各ステートに遷移を追加
			idleState.transitions.push_back(idleToTurnEffect);
			turnEffectState.transitions.push_back(turnEffectToMove);
			moveState.transitions.push_back(moveToTurnReturn);
			turnReturnState.transitions.push_back(turnReturnToIdle);

			// Animatorに登録
			auto animator = prefab->AddAppBase<Animator>();
			animator->AddState(idleState);
			animator->AddState(turnEffectState);
			animator->AddState(moveState);
			animator->AddState(turnReturnState);

			// パラメータ追加
			animator->AddParameter("isMoving", AnimatorDef::AnimatorParameterType::Bool);
		}
	}
	{
#if _DEBUG
		auto boss = GameObject::Instantiate("boss");
		boss->transform->position = Vector2D(-300, 300);
		boss->transform->scale = Vector2D(1.25f, 1.25f);
		boss->AddAppBase<BarrelDistort>();
		auto sp = boss->AddAppBase<Sprite3DRenderer>();
		sp->SetLayer(-6);
		sp->SetSprite(Sprite::MakeLoadImage("Resources/Images/Enemy/Boss/BossMahouJin.png"));
		sp->SetRotation({ 0, 50, 180 });
		sp->SetColor({ 255,255,255,175 });
		boss->AddAppBase<BossAuraController>()->SetAuraRenderer(sp);
		auto hp = boss->AddAppBase<HpGauge>();
		hp->SetSprite(Sprite::MakeLoadImage("Resources/Images/Enemy/Boss/HpMeter_Donut.png"));
		hp->SetPointSprite(Sprite::MakeLoadImage("Resources/Images/Enemy/Boss/SpellPoint.png"));
		hp->AddOutsideLine(175 / 2);
		hp->AddOutsideLine(175 / 2 - 5);
		hp->Damage(90);
#endif
	}
}


void GameScene::Update()
{
	EnemyManager::GetInstance().Update();
	ItemManager::GetInstance().Update();

	HUDManager::GetInstance().Update();
}

void GameScene::Draw() {
#if _DEBUG
	DrawString(0, 0, "ゲームシーン", 0xFFFFFF);
#endif
}

void GameScene::Release() {
	Object.DestroySceneObjects();
	ItemManager::GetInstance().Clear();
	HUDManager::GetInstance().Reset();
	Sounds["#!gameOver"];
}
