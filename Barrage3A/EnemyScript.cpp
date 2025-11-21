#include "EnemyScript.h"
#include "GameManager.h"
#include "PathManager.h"
#include "Enemy.h"
#include "Mathf.h"


void EnemyScript::Add(const EnemyInstruction& instr) {
    instructions.push_back(instr);
}

void EnemyScript::Update(Enemy& enemy) {
    if (finished) return;

    while (true) {
        if (ip >= instructions.size()) {
            finished = true;
            return;
        }

        const EnemyInstruction& instr = instructions[ip];

        switch (instr.type) {
        case EnemyInstruction::Type::SetHp:
            enemy.SetHp(instr.extra);
            ip++;
            break;

        case EnemyInstruction::Type::SetHitboxEnable:
            enemy.SetHitboxEnable(instr.extra != 0);
            ip++;
            break;

        case EnemyInstruction::Type::SetDropItem:
            enemy.AddDropItem(static_cast<ItemType>(instr.extra), instr.duration);
            ip++;
            break;

        case EnemyInstruction::Type::SetAngle:
            enemy.SetAngle(instr.angle);
            ip++;
            break;

        case EnemyInstruction::Type::Die:
            enemy.Die();
            finished = true;  // 死亡なのでスクリプトも終了して良いかも
            return;
        case EnemyInstruction::Type::MoveTo:
            if (frame == 0) {
                startPos = enemy.GetPosition();
            }
            {
                float t = float(frame) / instr.duration;
                if (t > 1.f) t = 1.f;
                Vector2D newPos = startPos * (1 - t) + instr.position * t;
                enemy.SetPosition(newPos);
                frame++;
                if (frame >= instr.duration) {
                    frame = 0;
                    ip++;
                }
            }
            return; // 継続命令は1フレームずつUpdate終了

        case EnemyInstruction::Type::Wait:
            frame++;
            if (frame >= instr.duration) {
                frame = 0;
                ip++;
            }
            return;

        case EnemyInstruction::Type::Teleport:
            enemy.SetPosition(instr.position);
            ip++;
            break;

        case EnemyInstruction::Type::Fire:
            enemy.FireShot(instr.shotScriptID);
            ip++;
            break;

        case EnemyInstruction::Type::SetVelocity:
            enemy.SetVelocity(instr.vector);
            ip++;
            break;

        case EnemyInstruction::Type::Accelerate:
            enemy.Accelerate(instr.vector);
            ip++;
            break;

        case EnemyInstruction::Type::Rotate:
            enemy.Rotate(instr.angle);
            frame++;
            if (frame >= instr.duration) {
                frame = 0;
                ip++;
            }
            return;
        case EnemyInstruction::Type::OscillateX:
            if (frame == 0) {
                startPos = enemy.GetPosition();
            }
            {
                float amplitude = instr.vector.x;
                float period = instr.vector.y;
                float angle = Mathf::PI2 * (float(frame) / period);
                float offsetX = std::sin(angle) * amplitude;
                Vector2D pos = startPos;
                pos.x += offsetX;
                enemy.SetPosition(pos);
                frame++;
                if (frame >= instr.duration) {
                    frame = 0;
                    ip++;
                }
            }
            return;
        case EnemyInstruction::Type::OscillateY:
            if (frame == 0) {
                startPos = enemy.GetPosition();
            }
            {
                float amplitude = instr.vector.x;
                float period = instr.vector.y;
                float angle = Mathf::PI2 * (float(frame) / period);  // sin(2πt / T)
                float offsetY = std::sin(angle) * amplitude;
                Vector2D pos = startPos;
                pos.y += offsetY;
                enemy.SetPosition(pos);
                frame++;
                if (frame >= instr.duration) {
                    frame = 0;
                    ip++;
                }
            }
            return;
        case EnemyInstruction::Type::SetAccelerationPattern:
            enemy.SetAcceleration(instr.vector);  // 加速度設定
            ip++;
            return;
        case EnemyInstruction::Type::HomingPlayer:
        {
            Vector2D toPlayer = GameManager::GetInstance().GetPlayerPosition() - enemy.GetPosition();
            float targetAngle = std::atan2(toPlayer.y, toPlayer.x);
            float newAngle = Mathf::MoveAngle(enemy.GetAngle(), targetAngle, Mathf::DegToRad(instr.angle));
            enemy.SetAngle(newAngle);
            Vector2D velocity = Vector2D::FromAngle(newAngle) * enemy.GetSpeed();
            enemy.SetVelocity(velocity);

            frame++;
            if (frame >= instr.duration) {
                frame = 0;
                ip++;
            }
        }
        return;
        case EnemyInstruction::Type::FollowPath:
        {
            auto path = PathManager::GetInstance().GetPath(static_cast<int>(instr.extra));
            if (path) {
                float t = float(frame) / float(instr.duration);
                Vector2D pos = path->GetPositionAt(t);
                enemy.SetPosition(pos);
            }

            frame++;
            if (frame >= instr.duration) {
                frame = 0;
                ip++;
            }
        }
        return;
        case EnemyInstruction::Type::SetUpdateFunction:
        {
            enemy.SetCustomUpdateFunction(instr.shotScriptID);  // 関数名やラベルで登録
            ip++;
        }
        return;

        case EnemyInstruction::Type::LoopStart:
            loopStack.push_back({ ip, instr.loopCount });
            ip++;
            break;

        case EnemyInstruction::Type::LoopEnd:
            if (loopStack.empty()) {
                // エラー or 無視
                ip++;
                break;
            }
            {
                auto& top = loopStack.back();
                if (--top.second > 0) {
                    ip = top.first + 1; // ループ開始の次の命令へ
                }
                else {
                    loopStack.pop_back();
                    ip++;
                }
            }
            break;

        case EnemyInstruction::Type::CallSubroutine:
            callStack.push(ip + 1);
            ip = FindLabel(instr.shotScriptID); // ラベル検索関数（ラベル→命令位置）
            if (ip == -1) {
                // ラベル見つからずエラー扱いか終了
                finished = true;
                return;
            }
            break;

        case EnemyInstruction::Type::SetEnemyType:
            enemy.SetType(instr.extra, instr.duration);
            ip++;
            break;

        case EnemyInstruction::Type::EmitEffect:
            enemy.EmitEffect(instr.extra);
            ip++;
            break;

        case EnemyInstruction::Type::Offset:
            enemy.SetOffset(instr.extra, instr.position);
            ip++;
            break;
        case EnemyInstruction::Type::RandomMove: {
            // instr.position = center, instr.vector.x/y = radiusX/Y, instr.extra = changeInterval, instr.duration = totalDuration (0=infinite)
            int changeInterval = std::max(1, instr.extra);
            // 初回到達時に instrCounters[ip] が未設定なら作る
            if (instrCounters.find(ip) == instrCounters.end()) instrCounters[ip] = 0;

            // frame はこの命令内で「補間フレーム」を表す（既存の慣習に合わせる）
            if (frame == 0) {
                // 新しいターゲットを選ぶ
                float cx = instr.position.x, cy = instr.position.y;
                float rx = instr.vector.x, ry = instr.vector.y;
                float tx = Random.Range(cx - rx, cx + rx);
                float ty = Random.Range(cy - ry, cy + ry);
                instrPositions[ip] = Vector2D(tx, ty);
                startPos = enemy.GetPosition();
            }

            // 補間
            int localInterval = changeInterval;
            float t = float(frame) / float(localInterval);
            if (t > 1.f) t = 1.f;
            Vector2D target = instrPositions[ip];
            Vector2D newPos = startPos * (1 - t) + target * t;
            enemy.SetPosition(newPos);

            frame++;
            instrCounters[ip]++; // 総経過フレームとして使う

            if (frame >= localInterval) {
                frame = 0; // 次のターゲットを選ぶためにリセット（次の Update で選択）
            }

            // 終了判定
            if (instr.duration > 0 && instrCounters[ip] >= instr.duration) {
                instrCounters.erase(ip);
                instrPositions.erase(ip);
                frame = 0;
                ip++;
            }
            return;
        }

        case EnemyInstruction::Type::RandomWander: {
            // instr.vector.x/y = amplitudeX/Y, instr.extra = changeInterval, instr.duration = totalDuration
            int changeInterval = std::max(1, instr.extra);
            if (instrCounters.find(ip) == instrCounters.end()) instrCounters[ip] = 0;

            if (frame == 0) {
                // 小さなターゲット（現在位置 + ランダムオフセット）
                Vector2D pos = enemy.GetPosition();
                float ax = instr.vector.x, ay = instr.vector.y;
                float tx = pos.x + Random.Range(-ax, ax);
                float ty = pos.y + Random.Range(-ay, ay);
                instrPositions[ip] = Vector2D(tx, ty);
                startPos = enemy.GetPosition();
            }

            int localInterval = changeInterval;
            float t = float(frame) / float(localInterval);
            if (t > 1.f) t = 1.f;
            Vector2D tgt = instrPositions[ip];
            Vector2D newPos = startPos * (1 - t) + tgt * t;
            enemy.SetPosition(newPos);

            frame++;
            instrCounters[ip]++;

            if (frame >= localInterval) {
                frame = 0; // 次の小目標へ
            }

            if (instr.duration > 0 && instrCounters[ip] >= instr.duration) {
                instrCounters.erase(ip);
                instrPositions.erase(ip);
                frame = 0;
                ip++;
            }
            return;
        }

        case EnemyInstruction::Type::End:
            finished = true;
            return;

        default:
            ip++;
            break;
        }
    }
}

int EnemyScript::FindLabel(const std::string& labelName) const {
    auto it = labelMap.find(labelName);
    return (it != labelMap.end()) ? it->second : -1;
}


void EnemyScript::Advance() {
    ip++;
    frame = 0;
}

bool EnemyScript::IsFinished() const {
    return finished;
}