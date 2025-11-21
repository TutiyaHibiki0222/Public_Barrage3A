#include "Project.h"
#include "Scene.h"

//void TitleScene::Init() { 
//
//    CreateUI::CreateUiJsonData(System::IO::Path::Combine(LoadFilePath, "Canvas", "title_ui_data.json"));
//    auto canvasObj = CreateUI::CreateUiJsonData(System::IO::Path::Combine(LoadFilePath, "Canvas", "title_ui_ux_data.json"));
//    // UI制御の初期化.
//    {
//        auto titleUI = canvasObj->AddAppBase<TitleUIManager>();
//        auto add = [&](const std::string& path, const std::string& normalKey, const std::string& selectedKey, bool isNoSelect = false, const std::function<void()> fan = nullptr) {
//            if (auto get = canvasObj->transform->FindByPath(path))
//            {
//                auto image = get->GetGameObject()->GetAppBase<Image>();
//                titleUI->AddPanel(image, normalKey, selectedKey,fan,isNoSelect);
//            }          
//        };
//
//        add("Canvas/GameStart", "UI/Fonts:TextPanelText_1", "UI/Fonts:TextPanelText_0", false, 
//            []() { 
//                SceneManager::GetInstance().SetTransitor<TileTransitor>(60);
//                SceneManager::GetInstance().LoadScene(SceneType::Game);
//            }
//        );
//        add("Canvas/ExtraStart"     , "UI/Fonts:TextPanelText_3", "UI/Fonts:TextPanelText_2"    , true);
//        add("Canvas/Practice"       , "UI/Fonts:TextPanelText_5", "UI/Fonts:TextPanelText_4"    , true);
//        add("Canvas/SpellPractice"  , "UI/Fonts:TextPanelText_7", "UI/Fonts:TextPanelText_6"    , true);
//        add("Canvas/Replay"         , "UI/Fonts:TextPanelText_9", "UI/Fonts:TextPanelText_8"    , true);
//        add("Canvas/PlayerData"     , "UI/Fonts:TextPanelText_11", "UI/Fonts:TextPanelText_10"  , true);
//        add("Canvas/MusicRoom"      , "UI/Fonts:TextPanelText_13", "UI/Fonts:TextPanelText_12"  , true);
//        add("Canvas/Option"         , "UI/Fonts:TextPanelText_15", "UI/Fonts:TextPanelText_14"  , true);
//        add("Canvas/Manual"         , "UI/Fonts:TextPanelText_17", "UI/Fonts:TextPanelText_16"  , true);
//        add("Canvas/Quit"           , "UI/Fonts:TextPanelText_19", "UI/Fonts:TextPanelText_18"  , false,
//            []() {
//                Application::GetInstanse().Quit();
//            }
//        );
//    }
//
//    // 読み込み.
//    Sounds["#?title"]; 
//    // もしBGMを最初に鳴らしたいならここで再生開始
//    auto musicController = GameObject::Instantiate("MusicController")->AddAppBase<MusicController>();
//    if (musicController) {
//        auto bgmClip = Sounds["bgm_title"];
//        musicController->SetMusicClip(bgmClip);
//        musicController->SetLoopSong(true);
//        musicController->SetMusicVolume(0.8f);
//    }
//    // タイトルの演出.
//    {
//        var effectObj = GameObject::Instantiate("Effect");
//        effectObj->transform->rotation = 10;
//        //float offset   = HEIGHT * 0.75f;
//        float CENTER_Y = -HEIGHT / 2.0f;
//        effectObj->transform->position.y = CENTER_Y - 250;
//        var effect = effectObj->AddAppBase<ParticleSystem>();
//        effect->SetSprite(Tex["Effects/Effect:effect000"]);
//        effect->SetRenderMode(RenderMode::ScreenSpaceOverlay);
//        effect->GetSortingLayer().orderInLayer = 1;
//        auto& settings = effect->GetParticleSettings();
//
//        // 最大数など
//        settings.maxParticles = 500;
//        settings.emissionRate = 100;
//        settings.lifetimeMin = 3.0f;
//        settings.lifetimeMax = 6.0f;
//
//        // ライン上に出す（spawnRadiusMin = Max = 0 かつ Shape: Line）
//        settings.spawnShape = SpawnParticleShape::Line;
//        settings.spawnRadiusMin = 0.0f;
//        settings.spawnRadiusMax = HEIGHT; // 幅100pxの線上に生成
//        settings.emissionInterval = 0.5f;
//        // 初速で上に飛ばす
//        //settings.speedMin = 80.0f;
//        //settings.speedMax = 100.0f;
//
//        // 移動方向：上向き（Yマイナス方向）
//        settings.velocityMode = VelocityDirectionMode::Cone;
//        settings.velocityDirectionDeg = -90.0f;  // 90度
//        settings.velocitySpreadDeg    =  45.0f;     // 多少ばらける
//        // 初期
//        settings.startColor = Color(255, 255, 255, 200);
//        settings.startRotationMin = { 0, 0, -180 };
//        settings.startRotationMax = { 0, 0,  180 };
//        // 回転やサイズなど
//        settings.sizeMin = 0.05f;
//        settings.sizeMax = 0.75f;
//        settings.rotationSpeedMin = { 0, 0, -180 };
//        settings.rotationSpeedMax = { 0, 0,  180 };
//    }
//}
//
//void TitleScene::Update()   {  }
//
//void TitleScene::Draw() { 
//#if _DEBUG
//	DrawString(0,0,"タイトルシーン",0x000000); 
//#endif
//}
//
//void TitleScene::Release() { 
//    Sounds["#!title"];
//    Object.DestroySceneObjects(); 
//}
//
//void GameScene::Init()      {
//
//    GameManager::GetInstance().ResetGameState();
//    GameManager::GetInstance().GetScoreManager().LoadHiScore();
//    // UI生成.
//    {
//        auto canvas = CreateUI::CreateUiJsonData(System::IO::Path::Combine(LoadFilePath, "Canvas", "ui_data.json"));
//        for (auto tex : canvas->transform->GetAppBasesInChildren<Text>()) {
//            tex->SetSpriteFont(Texture2DManager::GetInstance().GetSpriteFont("number_main"));
//        }
//        auto hud = canvas->transform->FindByPath("gameUi/Boders");
//
//        HUDManager::GetInstance().Initialize(
//            hud->FindName("HiScore")->GetAppBaseInChildren<Text>(),
//            hud->FindName("Score")->GetAppBaseInChildren<Text>(),
//            hud->FindName("Lifes")->GetAppBasesInChildren<Image>(),
//            hud->FindName("LifeFragmentsText")->GetAppBaseInChildren<Text>(),
//            hud->FindName("Boms")->GetAppBasesInChildren<Image>(),
//            hud->FindName("BomFragmentsText")->GetAppBaseInChildren<Text>(),
//            hud->FindName("reiryoku")->GetAppBaseInChildren<Text>(),
//            hud->FindName("MaxAddScore")->GetAppBaseInChildren<Text>(),
//            hud->FindName("GrazeCount")->GetAppBaseInChildren<Text>(),
//            hud->FindName("TotalGrazeCount")->GetAppBaseInChildren<Text>(),
//            canvas->transform->FindName("fpsText")->GetAppBaseInChildren<Text>()
//        );
//    }
//
//    EnemyManager::GetInstance().Init();
//    EnemyManager::GetInstance().LoadCSV(LoadStage1CSV.string());
//
//    Sounds["#?stage1"];
//
//    auto musicController = GameObject::Instantiate("MusicController")->AddAppBase<MusicController>();
//    if (musicController) {
//        auto bgmClip    = Sounds["bgm_stage1"];
//        musicController->SetMusicClip(bgmClip);
//        musicController->SetLoopSong(true);
//        musicController->SetMusicVolume(0.8f);
//    }
//
//    {
//        var obj = GameObject::Instantiate("Back");
//        var mgr = obj->AddAppBase<Pseudo3DBackgroundManager>();
//		mgr->SetBackground(std::make_shared<Stage3>());
//
//        var ptObj = GameObject::Instantiate("Obj");
//        obj->transform->position.x = -150;
//        var pt = ptObj->AddAppBase<ParticleSystem>();
//        pt->SetSprite(Sprite::MakeLoadImage("Resources/Images/Effects/momize.png"));
//        pt->GetSortingLayer().layer = -10;
//
//        ParticleSettings& mapleLeafSettings = pt->GetParticleSettings();
//        mapleLeafSettings.maxParticles = 150;
//        mapleLeafSettings.emissionRate = 3.0f;
//        mapleLeafSettings.emissionInterval = 1.0f / mapleLeafSettings.emissionRate;
//        mapleLeafSettings.emissionDuration = -1.0f; // 無限
//        mapleLeafSettings.burstCount = 0;
//
//        mapleLeafSettings.spawnRadiusMin = 0.0f;
//        mapleLeafSettings.spawnRadiusMax = 150.0f; // 横にかなり広く
//
//        mapleLeafSettings.lifetimeMin = 6.0f;
//        mapleLeafSettings.lifetimeMax = 12.0f;
//
//        mapleLeafSettings.speedMin = 80.0f;
//        mapleLeafSettings.speedMax = 180.0f;
//
//        mapleLeafSettings.sizeMin = 0.7f;
//        mapleLeafSettings.sizeMax = 1.5f;
//
//        mapleLeafSettings.startRotationMin = { 0, 0, 0 };
//        mapleLeafSettings.startRotationMax = { 0, 0, 360 };
//
//        mapleLeafSettings.rotationSpeedMin = { 0, 0, -240 };
//        mapleLeafSettings.rotationSpeedMax = { 0, 0,  240 };
//
//        mapleLeafSettings.startColor = { 255, 150, 50, 255 };
//        mapleLeafSettings.useRandomStartColor = true;
//        mapleLeafSettings.startColorMin = { 255, 120, 0, 200 };  // オレンジ寄りの黄色
//        mapleLeafSettings.startColorMax = { 255, 40, 0, 255 };   // 濃い赤
//
//        mapleLeafSettings.spawnShape = SpawnParticleShape::Box;
//        mapleLeafSettings.spawnBoxMin = {-450.0f, -400.0f }; // 画面上部中央から左右に広げる
//        mapleLeafSettings.spawnBoxMax = { 100.0f,  100.0f }; // 縦は薄く
//
//        mapleLeafSettings.velocityMode = VelocityDirectionMode::Cone;
//        mapleLeafSettings.velocityDirectionDeg = 75.0f;  // 斜め右下（基準）
//        mapleLeafSettings.velocitySpreadDeg = 30.0f;     // ±15度程度の広がり
//
//        mapleLeafSettings.customUpdate = [](Particle& p, Vector2D worldPos, Vector2D worldScale) {
//            float dt = Time.deltaTime;
//
//            // 斜めに一定速度で落下（速度は固定）
//            p.pos.x += p.vc.x * dt;
//            p.pos.y += p.vc.y * dt;
//
//            // ここで「位置の揺れ」を加える（速度は変えない）
//            // 揺れの振幅と周期
//            float swayAmplitude = 20.0f;  // 揺れの幅（ピクセル）
//            float swayFrequency = 2.0f;   // 揺れの速さ（Hz）
//
//            // 乱数的位相は p.angleDirection で持っている想定
//            float swayX = swayAmplitude * std::sin(swayFrequency * (p.totalLife - p.life) + p.angleDirection);
//            float swayY = swayAmplitude * 0.5f * std::sin(swayFrequency * 1.5f * (p.totalLife - p.life) + p.angleDirection + 1.0f);
//
//            // 位置に揺れを加算（速度は変えないので軌跡は斜め落下のまま）
//            p.pos.x += swayX * dt;
//            p.pos.y += swayY * dt;
//
//            // 回転は基本回転速度のみでゆっくり回転
//            p.rot.z += p.rotSpeed.z * dt;
//
//            // 経過割合
//            float age = p.totalLife - p.life;
//            float t = age / p.totalLife;
//
//            // サイズはほぼ一定、終盤に少し縮小
//            p.sizeCurrent = p.size * (1.0f - 0.1f * t);
//
//            // アルファフェード（フェードイン＆アウト）
//            float fadeTime = 1.0f;
//            float alphaFactor = 1.0f;
//            if (age < fadeTime) alphaFactor = age / fadeTime;
//            else if (p.life < fadeTime) alphaFactor = p.life / fadeTime;
//
//            alphaFactor = std::clamp(alphaFactor, 0.0f, 1.0f);
//            p.color.a = p.startColor.a * alphaFactor;
//            };
//    }
//
//    {
//        var player = GameObject::Instantiate("Player");
//        player->transform->scale = { 1.25f,1.25f };
//        player->SetTag("Player");
//        player->AddAppBase<Player>();
//        player->transform->position = Vector2D(-150, -300);
//        auto sp = player->AddAppBase<SpriteRenderer>();
//        player->SetLayer(Layer::Player);
//
//        GameManager::GetInstance().SetPlayer(player);
//
//        auto p = Texture2DManager::GetInstance().GetTexture2D("Player", "霊夢");
//        auto animator = player->AddAppBase<Animator>();
//        animator->SetSpeed(2);
//        // パラメータ追加
//        animator->AddParameter("isL", AnimatorDef::AnimatorParameterType::Bool);
//        animator->AddParameter("isR", AnimatorDef::AnimatorParameterType::Bool);
//        // Clip 作成
//        auto clip1 = std::make_shared<AnimatorDef::AnimationClip>();
//        clip1->duration = 1.0f;
//        clip1->frames = (p->GetTexturesByPrefixRange("player01", 0, 3));
//        clip1->loop = true;
//
//        auto clip2 = std::make_shared<AnimatorDef::AnimationClip>();
//        clip2->duration = 1.0f;
//        clip2->frames = (p->GetTexturesByPrefixRange("player01", 4, 7));
//        clip2->loop = true;
//
//        auto clip3 = std::make_shared<AnimatorDef::AnimationClip>();
//        clip3->duration = 1.0f;
//        clip3->frames = (p->GetTexturesByPrefixRange("player01", 8, 11));
//        clip3->loop = true;
//
//        animator->AddParameter("isFacing", AnimatorDef::AnimatorParameterType::Bool);
//        // ステート作成
//        AnimatorDef::AnimatorState state1{
//            "State1", clip1, {},
//            []() { std::cout << "State1 Entered!" << std::endl; },
//            []() { std::cout << "State1 Exited!" << std::endl; }
//        };
//
//        AnimatorDef::AnimatorState state2{
//            "StateLeft", clip3, {},
//            []() { std::cout << "State2 Entered!" << std::endl; },
//            []() { std::cout << "State2 Exited!" << std::endl; }
//        };
//
//        AnimatorDef::AnimatorState state3{
//            "StateRight", clip2, {},
//            []() { std::cout << "State3 Entered!" << std::endl; },
//            []() { std::cout << "State3 Exited!" << std::endl; }
//        };
//        // State1（たとえばIdleなど）からLeftへ遷移
//        AnimatorDef::AnimatorTransition toLeft;
//        toLeft.toStateName = "StateLeft";
//        toLeft.AddBoolCondition("isL", true);
//        // State1からRightへ遷移
//        AnimatorDef::AnimatorTransition toRight;
//        toRight.toStateName = "StateRight";
//        toRight.AddBoolCondition("isR", true);
//        // StateLeftやStateRightから元のState1に戻る遷移も作ると良いです
//        AnimatorDef::AnimatorTransition backToIdleFromLeft;
//        backToIdleFromLeft.toStateName = "State1";
//        backToIdleFromLeft.AddBoolCondition("isL", false);
//
//        AnimatorDef::AnimatorTransition backToIdleFromRight;
//        backToIdleFromRight.toStateName = "State1";
//        backToIdleFromRight.AddBoolCondition("isR", false);
//
//        // 遷移追加
//        state1.transitions.push_back(toLeft);
//        state1.transitions.push_back(toRight);
//
//        state2.transitions.push_back(backToIdleFromLeft);
//        state3.transitions.push_back(backToIdleFromRight);
//
//        // 最後にステート登録
//        animator->AddState(state2);
//        animator->AddState(state3);
//        animator->AddState(state1);
//        // その他.
//        {
//            auto slowImg = Tex("Player/Slow/霊夢");
//            var show1   = GameObject::Instantiate("Border");
//            var show2   = GameObject::Instantiate("Border");
//            var dot     = GameObject::Instantiate("Dot");
//            var graze   = GameObject::Instantiate("Graze");
//            {
//                var sp1 = show1->AddAppBase<SpriteRenderer>();
//                sp1->SetLayer(2);
//                sp1->SetSprite(slowImg->GetTextureData("Border"));
//                var s = show1->AddAppBase<ShiftAura>();
//                s->SetAlphaRange(0, 0.5f);
//                s->SetRotation(0, 200.0f, 270);
//                s->SetScaleRange(0.2f, 1);
//                s->SetAppearTime(0.3f);
//            }
//
//            {
//                var sp2 = show2->AddAppBase<SpriteRenderer>();
//                sp2->SetLayer(2);
//                sp2->SetSprite(slowImg->GetTextureData("Border"));
//                var s = show2->AddAppBase<ShiftAura>();
//                s->SetAlphaRange(0, 0.5f);
//                s->SetRotation(0, -75.0f, -270);
//                s->SetScaleRange(0.2f, 1);
//                s->SetAppearTime(0.3f);
//            }
//
//            {
//                var sp3 = dot->AddAppBase<SpriteRenderer>();
//                var sp = slowImg->GetTextureData("Dot");
//                sp3->SetLayer(5);
//                sp3->SetSprite(sp);
//                var s = dot->AddAppBase<ShiftAura>();
//                s->SetAlphaRange(0, 180);
//                s->SetAppearTime(0.3f);
//            }
//            {
//                graze->SetLayer(Layer::Graze);
//                graze->SetTag("Graze");
//                graze->AddAppBase<CircleCollider>()->SetRadius((float)(slowImg->GetTextureData("Border")->width / 2));
//            }
//            show1->transform->SetParent(player->transform, false);
//            show2->transform->SetParent(player->transform, false);
//            dot->transform->SetParent(player->transform, false);
//            graze->transform->SetParent(player->transform, false);
//        }
//    }
//    // テスト.
//    {
//        // Prefab.
//        if(!PrefabMgr.HasPrefab("Enemy01"))
//        {
//            var prefab = PrefabMgr.CreatePrefab("Enemy01");
//            prefab->SetTag("Enemy");
//            prefab->SetLayer(Layer::Enemy);
//            prefab->AddAppBase<BoxCollider>()->SetSize({ 8,8 });
//            auto sp = prefab->AddAppBase<SpriteRenderer>();
//            sp->SetSprite(Tex["Enemys/Mini:Enemy_1"]);
//            sp->SetLayer(3);
//            
//            // 通常向き.
//            auto clipIdle = std::make_shared<AnimatorDef::AnimationClip>();
//            clipIdle->duration = 1.0f;
//            clipIdle->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRangePingPong("Enemy", 0, 4);
//            clipIdle->loop = true;
//
//            auto clipTurnRight = std::make_shared<AnimatorDef::AnimationClip>();
//            clipTurnRight->duration = 0.4f;
//            clipTurnRight->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRange("Enemy", 4, 7);
//            clipTurnRight->loop = false;
//
//            auto clipTurnReturn = std::make_shared<AnimatorDef::AnimationClip>();
//            clipTurnReturn->duration = 0.4f;
//            clipTurnReturn->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRange("Enemy", 7, 4);
//            clipTurnReturn->loop = false;
//
//            auto clipMoveRight = std::make_shared<AnimatorDef::AnimationClip>();
//            clipMoveRight->duration = 1.0f;
//            clipMoveRight->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRangePingPong("Enemy", 8, 11);
//            clipMoveRight->loop = true;  // 移動はループ想定
//
//            // ステート定義
//            AnimatorDef::AnimatorState idleState{ "Idle", clipIdle};
//
//            AnimatorDef::AnimatorState turnEffectState{"TurnEffect", clipTurnRight, {},};
//
//            AnimatorDef::AnimatorState moveState{"Move", clipMoveRight, {},};
//
//            AnimatorDef::AnimatorState turnReturnState{"TurnReturn", clipTurnReturn, {}};
//
//            // 遷移作成
//            // Idle → TurnEffect（isMoving==true）
//            AnimatorDef::AnimatorTransition idleToTurnEffect;
//            idleToTurnEffect.toStateName = "TurnEffect";
//            idleToTurnEffect.AddBoolCondition("isMoving", true);
//
//            // TurnEffect → Move（クリップ終了時に遷移）
//            AnimatorDef::AnimatorTransition turnEffectToMove;
//            turnEffectToMove.toStateName = "Move";
//            turnEffectToMove.hasExitTime = true; // クリップ終了で遷移
//
//            // Move → TurnReturn（isMoving==false）
//            AnimatorDef::AnimatorTransition moveToTurnReturn;
//            moveToTurnReturn.toStateName = "TurnReturn";
//            moveToTurnReturn.AddBoolCondition("isMoving", false);
//
//            // TurnReturn → Idle（クリップ終了時に遷移）
//            AnimatorDef::AnimatorTransition turnReturnToIdle;
//            turnReturnToIdle.toStateName = "Idle";
//            turnReturnToIdle.hasExitTime = true;
//
//            // 各ステートに遷移を追加
//            idleState.transitions.push_back(idleToTurnEffect);
//            turnEffectState.transitions.push_back(turnEffectToMove);
//            moveState.transitions.push_back(moveToTurnReturn);
//            turnReturnState.transitions.push_back(turnReturnToIdle);
//
//            // Animatorに登録
//            auto animator = prefab->AddAppBase<Animator>();
//            animator->AddState(idleState);
//            animator->AddState(turnEffectState);
//            animator->AddState(moveState);
//            animator->AddState(turnReturnState);
//
//            // パラメータ追加
//            animator->AddParameter("isMoving", AnimatorDef::AnimatorParameterType::Bool);
//
//
//        }
//        if (!PrefabMgr.HasPrefab("Enemy02"))
//        {
//            var prefab = PrefabMgr.CreatePrefab("Enemy02");
//            prefab->SetTag("Enemy");
//            prefab->SetLayer(Layer::Enemy);
//            prefab->AddAppBase<BoxCollider>()->SetSize({ 8,8 });
//            auto sp = prefab->AddAppBase<SpriteRenderer>();
//            sp->SetSprite(Tex["Enemys/Mini:Enemy_2"]);
//            sp->SetLayer(3);
//            // 通常向き.
//            auto clipIdle = std::make_shared<AnimatorDef::AnimationClip>();
//            clipIdle->duration = 1.0f;
//            clipIdle->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRangePingPong("Enemy", 12, 15);
//            clipIdle->loop = true;
//
//            auto clipTurnRight = std::make_shared<AnimatorDef::AnimationClip>();
//            clipTurnRight->duration = 0.4f;
//            clipTurnRight->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRange("Enemy", 16, 19);
//            clipTurnRight->loop = false;
//
//            auto clipTurnReturn = std::make_shared<AnimatorDef::AnimationClip>();
//            clipTurnReturn->duration = 0.4f;
//            clipTurnReturn->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRange("Enemy", 19, 16);
//            clipTurnReturn->loop = false;
//
//            auto clipMoveRight = std::make_shared<AnimatorDef::AnimationClip>();
//            clipMoveRight->duration = 1.0f;
//            clipMoveRight->frames = Tex("Enemys/Mini")->GetTexturesByPrefixRangePingPong("Enemy", 20, 23);
//            clipMoveRight->loop = true;  // 移動はループ想定
//
//            // ステート定義
//            AnimatorDef::AnimatorState idleState{ "Idle", clipIdle };
//
//            AnimatorDef::AnimatorState turnEffectState{ "TurnEffect", clipTurnRight, {}, };
//
//            AnimatorDef::AnimatorState moveState{ "Move", clipMoveRight, {}, };
//
//            AnimatorDef::AnimatorState turnReturnState{ "TurnReturn", clipTurnReturn, {} };
//
//            // 遷移作成
//            // Idle → TurnEffect（isMoving==true）
//            AnimatorDef::AnimatorTransition idleToTurnEffect;
//            idleToTurnEffect.toStateName = "TurnEffect";
//            idleToTurnEffect.AddBoolCondition("isMoving", true);
//
//            // TurnEffect → Move（クリップ終了時に遷移）
//            AnimatorDef::AnimatorTransition turnEffectToMove;
//            turnEffectToMove.toStateName = "Move";
//            turnEffectToMove.hasExitTime = true; // クリップ終了で遷移
//
//            // Move → TurnReturn（isMoving==false）
//            AnimatorDef::AnimatorTransition moveToTurnReturn;
//            moveToTurnReturn.toStateName = "TurnReturn";
//            moveToTurnReturn.AddBoolCondition("isMoving", false);
//
//            // TurnReturn → Idle（クリップ終了時に遷移）
//            AnimatorDef::AnimatorTransition turnReturnToIdle;
//            turnReturnToIdle.toStateName = "Idle";
//            turnReturnToIdle.hasExitTime = true;
//
//            // 各ステートに遷移を追加
//            idleState.transitions.push_back(idleToTurnEffect);
//            turnEffectState.transitions.push_back(turnEffectToMove);
//            moveState.transitions.push_back(moveToTurnReturn);
//            turnReturnState.transitions.push_back(turnReturnToIdle);
//
//            // Animatorに登録
//            auto animator = prefab->AddAppBase<Animator>();
//            animator->AddState(idleState);
//            animator->AddState(turnEffectState);
//            animator->AddState(moveState);
//            animator->AddState(turnReturnState);
//
//            // パラメータ追加
//            animator->AddParameter("isMoving", AnimatorDef::AnimatorParameterType::Bool);
//        }
//        if (!PrefabMgr.HasPrefab("EliteEnemy01"))
//        {
//            var prefab = PrefabMgr.CreatePrefab("EliteEnemy01");
//            prefab->SetTag("Enemy");
//            prefab->SetLayer(Layer::Enemy);
//            prefab->AddAppBase<CircleCollider>()->SetRadius(16);
//            auto sp = prefab->AddAppBase<SpriteRenderer>();
//            sp->SetSprite(Tex["Enemys/Elite:EliteEnemy01_2"]);
//            sp->SetLayer(3);
//            // 通常向き.
//            auto clipIdle = std::make_shared<AnimatorDef::AnimationClip>();
//            clipIdle->duration = 1.0f;
//            clipIdle->frames = Tex("Enemys/Elite")->GetTexturesByPrefixRangePingPong("EliteEnemy01", 0, 3);
//            clipIdle->loop = true;
//
//            auto clipTurnRight = std::make_shared<AnimatorDef::AnimationClip>();
//            clipTurnRight->duration = 0.4f;
//            clipTurnRight->frames = Tex("Enemys/Elite")->GetTexturesByPrefixRange("EliteEnemy01", 4, 7);
//            clipTurnRight->loop = false;
//
//            auto clipTurnReturn = std::make_shared<AnimatorDef::AnimationClip>();
//            clipTurnReturn->duration = 0.4f;
//            clipTurnReturn->frames = Tex("Enemys/Elite")->GetTexturesByPrefixRange("EliteEnemy01", 7, 4);
//            clipTurnReturn->loop = false;
//
//            auto clipMoveRight = std::make_shared<AnimatorDef::AnimationClip>();
//            clipMoveRight->duration = 1.0f;
//            clipMoveRight->frames = Tex("Enemys/Elite")->GetTexturesByPrefixRangePingPong("Enemy", 20, 23);
//            clipMoveRight->loop = true;  // 移動はループ想定
//
//            // ステート定義
//            AnimatorDef::AnimatorState idleState{ "Idle", clipIdle };
//
//            AnimatorDef::AnimatorState turnEffectState{ "TurnEffect", clipTurnRight, {}, };
//
//            AnimatorDef::AnimatorState moveState{ "Move", clipMoveRight, {}, };
//
//            AnimatorDef::AnimatorState turnReturnState{ "TurnReturn", clipTurnReturn, {} };
//
//            // 遷移作成
//            // Idle → TurnEffect（isMoving==true）
//            AnimatorDef::AnimatorTransition idleToTurnEffect;
//            idleToTurnEffect.toStateName = "TurnEffect";
//            idleToTurnEffect.AddBoolCondition("isMoving", true);
//
//            // TurnEffect → Move（クリップ終了時に遷移）
//            AnimatorDef::AnimatorTransition turnEffectToMove;
//            turnEffectToMove.toStateName = "Move";
//            turnEffectToMove.hasExitTime = true; // クリップ終了で遷移
//
//            // Move → TurnReturn（isMoving==false）
//            AnimatorDef::AnimatorTransition moveToTurnReturn;
//            moveToTurnReturn.toStateName = "TurnReturn";
//            moveToTurnReturn.AddBoolCondition("isMoving", false);
//
//            // TurnReturn → Idle（クリップ終了時に遷移）
//            AnimatorDef::AnimatorTransition turnReturnToIdle;
//            turnReturnToIdle.toStateName = "Idle";
//            turnReturnToIdle.hasExitTime = true;
//
//            // 各ステートに遷移を追加
//            idleState.transitions.push_back(idleToTurnEffect);
//            turnEffectState.transitions.push_back(turnEffectToMove);
//            moveState.transitions.push_back(moveToTurnReturn);
//            turnReturnState.transitions.push_back(turnReturnToIdle);
//
//            // Animatorに登録
//            auto animator = prefab->AddAppBase<Animator>();
//            animator->AddState(idleState);
//            animator->AddState(turnEffectState);
//            animator->AddState(moveState);
//            animator->AddState(turnReturnState);
//
//            // パラメータ追加
//            animator->AddParameter("isMoving", AnimatorDef::AnimatorParameterType::Bool);
//        }
//    }
//    {
//       auto boss = GameObject::Instantiate("boss");
//	   boss->transform->position = Vector2D(-300, 300);
//       boss->AddAppBase<BarrelDistort>();
//       auto sp = boss->AddAppBase<Sprite3DRenderer>();
//       sp->SetLayer(-6);
//       sp->SetSprite(Sprite::MakeLoadImage("Resources/Images/Enemy/Boss/BossMahouJin.png"));
//	   sp->SetRotation({ 0, 50, 180 });
//	   boss->AddAppBase<BossAuraController>()->SetAuraRenderer(sp);
//    }
//}
//
//System::Coroutine Change() {
//    _yield new System::WaitForFrames(30);
//    SceneManager::GetInstance().SetTransitor<TileTransitor>(30);
//    SceneManager::GetInstance().LoadScene(SceneType::Title);
//}
//
//void GameScene::Update()    
//{  
//    EnemyManager::GetInstance().Update();
//    ItemManager::GetInstance().Update();
//
//    HUDManager::GetInstance().Update();
//
//    if (GameManager::GetInstance().IsGameOver() && !isChangeScene) {
//		isChangeScene = true;
//		System::StartCoroutine(Change());
//    }
//}
//
//void GameScene::Draw()      { 
//#if _DEBUG
//	DrawString(0, 0, "ゲームシーン", 0xFFFFFF); 
//#endif
//}
//
//void GameScene::Release() {
//    Object.DestroySceneObjects(); 
//    ItemManager::GetInstance().Clear();
//    HUDManager::GetInstance().Reset();
//    Sounds["#!stage1"];
//}
