// Tmp.cpp
#include "Tmp.h"
#include "Dx3DCamera.h"
#include "Define.h"
#include "Random.h"
#include "GameObject.h"
#include "ParticleSystem.h"
#include "Texture2D.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"

constexpr float WIDX = 60.0f;
constexpr float WIDY = 60.0f;
constexpr int START_N = -1;
constexpr int END_N = 5;

constexpr int wCount = 3;  // 横・縦に敷く個数
constexpr float waterUnitW = WIDX / wCount;
constexpr float waterUnitH = WIDY / wCount;

static const float csDefRotaX = 17.5f * (Define::PI / 180.0f);

Tmp::~Tmp() {
    DeleteGraph(backgrounds.front()->GetTextureHandle());
    DeleteGraph(waters.front()->GetTextureHandle());
    DeleteGraph(grounds.front()->GetTextureHandle());
    DeleteGraph(tops.front()->GetTextureHandle());
	Sounds["#!Tmp"];
}

void Tmp::Init()
{
    backgrounds.clear();
    waters.clear();
    grounds.clear();
    tops.clear();

    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    auto createUnit = [&](int tex, const VECTOR& pos, const VECTOR& scale, const VECTOR& velocity, int alpha = 255, float w = WIDX, float h = WIDY, bool flipX = false) {
        auto u = std::make_shared<StageUnit>();
        u->SetTextureHandle(tex);
        u->SetPosition(pos);
        u->SetScale(scale);
        u->SetVerticesAsBox(w, h, GetColorU8(255, 255, 255, alpha));
        u->SetDrawRotateX(csDefRotaX);
        u->SetDrawMove(VGet(0, 0, -10));
        u->SetVelocity(velocity);
        if (flipX) u->SetScale(VGet(-scale.x, scale.y, scale.z));
        return u;
        };

    // Load textures
    int texBack		= LoadGraph("Resources/Images/Stage/3D/Tmp/background.png");
    int texWater	= LoadGraph("Resources/Images/Stage/3D/Tmp/water.png");
    int texGroundL	= LoadGraph("Resources/Images/Stage/3D/Tmp/ground_L.png");
    int texGroundR	= LoadGraph("Resources/Images/Stage/3D/Tmp/ground_R.png");
    int texTop		= LoadGraph("Resources/Images/Stage/3D/Tmp/top.png");

    for (int i = START_N; i < END_N; ++i) {
        float y = WIDY * i;
        // Far background
        backgrounds.push_back(createUnit(texBack, VGet(0, y, 20), VGet(1, 1, 1), VGet(0, -0.03f, 0), 255,85,85));

        // Water (semi-transparent): two types
        for (int x = -2; x <= 2; ++x) {
            for (int wy = 0; wy < wCount; ++wy) {
                for (int wx = 0; wx < wCount; ++wx) {
                    float offsetX = x * WIDX + wx * waterUnitW;
                    float offsetY = y + wy * waterUnitH;

                    auto w1 = createUnit(texWater, VGet(offsetX, offsetY, 18), VGet(1, 1, 1), VGet(0.02f, -0.08f, 0), 255, waterUnitW, waterUnitH);
                    w1->SetColor({ 255,255,255,70 });
                    waters.push_back(w1);
                    auto w2 = createUnit(texWater, VGet(offsetX, offsetY, 18), VGet(-1, -1, 1), VGet(-0.01f, -0.04f, 0), 255, waterUnitW, waterUnitH);
                    w2->SetColor({ 255,255,255,70 });
                    waters.push_back(w2);
                }
            }
        }
        // Ground
        grounds.push_back(createUnit(texGroundL, VGet(-20, y, 0), VGet(1, -1, 1), VGet(0, -0.05f, 0), 255, 30, 60));
        grounds.push_back(createUnit(texGroundR, VGet( 20, y, 0), VGet(1, -1, 1), VGet(0, -0.05f, 0), 255, 30, 60));

        float zOffsetLeft   = Random.Range(-2.0f, 2.0f);  // 奥行きズレ
        float zOffsetRight = Random.Range(-2.0f, 2.0f);

        tops.push_back(createUnit(texTop, VGet(-10, y + zOffsetLeft, -10 + zOffsetLeft), VGet(1, 1, 1), VGet(0, -0.07f, 0), 255, 30, 50));
        tops.push_back(createUnit(texTop, VGet( 10, y + zOffsetRight, -10 + zOffsetRight), VGet(1, -1, 1), VGet(0, -0.07f, 0), 255, 30, 50, true));

    }

    frame = 0;

    // Material / Fog setup
    MATERIALPARAM M{};
    M.Diffuse = GetColorF(0, 0, 0, 1);
    M.Specular = GetColorF(0, 0, 0, 0);
    M.Ambient = GetColorF(1, 1, 1, 1);
    M.Emissive = GetColorF(1, 1, 1, 0);
    M.Power = 20.f;
    SetMaterialParam(M);
    SetMaterialUseVertSpcColor(FALSE);
    SetMaterialUseVertDifColor(TRUE);

    auto& cam = Dx3DCamera::Instance();
    cam.SetFogEnable(true);
    cam.SetFogColor(255, 100, 100);
    // Initial fog range (will override per layer in Draw)
    cam.SetFogRange(65.0f, 90.0f);

    // Fog設定.
    script.Add(FogInstruction::SetColor({ 0xFF0000}));
    script.Add(FogInstruction::SetRange(20,60));
    script.Add(FogInstruction::Wait(300));
    script.Add(FogInstruction::LerpColorAndRange({ 0xFF0000 }, { 0x000000 }, 20, 60,      10,    11,  480));
    script.Add(FogInstruction::Wait(30));
    script.Add(FogInstruction::LerpRange(10, 11,70.0f, 94.0f, 380));
    script.Add(FogInstruction::Wait(30));
    script.Add(FogInstruction::LerpColorAndRange({ 0x000000 }, { 255,75,75 }, 70.0f, 94.0f, 68.0f, 94.0f, 380));
	// ステージ演出(effect)
	{
		var ptObj = GameObject::Instantiate("Obj");
		var pt = ptObj->AddAppBase<ParticleSystem>();
		pt->SetSprite(Sprite::MakeLoadImage("Resources/Images/Effects/momize.png"));
		pt->GetSortingLayer().layer = -10;

		ParticleSettings& mapleLeafSettings = pt->GetParticleSettings();
		mapleLeafSettings.maxParticles = 150;
		mapleLeafSettings.emissionRate = 3.0f;
		mapleLeafSettings.emissionInterval = 1.0f / mapleLeafSettings.emissionRate;
		mapleLeafSettings.emissionDuration = -1.0f; // 無限
		mapleLeafSettings.burstCount = 0;

		mapleLeafSettings.spawnRadiusMin = 0.0f;
		mapleLeafSettings.spawnRadiusMax = 150.0f; // 横にかなり広く

		mapleLeafSettings.lifetimeMin = 6.0f;
		mapleLeafSettings.lifetimeMax = 12.0f;

		mapleLeafSettings.speedMin = 80.0f;
		mapleLeafSettings.speedMax = 180.0f;

		mapleLeafSettings.sizeMin = 0.7f;
		mapleLeafSettings.sizeMax = 1.5f;

		mapleLeafSettings.startRotationMin = { 0, 0, 0 };
		mapleLeafSettings.startRotationMax = { 0, 0, 360 };

		mapleLeafSettings.rotationSpeedMin = { 0, 0, -240 };
		mapleLeafSettings.rotationSpeedMax = { 0, 0,  240 };

		mapleLeafSettings.startColor = { 255, 150, 50, 255 };
		mapleLeafSettings.useRandomStartColor = true;
		mapleLeafSettings.startColorMin = { 255, 120, 0, 200 };  // オレンジ寄りの黄色
		mapleLeafSettings.startColorMax = { 255, 40, 0, 255 };   // 濃い赤

		mapleLeafSettings.spawnShape = SpawnParticleShape::Box;
		mapleLeafSettings.spawnBoxMin = { -450.0f, -400.0f }; // 画面上部中央から左右に広げる
		mapleLeafSettings.spawnBoxMax = { 100.0f,  100.0f }; // 縦は薄く

		mapleLeafSettings.velocityMode = VelocityDirectionMode::Cone;
		mapleLeafSettings.velocityDirectionDeg = 75.0f;  // 斜め右下（基準）
		mapleLeafSettings.velocitySpreadDeg = 30.0f;     // ±15度程度の広がり

		mapleLeafSettings.customUpdate = [](Particle& p, Vector2D worldPos, Vector2D worldScale) {
			float dt = Time.deltaTime;

			// 斜めに一定速度で落下（速度は固定）
			p.pos.x += p.vc.x * dt;
			p.pos.y += p.vc.y * dt;

			// ここで「位置の揺れ」を加える（速度は変えない）
			// 揺れの振幅と周期
			float swayAmplitude = 20.0f;  // 揺れの幅（ピクセル）
			float swayFrequency = 2.0f;   // 揺れの速さ（Hz）

			// 乱数的位相は p.angleDirection で持っている想定
			float swayX = swayAmplitude * std::sin(swayFrequency * (p.totalLife - p.life) + p.angleDirection);
			float swayY = swayAmplitude * 0.5f * std::sin(swayFrequency * 1.5f * (p.totalLife - p.life) + p.angleDirection + 1.0f);

			// 位置に揺れを加算（速度は変えないので軌跡は斜め落下のまま）
			p.pos.x += swayX * dt;
			p.pos.y += swayY * dt;

			// 回転は基本回転速度のみでゆっくり回転
			p.rot.z += p.rotSpeed.z * dt;

			// 経過割合
			float age = p.totalLife - p.life;
			float t = age / p.totalLife;

			// サイズはほぼ一定、終盤に少し縮小
			p.sizeCurrent = p.size * (1.0f - 0.1f * t);

			// アルファフェード（フェードイン＆アウト）
			float fadeTime = 1.0f;
			float alphaFactor = 1.0f;
			if (age < fadeTime) alphaFactor = age / fadeTime;
			else if (p.life < fadeTime) alphaFactor = p.life / fadeTime;

			alphaFactor = std::clamp(alphaFactor, 0.0f, 1.0f);
			p.color.a = p.startColor.a * alphaFactor;
			};
	}
	// 音楽.
	{
		Sounds["#?Tmp"];
		// もしBGMを最初に鳴らしたいならここで再生開始
		auto musicController = GameObject::Instantiate("MusicController")->AddAppBase<MusicController>();
		if (musicController) {
			auto bgmClip = Sounds["bgm_tmp"];
			musicController->SetMusicClip(bgmClip);
			musicController->SetLoopSong(true);
			musicController->SetMusicVolume(0.8f);
		}
	}
}

float LoopPosition(float pos, float start, float length) {
    float relative = pos - start;
    while (relative < 0) relative += length;
    while (relative >= length) relative -= length;
    return start + relative;
}


void Tmp::Update()
{
    // === 移動速度制御: Fogスクリプトと同期っぽく見えるように ===
    fallPhaseFrame++;

    if (fallPhaseFrame < 300) {
        // 初期高速
        moveSpeedGround = -1.2f;
        moveSpeedTop = -1.2f;
    }
    else if (fallPhaseFrame < 300 + 480) {
        // ゆっくり停止に向かう補間
        float t = (fallPhaseFrame - 300) / 480.0f;
        moveSpeedGround = -1.2f * (1.0f - t); // 線形補間（Easeに変えてもOK）
        moveSpeedTop = -1.2f * (1.0f - t);
    }
    else if (fallPhaseFrame < 300 + 480 + 30) {
        // 停止中
        moveSpeedGround = 0.0f;
        moveSpeedTop = 0.0f;
    }
    else if (fallPhaseFrame < 300 + 480 + 30 + 60) {
        // ゆっくり元の速度に戻る（0 → -0.05 / -0.07）
        float t = (fallPhaseFrame - (300 + 480 + 30)) / 60.0f;
        moveSpeedGround = -0.05f * t;
        moveSpeedTop = -0.07f * t;
    }
    else {
        // 通常速度維持
        moveSpeedGround = -0.05f;
        moveSpeedTop = -0.07f;
    }

    // === 背景・地面ループ更新 ===
    auto loopList = [&](std::list<std::shared_ptr<StageUnit>>& list, float moveYSpeed) {
        for (auto& u : list) {
            u->SetVelocity(VGet(0, moveYSpeed, 0));
            u->Update(1.0f);
            auto p = u->GetPosition();
            if (p.y < WIDY * (START_N - 1)) {
                p.y += WIDY * (END_N - START_N);
                u->SetPosition(p);
            }
        }
        };

    loopList(backgrounds, -0.03f);          // 背景: 固定速度
    loopList(grounds, moveSpeedGround);     // 地面: 可変

    // Top は特殊処理あり（Z再設定）
    for (auto& u : tops) {
        u->SetVelocity(VGet(0, moveSpeedTop, 0));
        u->Update(1.0f);
        auto p = u->GetPosition();
        if (p.y < WIDY * (START_N - 1)) {
            p.y += WIDY * (END_N - START_N);

            // Z を -10 ± [-2 ~ +5] に更新
            float zOffset = Random.Range(-10.0f, 7.0f);
            p.z = -10.0f + zOffset;

            u->SetPosition(p);
        }
    }

    // 水面：ループ位置制御
    float loopStartX = -2 * WIDX;
    float loopEndX = 2 * WIDX + (wCount - 1) * waterUnitW;
    float loopWidth = loopEndX - loopStartX + waterUnitW;

    float loopStartY = waterUnitH * START_N * wCount;
    float loopHeight = waterUnitH * wCount * (END_N - START_N);

    for (auto& u : waters) {
        u->Update(1.0f);
        auto p = u->GetPosition();

        p.x = LoopPosition(p.x, loopStartX, loopWidth);
        p.y = LoopPosition(p.y, loopStartY, loopHeight);

        u->SetPosition(p);
    }

    // フォグスクリプト更新
    if (!script.IsFinished())
        script.Update();
    else
        frame++;
}


void Tmp::Draw()
{
    SetDrawMode(DX_DRAWMODE_BILINEAR);
    auto& cam = Dx3DCamera::Instance();

    float rotateY = cosf(frame * 0.001f) * 0.1f;

    float distance = 65.0f;
    float eyeX = sinf(rotateY) * distance;
    float eyeY = 0;
    float eyeZ = -cosf(rotateY) * distance;

    VECTOR eye = VGet(eyeX, eyeY, eyeZ);
    VECTOR target = VGet(0, 0, 0);

    cam.SetCamera(eye, target);

    cam.SetNearFar(1.0f, 10000.0f);
    cam.SetScreenCenter(Define::OUT_W / 2.f, Define::OUT_H / 2.f);
    cam.Apply();

    // ① Far background & water (strong fog)
    for (auto& u : backgrounds) u->Draw(VGet(0, 0, 0));
    for (auto& u : waters)      u->Draw(VGet(0, 0, 0));
    // ② Ground (medium fog)
    for (auto& u : grounds)     u->Draw(VGet(0, 0, 0));
    // ③ Top (light fog)
    for (auto& u : tops)        u->Draw(VGet(0, 0, 0));

    SetDrawMode(DX_DRAWMODE_NEAREST);
}