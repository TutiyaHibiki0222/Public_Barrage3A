#include "TitleScene.h"

#include "BuildUI.hpp"
#include "common.h"

#include "GameObjectMgr.h"

#include "TileTransitor.h"
#include "TitleUIManager.h"
#include "MusicController.h"
#include "ParticleSystem.h"


void TitleScene::Init() {

	Time.timeScale = 1;

	CreateUI::CreateUiJsonData(System::IO::Path::Combine(LoadFilePath, "Canvas", "title_ui_data.json"));
	auto canvasObj = CreateUI::CreateUiJsonData(System::IO::Path::Combine(LoadFilePath, "Canvas", "title_ui_ux_data.json"));
	// UI制御の初期化.
	{
		auto titleUI = canvasObj->AddAppBase<TitleUIManager>();
		auto add = [&](const std::string& path, const std::string& normalKey, const std::string& selectedKey, bool isNoSelect = false,float time = 1.0f, const std::function<void()> fan = nullptr) {
			if (auto get = canvasObj->transform->FindByPath(path))
			{
				auto image = get->GetGameObject()->GetAppBase<Image>();
				titleUI->AddPanel(image, normalKey, selectedKey, fan, isNoSelect);
			}
			};

		add("Canvas/GameStart", "UI/Fonts:TextPanelText_1", "UI/Fonts:TextPanelText_0", false,1.5f,
			[]() {
				SceneManager::GetInstance().SetTransitor<TileTransitor>(60);
				SceneManager::GetInstance().LoadScene(SceneType::Game);
			}
		);
		add("Canvas/ExtraStart", "UI/Fonts:TextPanelText_3", "UI/Fonts:TextPanelText_2", true);
		add("Canvas/Practice", "UI/Fonts:TextPanelText_5", "UI/Fonts:TextPanelText_4", true);
		add("Canvas/SpellPractice", "UI/Fonts:TextPanelText_7", "UI/Fonts:TextPanelText_6", true);
		add("Canvas/Replay", "UI/Fonts:TextPanelText_9", "UI/Fonts:TextPanelText_8", true);
		add("Canvas/PlayerData", "UI/Fonts:TextPanelText_11", "UI/Fonts:TextPanelText_10", true);
		add("Canvas/MusicRoom", "UI/Fonts:TextPanelText_13", "UI/Fonts:TextPanelText_12", true);
		add("Canvas/Option", "UI/Fonts:TextPanelText_15", "UI/Fonts:TextPanelText_14", true);
		add("Canvas/Manual", "UI/Fonts:TextPanelText_17", "UI/Fonts:TextPanelText_16", true);
		add("Canvas/Quit", "UI/Fonts:TextPanelText_19", "UI/Fonts:TextPanelText_18", false,0.25f,
			[]() {
				Application::GetInstanse().Quit();
			}
		);
	}

	// 読み込み.
	Sounds["#?title"];
	// もしBGMを最初に鳴らしたいならここで再生開始
	auto musicController = GameObject::Instantiate("MusicController")->AddAppBase<MusicController>();
	if (musicController) {
		auto bgmClip = Sounds["bgm_title"];
		musicController->SetMusicClip(bgmClip);
		musicController->SetLoopSong(true);
		musicController->SetMusicVolume(0.8f);
	}
	// タイトルの演出.
	{
		var effectObj = GameObject::Instantiate("Effect");
		effectObj->transform->rotation = 10;
		//float offset   = HEIGHT * 0.75f;
		float CENTER_Y = -HEIGHT / 2.0f;
		effectObj->transform->position.y = CENTER_Y - 250;
		var effect = effectObj->AddAppBase<ParticleSystem>();
		effect->SetSprite(Tex["Effects/Effect:effect000"]);
		effect->SetRenderMode(RenderMode::ScreenSpaceOverlay);
		effect->GetSortingLayer().orderInLayer = 1;
		auto& settings = effect->GetParticleSettings();

		// 最大数など
		settings.maxParticles = 500;
		settings.emissionRate = 0;
		settings.emissionInterval = 0.01f;
		settings.lifetimeMin = 3.0f;
		settings.lifetimeMax = 6.0f;

		// ライン上に出す（spawnRadiusMin = Max = 0 かつ Shape: Line）
		settings.spawnShape = SpawnParticleShape::Line;
		settings.spawnRadiusMin = 0.0f;
		settings.spawnRadiusMax = HEIGHT; // 幅100pxの線上に生成
		//settings.emissionInterval = 0.5f;
		// 初速で上に飛ばす
		//settings.speedMin = 80.0f;
		//settings.speedMax = 100.0f;

		// 移動方向：上向き（Yマイナス方向）
		settings.velocityMode = VelocityDirectionMode::Cone;
		settings.velocityDirectionDeg = -90.0f;  // 90度
		settings.velocitySpreadDeg = 45.0f;     // 多少ばらける
		// 初期
		settings.startColor = Color(255, 255, 255, 200);
		settings.startRotationMin = { 0, 0, -180 };
		settings.startRotationMax = { 0, 0,  180 };
		// 回転やサイズなど
		settings.sizeMin = 0.05f;
		settings.sizeMax = 0.75f;
		settings.rotationSpeedMin = { 0, 0, -180 };
		settings.rotationSpeedMax = { 0, 0,  180 };
	}
}

void TitleScene::Update() {}

void TitleScene::Draw() {
#if _DEBUG
	DrawString(0, 0, "タイトルシーン", 0x000000);
#endif
}

void TitleScene::Release() {
	Sounds["#!title"];
	Object.DestroySceneObjects();
}