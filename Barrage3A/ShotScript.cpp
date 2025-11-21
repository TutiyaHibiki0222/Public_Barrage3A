#include "ShotScript.h"
#include "GameManager.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"
#include "BulletType.h"  // CreateBulletのために必要
#include "BulletScriptManager.h"
#include <cstdlib>              // rand(), RAND_MAX
#include <cmath>

void ShotScript::AddInstruction(const ShotInstruction& instr) {
    instructions.push_back(instr);
}

void ShotScript::Update(Vector2D position) {
    if (finished) return;

    while (true) {
        if (ip >= instructions.size()) {
            finished = true;
            return;
        }

        const auto& instr = instructions[ip];
        switch (instr.type) {
        // ─── 設定系 ─────────────────────────────────────────
        case ShotInstruction::Type::SetEnemyBulletType: {
            RegisteredBullet& rb = registeredBullets[instr.slot];
            rb.templateId   = instr.count;
            rb.colorId      = instr.extra;
            ip++;
            break;
        }
        case ShotInstruction::Type::SetBulletDefault: {
            defaultTemplateId = static_cast<BulletParentID>(instr.count);
            defaultColorId = static_cast<BulletColor>(instr.extra);
            ip++;
            break;
        }
        // ------ アクション系 --------------------------------------------------------------------------
        case ShotInstruction::Type::PlaySE: {
            auto clip = Sounds[instr.soundName];
            auto scn = MusicController::GetInstance();

            if (scn && clip) scn->OneShotAudio(clip->Clone(), instr.volume);

            ip++;
            break;
        }
        // ─── 発射前制御系 ─────────────────────────────────────
        case ShotInstruction::Type::RandomizeAngle: {
            // グローバル乱数範囲を設定（全弾をこの命令後に発射する場合）
            randomAngleRange = instr.maxOffset;
            ip++;
            break;
        }
        case ShotInstruction::Type::Offset: {
            RegisteredBullet& rb = registeredBullets[instr.slot];
            rb.offset = { instr.angle, instr.speed };
            ip++;
            break;
        }
        case ShotInstruction::Type::EmitEffect: {
            // 発射時エフェクト。slotフィールドをEffectIDとして利用
            //EffectManager::GetInstance().PlayEffect(instr.slot, position);
            ip++;
            break;
        }
        case ShotInstruction::Type::EmitSpeedUp: {
            RegisteredBullet& rb = registeredBullets[instr.slot];
            rb.speed += instr.speed * (instr.angle2 > 0 ? 1 : -1);
            ip++;
            break;
        }
        case ShotInstruction::Type::EmitDelay: {
            // 後回し実装: 弾発射をこのあとdurationフレーム遅延させるなら待機命令をはさむなど
            ip++;
            break;
        }
        case ShotInstruction::Type::SetBulletScript: {
			control = BulletScriptManager::GetInstance().CloneBulletScript(instr.label);
            ip++;
            break;
        }
        // ─── 発射命令 ─────────────────────────────────────────
        case ShotInstruction::Type::Fire: {
            FireBullet(position,
                baseAngle + instr.angle,
                instr.speed,
                /*slot*/ -1);
            ip++;
            break;
        }
        case ShotInstruction::Type::FireMultiSpeed: {
            for (int i = 0; i < instr.count; ++i) {
                float speed = instr.speeds[i];
                FireBullet(position, baseAngle + instr.angle, speed);
            }
            ++ip;
            break;
        }
        case ShotInstruction::Type::FireSpread: {
            int slot = instr.slot;
            float startAngle = baseAngle + instr.angle;
            float speed = instr.speed;
            int count = instr.count;
            float totalAngle = instr.totalAngle;

            for (int i = 0; i < count; ++i) {
                float angle;
                if (std::fabs(totalAngle - 360.0f) < 0.001f) {
                    angle = startAngle + (totalAngle / count) * i;
                }
                else {
                    angle = startAngle + (totalAngle / (count - 1)) * i;
                }

                FireBullet(position, angle, speed, -1);
            }

            ip++;
            break;
        }
        case ShotInstruction::Type::FireSpreadCentered: {
            int slot = instr.slot;
            float startAngle = baseAngle + instr.angle;
            float speed = instr.speed;
            int count = instr.count;
            float totalAngle = instr.totalAngle;

            if (count <= 0) {
                ip++;
                break;
            }

            if (count == 1) {
                FireBullet(position, startAngle, speed, slot);
            }
            else {
                float angleStep = totalAngle / (count - 1);
                float beginAngle = startAngle - totalAngle * 0.5f;

                for (int i = 0; i < count; ++i) {
                    float angle = beginAngle + i * angleStep;
                    FireBullet(position, angle, speed, slot);
                }
            }

            ip++;
            break;
        }
        case ShotInstruction::Type::FireSpreadShuffled: {
            int slot = instr.slot;
            float startAngle = baseAngle + instr.angle;
            float speed = instr.speed;
            int count = instr.count;
            float totalAngle = instr.totalAngle;

            std::vector<float> angles;
            angles.reserve(count);

            for (int i = 0; i < count; ++i) {
                float angle;
                if (std::fabs(totalAngle - 360.0f) < 0.001f) {
                    angle = startAngle + (totalAngle / count) * i;
                }
                else {
                    angle = startAngle + (totalAngle / (count - 1)) * i;
                }
                angles.push_back(angle);
            }

            std::shuffle(angles.begin(), angles.end(),Random.GetEngine());

            for (float angle : angles) {
                FireBullet(position, angle, speed, slot);
            }

            ip++;
            break;
        }
        case ShotInstruction::Type::FireSpreadRandomized: {
            float centerAngle = baseAngle + instr.angle;
            float totalAngle = instr.totalAngle;
            int count = instr.count;
            float speed = instr.speed;

            for (int i = 0; i < count; ++i) {
                // 0～1の範囲でランダムにばらつかせる
                float randT = Random.Value(); // 0.0～1.0の乱数を返す関数
                float angle = centerAngle - totalAngle * 0.5f + totalAngle * randT;

                FireBullet(position, angle, speed, -1);
            }

            ip++;
            break;
        }
        case ShotInstruction::Type::RegisterBullet: {
            RegisteredBullet& rb = registeredBullets[instr.slot];
            rb.speed = instr.speed;
            rb.angle = instr.angle;
            ip++;
            break;
        }
        case ShotInstruction::Type::OnFire: {
            auto it = registeredBullets.find(instr.slot);
            if (it != registeredBullets.end()) {
                const auto& rb = it->second;
                FireBullet(position,
                    baseAngle + rb.angle,
                    rb.speed,
                    instr.slot);
            }
            ip++;
            break;
        }

        // ─── 状態変化系 ─────────────────────────────────────
        case ShotInstruction::Type::Rotate: {
            if (instr.slot < 0) {
                defaultAngularVelocity = instr.angle;
            }
            else {
                RegisteredBullet& rb = registeredBullets[instr.slot];
                rb.angularVelocity = instr.angle;
            }
            ip++;
            break;
        }
        case ShotInstruction::Type::Accelerate: {
            if (instr.slot < 0) {
                defaultAcceleration = instr.speed;
            }
            else {
                RegisteredBullet& rb = registeredBullets[instr.slot];
                rb.acceleration = instr.speed;
            }
            ip++;
            break;
        }
        case ShotInstruction::Type::AddBaseAngle:
            baseAngle += instr.angle;
            ip++;
            break;

        case ShotInstruction::Type::SetAngle:
            baseAngle = instr.angle;
            ip++;
            break;
        case ShotInstruction::Type::SetPlayerAngle: {
            Vector2D playerPos = GameManager::GetInstance().GetPlayer()->transform->position;
            baseAngle = CalculateAngleToPlayer(position, playerPos);
            ip++;
            break;
        }
        // ─── ループ・待機・終了 ─────────────────────────────────
        case ShotInstruction::Type::LoopStart:
            loopStack.push_back({ ip, instr.count });
            ip++;
            break;

        case ShotInstruction::Type::LoopEnd: {
            if (loopStack.empty()) { ip++; break; }
            auto& top = loopStack.back();
            if (--top.remainingCount > 0) {
                ip = top.startIp + 1;
            }
            else {
                loopStack.pop_back();
                ip++;
            }
            break;
        }

        case ShotInstruction::Type::Wait:
            frame++;
            if (frame >= instr.count) {
                frame = 0;
                ip++;
            }
            return;  // 1フレームごとにここで戻る

        case ShotInstruction::Type::End:
            finished = true;
            return;

        default:
            ip++;
            break;
        }
    }
}

std::shared_ptr<ShotScript> ShotScript::Clone() const {
    auto clone = std::make_shared<ShotScript>();
    clone->instructions = this->instructions;
    clone->defaultTemplateId = this->defaultTemplateId;
    clone->defaultColorId = this->defaultColorId;
    clone->control = this->control;
    // 状態クリア
    clone->ip = 0;
    clone->frame = 0;
    clone->finished = false;
    clone->loopStack.clear();
    clone->registeredBullets.clear();
    clone->randomAngleRange = 0.f;
    return clone;
}

void ShotScript::SetBulletScript(const std::shared_ptr<BulletScript> _control) {
    control = _control;
}

void ShotScript::ApplyRandomAngle(float& angle) {
    if (randomAngleRange > 0.f) {
        float t = Random.Value();
        float delta = (t * 2.f - 1.f) * randomAngleRange;    // [-range, +range]
        angle += delta;
    }
}

void ShotScript::FireBullet(Vector2D pos, float angle, float speed, int slot) {
    RegisteredBullet bulletInfo;
    if (slot >= 0) {
        auto it = registeredBullets.find(slot);
        if (it == registeredBullets.end()) return;
        bulletInfo = it->second;
    }
    else {
        // デフォルト
        bulletInfo.angle = 0.f;
        bulletInfo.speed = speed;
        bulletInfo.templateId = static_cast<int>(defaultTemplateId);
        bulletInfo.colorId = static_cast<int>(defaultColorId);
    }

    // 発射位置オフセット反映
    pos += bulletInfo.offset;

    // ベースRandomノイズを反映
    ApplyRandomAngle(angle);

    // 弾の生成
    BulletParentID typeId = static_cast<BulletParentID>(bulletInfo.templateId);
    BulletColor    colorId = static_cast<BulletColor>(bulletInfo.colorId);
    auto bulletGO = BulletTypeManager::GetInstance().CreateBullet(typeId, colorId);
    if (!bulletGO) return;

    bulletGO->transform->position = pos;
    bulletGO->transform->rotation = angle;

    // 弾コンポーネント設定
    auto bullet = bulletGO->AddAppBase<EnemyBullet>();
    bullet->SetSpeed(bulletInfo.speed);
    // FireBullet 内で
    bullet->SetRotationRate(slot >= 0 ? bulletInfo.angularVelocity : defaultAngularVelocity);
    bullet->SetAcceleration(slot >= 0 ? bulletInfo.acceleration : defaultAcceleration);

    if (control) {
        auto bs = control->Clone();
        bs->InitSpeed(speed);
        bs->InitAngle(angle);
        bullet->SetBulletControllerInstance(bs);
    }
    // リセット：発射角度ノイズは1度だけ適用とし、次弾は再設定命令待ち
    randomAngleRange = 0.f;
}
