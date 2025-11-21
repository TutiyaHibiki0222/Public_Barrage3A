#pragma once

#include "ShotInstruction.h"
#include "BulletType.h"
#include "BulletScript.h"
#include <memory>
#include <vector>
#include <map>
#include <cmath>

class ShotScript {
private:
    struct LoopInfo {
        size_t startIp;       // ループ開始位置の命令ポインタ
        int remainingCount;   // 残りループ回数
    };

    struct RegisteredBullet {
        float angle = 0.f;            // 弾登録時の初期角度
        float speed = 0.f;            // 弾登録時の初速度
        int templateId  = 0;          // 弾の種類ID
        int colorId     = 0;             // 弾の色ID
        Vector2D offset{ 0, 0 };      // 発射位置のオフセット
        float angularVelocity = 0.f;  // 角速度（回転速度）
        float acceleration = 0.f;     // 加速度
    };

    std::vector<ShotInstruction> instructions;          // 命令列
    std::vector<LoopInfo> loopStack;                    // ループ状態スタック
    std::map<int, RegisteredBullet> registeredBullets;  // 登録弾管理

    BulletParentID defaultTemplateId = BulletParentID::B1;  // デフォルト弾種
    BulletColor defaultColorId       = BulletColor::Red;    // デフォルト色
    std::shared_ptr<BulletScript> control;

    size_t ip = 0;            // 命令実行位置（命令ポインタ）
    int frame = 0;            // 待機用フレームカウンタ
    bool finished = false;    // スクリプト終了フラグ

    float randomAngleRange = 0.f;  // ランダム角度の最大振れ幅（degrees）
    // ShotScript のメンバ変数に追加
    float defaultAngularVelocity = 0.f;
    float defaultAcceleration = 0.f;

    float baseAngle = 0.f;
public:
    // 命令を追加
    void AddInstruction(const ShotInstruction& instr);

    // 命令実行 更新
    void Update(Vector2D position);

    // クローン作成（状態は初期化される）
    std::shared_ptr<ShotScript> Clone() const;

    void SetBulletScript(const std::shared_ptr<BulletScript> _control);

    // スクリプト終了判定
    bool IsFinished() const {
        return finished;
    }

private:
    // 弾発射処理
    void FireBullet(Vector2D pos, float angle, float speed, int slot = -1);
    float CalculateAngleToPlayer(Vector2D from, Vector2D to) const {
        Vector2D diff = to - from;
        return std::atan2(diff.y, diff.x) * 180.f / 3.14159265f;
    }
    // 角度にランダムノイズを加える
    void ApplyRandomAngle(float& angle);

};
