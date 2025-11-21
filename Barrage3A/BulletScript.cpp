#include "BulletScript.h"
#include "BulletBase.h"
#include "Transform2D.h"
#include "GameManager.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"

void BulletScript::Update(std::shared_ptr<Transform2D> tr, BulletBase* bullet) {


    bool isLoop = true;
    while (isLoop)
    {
        if (ip >= instructions.size()) {
            return; // 命令終了
        }

        const auto& instr = instructions[ip];

        switch (instr.type) {
        case BulletInstruction::Type::SetBulletType:
            bullet->SetBulletType(instr.parentID, instr.color);
            ++ip;
            frame = 0;
            break;

        case BulletInstruction::Type::SetVelocity:
            speed = instr.value1;
            angle = instr.value2;
            ++ip;
            frame = 0;
            break;

        case BulletInstruction::Type::Accelerate:
            speed += instr.value1;
            if (++frame >= instr.duration) {
                ++ip;
                frame = 0;
            }
            isLoop = false;
            break;

        case BulletInstruction::Type::Rotate:
            angle += instr.value1;
            if (++frame >= instr.duration) {
                ++ip;
                frame = 0;
            }
            isLoop = false;
            break;

        case BulletInstruction::Type::Wait:
            if (++frame >= instr.duration) {
                ++ip;
                frame = 0;
            }
            isLoop = false;
            break;

        case BulletInstruction::Type::LerpVelocity: {
            if (frame == 0) lerpStartSpeed = speed;  // 初回のみ記録
            float t = static_cast<float>(frame) / instr.duration;
            speed = Mathf::Lerp(lerpStartSpeed, instr.value1, t);
            if (++frame >= instr.duration) {
                speed = instr.value1;
                ++ip;
                frame = 0;
            }
            isLoop = false;
            break;
        }

        case BulletInstruction::Type::RotateTo: {
            if (frame == 0) lerpStartAngle = angle;
            float t = static_cast<float>(frame) / instr.duration;
            angle = Mathf::LerpAngle(lerpStartAngle, instr.value1, t);
            if (++frame >= instr.duration) {
                angle = instr.value1;
                ++ip;
                frame = 0;
            }
            isLoop = false;
            break;
        }
        case BulletInstruction::Type::AddRotateTo: {
            if (frame == 0) {
                lerpStartAngle = angle;  // メンバ変数で開始角度を保持
                targetAngle = lerpStartAngle + instr.value1;  // instr.value1 は相対角度（度）
            }
            float t = float(frame) / instr.duration;
            angle = Mathf::LerpAngle(lerpStartAngle, targetAngle, t);

            if (++frame >= instr.duration) {
                angle = targetAngle;
                frame = 0;
                ++ip;
            }
            isLoop = false;
            break;
        }

        case BulletInstruction::Type::AimAtPlayer: {
            Vector2D diff = GameManager::GetInstance().GetPlayerPosition() - tr->position;
            angle = std::atan2(diff.y, diff.x) * 180.0f / Mathf::PI;
            ++ip;
            break;
        }

        case BulletInstruction::Type::Seek: {
            Vector2D diff = GameManager::GetInstance().GetPlayerPosition() - tr->position;
            float targetAngle = std::atan2(diff.y, diff.x) * 180.0f / Mathf::PI;
            float delta = instr.value1;
            float diffAngle = Mathf::DeltaAngle(angle, targetAngle);
            angle += std::clamp(diffAngle, -delta, delta);
            if (++frame >= instr.duration) {
                ++ip;
                frame = 0;
            }
            isLoop = false;
            break;
        }

        case BulletInstruction::Type::RandomizeAngle:
            angle += (Random.Value() * 2.f - 1.f) * instr.value1;
            ++ip;
            break;

        case BulletInstruction::Type::Branch:
            // 未実装：弾の複製が必要。今後対応予定。
            ++ip;
            break;

        case BulletInstruction::Type::DelayedFire:
            if (++frame >= instr.duration) {
                // ip を進めない → 外部から Fire が呼ばれる
                frame = 0;
            }
            isLoop = false;
            break;

        case BulletInstruction::Type::Wave: {
            float amplitude = instr.value1;  // 振幅（度）
            float frequency = instr.value2;  // 周波数 (Hz)
            float time = float(frame) / 60.f;  // 秒換算 (60FPS前提)
            float offset = amplitude * std::sin(2.f * Mathf::PI * frequency * time);

            angle = angle - lastWaveOffset + offset;  // 角度を波打ちオフセットで補正
            lastWaveOffset = offset;

            if (++frame >= instr.duration) {
                lastWaveOffset = 0.f;
                ++ip;
                frame = 0;
            }
            isLoop = false;
            break;
        }
        case BulletInstruction::Type::Zigzag: {
            float angleDelta = instr.value1;  // 折り返し時の角度オフセット
            int period = instr.duration;      // 片道の周期フレーム数

            if (frame == 0) {
                baseAngle = angle;
            }

            int totalPeriod = period * 2;
            int currentFrame = frame % totalPeriod;

            float offset = (currentFrame < period) ? angleDelta : -angleDelta;

            angle = baseAngle + offset;

            if (++frame >= totalPeriod) {
                ++ip;
                frame = 0;
            }
            isLoop = false;
            break;
        }
        case BulletInstruction::Type::PlaySE: {
            auto clip = Sounds[instr.soundName];
            auto scn = MusicController::GetInstance();
            if (scn && clip) scn->OneShotAudio(clip->Clone(), instr.volume);
            ip++;
            break;
        }
        case BulletInstruction::Type::LoopStart: {
            int loopCount = static_cast<int>(instr.value1);
            if (loopCount <= 0) {
                ++ip;  // スキップ
                break;
            }

            loopStack.push(LoopContext((int)ip, loopCount));
            ++ip;
            break;
        }

        case BulletInstruction::Type::LoopEnd: {
            if (loopStack.empty()) {
                ++ip; // エラー: LoopStart 対応なし
                break;
            }

            auto& ctx = loopStack.top();
            ctx.remaining--;

            if (ctx.remaining > 0) {
                ip = ctx.startIp + 1; // ループ本体に戻る
            }
            else {
                loopStack.pop();
                ++ip;
            }
            break;
        }
        case BulletInstruction::Type::End:
            ip = instructions.size();
            return;
        default:
            ip++;
            break;
        }
    }
    // 最後に弾の方向と速度を反映
    tr->rotation = angle;
    bullet->SetSpeed(speed);
}
