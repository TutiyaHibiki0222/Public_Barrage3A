/*
    ◆ ParticleSystem.cpp

    クラス名        : ParticleSystem クラス
    作成日          : 2025/06/28 (土) 04:20:22
	最終変更日      : 2025/06/28 (土) 20:25:05
    作成者          : 𡈽屋 響
	概要            : エフェクト用のパーティクルシステム.
*/
#include "ParticleSystem.h"
#include "RendererManager.h"
#include "GameObject.h"
#include "GameObjectMgr.h"

// コンストラクタ 
ParticleSystem::ParticleSystem() : AppBase("ParticleSystem"){

}
ParticleSystem::ParticleSystem(std::shared_ptr<GameObject> owner) : AppBase("ParticleSystem", owner) {

}

// AppBase Event.
void ParticleSystem::Awake() {
    RendererManager::GetInstance().AddRenderer(shared_from_this());
}

void ParticleSystem::Restart() {
    for (auto& p : particles) {
        p.active = false; // 初期状態は非アクティブ
    }
    elapsedTime = 0.0f;
    emitCounter = 0.0f;
    burstDone = false;
}

void ParticleSystem::Start() {
    if (settings.maxParticles <= 0) return;
    particles.resize(settings.maxParticles);
    for (auto& p : particles) {
        p.active = false;       // 初期状態は非アクティブ
        p.sizeCurrent = 0.0f;   // 初期サイズ
    }
    elapsedTime = 0.0f;
    emitCounter = 0.0f;
    burstDone = false;
}

void ParticleSystem::Update() {

	if (isStop) return; // 停止状態なら何もしない

    bool allowContinuous = (settings.emissionDuration < 0.0f) || (elapsedTime < settings.emissionDuration);

    float dt = Time.deltaTime;
    elapsedTime += dt;

    if (!burstDone && settings.burstCount > 0) {
        for (int i = 0; i < settings.burstCount; ++i) EmitParticle();
        burstDone = true;
    }

    if (allowContinuous && settings.burstCount <= 0) {
        float interval = 0.0f;

        if (settings.emissionInterval > 0.0f) {
            interval = settings.emissionInterval;   // 明示的な間隔指定
        }
        else if (settings.emissionRate > 0.0f) {
            interval = 1.0f / settings.emissionRate; // Rate指定（毎秒〇個）
        }

        if (interval > 0.0f) {
            emitCounter += dt;
            while (emitCounter >= interval) {
                EmitParticle();
                emitCounter -= interval;
            }
        }
    }

    if (dt != 0) {
        for (auto& p : particles) {
            if (!p.active) continue;



            p.life -= dt;
            if (p.life <= 0) { p.active = false; continue; }

            if (settings.customUpdate) {
                // カスタム関数で更新
                settings.customUpdate(p, transform->GetWorldPosition(), transform->GetWorldScale());
            }
            else {
                // デフォルト更新処理
                p.pos.x += p.vc.x * dt;
                p.pos.y += p.vc.y * dt;
                p.rot.x += p.rotSpeed.x * dt;
                p.rot.y += p.rotSpeed.y * dt;
                p.rot.z += p.rotSpeed.z * dt;

                // サイズやアルファを寿命に応じて更新
                float t = p.life / p.totalLife; // 寿命比率(1→0)
                p.sizeCurrent = p.size * t;     // 線形で小さくする例
                // カラー（寿命によってアルファと明度をフェードアウト）
                p.color.a = p.startColor.a * t;
            }
        }
    }
    if (isEndDestroy) {
        bool allInactive = true;
        for (auto& p : particles) {
            if (p.active) {
                allInactive = false;
                break;
            }
        }
        if (allInactive) {
            Destroy();
        }
    }
}

void ParticleSystem::OnDestroy() {
    RendererManager::GetInstance().Remove(shared_from_this());
}

void ParticleSystem::Draw() {

    if (isStop) return;
    if (particles.empty()) return;

    // カスタム描画が設定されていればそれを優先
    if (customDraw) {
        for (auto& p : particles) {
            if (p.active) {
                customDraw(p, *sprite);
            }
        }
        return;
    }

    if (!sprite) return;
    auto sp = sprite;
    auto scale = transform->GetWorldScale();

    for (auto& p : particles) {
        if (!p.active) continue;
        // --- デフォルト描画 ---
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, p.color.A255());
        SetDrawBright(p.color.R255(), p.color.G255(), p.color.B255());

        float halfX = (sp->width * 0.5f) * p.sizeCurrent * scale.x;
        float halfY = (sp->height * 0.5f) * p.sizeCurrent * scale.y;

        float radZ = Mathf::DegToRad(p.rot.z);
        float radX = Mathf::DegToRad(p.rot.x);
        float radY = Mathf::DegToRad(p.rot.y);

        float sx = Mathf::Cos(radY);
        float sy = Mathf::Cos(radX);

        float dx[4] = { -halfX,  halfX, halfX, -halfX };
        float dy[4] = { -halfY, -halfY, halfY,  halfY };

        float px[4], py[4];
        for (int i = 0; i < 4; ++i) {
            float x = dx[i] * sx;
            float y = dy[i] * sy;
            px[i] = p.pos.x + x * Mathf::Cos(radZ) - y * Mathf::Sin(radZ);
            py[i] = p.pos.y + x * Mathf::Sin(radZ) + y * Mathf::Cos(radZ);
        }

        DrawModiGraph(
            (int)px[0], (int)py[0], (int)px[1], (int)py[1],
            (int)px[2], (int)py[2], (int)px[3], (int)py[3],
            sp->spriteData, TRUE
        );

    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
    SetDrawBright(255, 255, 255); // 色をリセット
}


bool ParticleSystem::IsDraw() {
    return gameObject->IsActive() && enabled;
}

int ParticleSystem::GetSortingOrder() const {
    return sortingLayer.GetSortingOrder();
}

RenderMode ParticleSystem::GetRenderMode() const {
    return renderMode;
}

void ParticleSystem::EmitParticle() {

    Vector2D origin = transform->GetWorldPosition();

    // ワールド回転角を取得（度→ラジアン）
    float rotationDeg = -transform->GetWorldRotation();
    float rotationRad = Mathf::DegToRad(rotationDeg);
    float cosRot = cosf(rotationRad);
    float sinRot = sinf(rotationRad);

    for (auto& p : particles) {
        if (!p.active) {
            p.active = true;

            // -------------------------
            // 生成位置（回転適用あり）
            // -------------------------
            float localX = 0.0f;
            float localY = 0.0f;

            switch (settings.spawnShape) {
            case SpawnParticleShape::Circle: {
                float angle = Random.Range(0.0f, 2.0f * Mathf::PI);
                float dist = Random.Range(settings.spawnRadiusMin, settings.spawnRadiusMax);
                localX = cosf(angle) * dist;
                localY = sinf(angle) * dist;
                break;
            }
            case SpawnParticleShape::Square: {
                localX = Random.Range(-settings.spawnRadiusMax, settings.spawnRadiusMax);
                localY = Random.Range(-settings.spawnRadiusMax, settings.spawnRadiusMax);
                break;
            }
            case SpawnParticleShape::Box: {
                localX = Random.Range(settings.spawnBoxMin.x, settings.spawnBoxMax.x);
                localY = Random.Range(settings.spawnBoxMin.y, settings.spawnBoxMax.y);
                break;
            }
            case SpawnParticleShape::Line: {
                localX = Random.Range(-settings.spawnRadiusMax, settings.spawnRadiusMax);
                localY = 0.0f;
                break;
            }
            }

            // 回転適用
            float worldX = localX * cosRot - localY * sinRot;
            float worldY = localX * sinRot + localY * cosRot;
            p.pos.x = origin.x + worldX;
            p.pos.y = origin.y + worldY;

            // -------------------------
            // 寿命・初期サイズ
            // -------------------------
            float life = Random.Range(settings.lifetimeMin, settings.lifetimeMax);
            p.life = p.totalLife = life;

            p.size = Random.Range(settings.sizeMin, settings.sizeMax);
            p.sizeCurrent = p.size;

            // 終端サイズ（寿命末に向かって成長するサイズ）
            p.targetSize = Random.Range(settings.endSizeMin, settings.endSizeMax);


            // -------------------------
            // 発射方向と速度（回転対応）
            // -------------------------
            float speed = Random.Range(settings.speedMin, settings.speedMax);
            float angle = 0.0f;
            bool velocitySet = false;

            float velocityDirectionRad = Mathf::DegToRad(settings.velocityDirectionDeg);
            float velocitySpreadRad = Mathf::DegToRad(settings.velocitySpreadDeg);

            switch (settings.velocityMode) {
            case VelocityDirectionMode::Random360:
                angle = Random.Range(0.0f, 2.0f * Mathf::PI);
                break;
            case VelocityDirectionMode::Cone:
                angle = velocityDirectionRad + Random.Range(-velocitySpreadRad / 2.0f, velocitySpreadRad / 2.0f);
                break;
            case VelocityDirectionMode::Fixed:
                angle = velocityDirectionRad;
                break;
            case VelocityDirectionMode::Line:
                angle = (Random.Range(0, 1) > 0.5f) ? velocityDirectionRad : velocityDirectionRad + Mathf::PI;
                break;
            case VelocityDirectionMode::CustomFunction:
                if (settings.customVelocityFunc) {
                    Vector2D dir = settings.customVelocityFunc().GetNormalize();
                    p.vc.x = dir.x * speed;
                    p.vc.y = dir.y * speed;
                    velocitySet = true;
                }
                else {
                    angle = Random.Range(0.0f, 2.0f * Mathf::PI);
                }
                break;
            }

            if (!velocitySet) {
                float vx = cosf(angle) * speed;
                float vy = sinf(angle) * speed;

                // ワールド回転適用
                p.vc.x = vx * cosRot - vy * sinRot;
                p.vc.y = vx * sinRot + vy * cosRot;
            }

            // -------------------------
            // 回転と色
            // -------------------------
            p.rot.x     = Random.Range(settings.startRotationMin.x, settings.startRotationMax.x);
            p.rot.y     = Random.Range(settings.startRotationMin.y, settings.startRotationMax.y);
            p.rot.z     = Random.Range(settings.startRotationMin.z, settings.startRotationMax.z);

            p.rotSpeed.x = Random.Range(settings.rotationSpeedMin.x, settings.rotationSpeedMax.x);
            p.rotSpeed.y = Random.Range(settings.rotationSpeedMin.y, settings.rotationSpeedMax.y);
            p.rotSpeed.z = Random.Range(settings.rotationSpeedMin.z, settings.rotationSpeedMax.z);

            // ランダムカラー設定
            if (settings.useRandomStartColor) {
                // 各チャンネルを最小～最大の範囲で乱数生成
                int r = Random.Range(settings.startColorMin.R255(), settings.startColorMax.R255());
                int g = Random.Range(settings.startColorMin.G255(), settings.startColorMax.G255());
                int b = Random.Range(settings.startColorMin.B255(), settings.startColorMax.B255());
                int a = Random.Range(settings.startColorMin.A255(), settings.startColorMax.A255());
                Color randCol = { r, g, b, a };
                p.startColor = p.color = randCol;
            }
            else {
                // 固定色
                p.startColor = p.color = settings.startColor;
            }
            break; // 1つだけ発生
        }
    }
}
