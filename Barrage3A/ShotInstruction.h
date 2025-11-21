#pragma once
#include "Vector.h"
#include "BulletType.h"
#include <string>

/**
 * @brief 敵の弾発射・制御スクリプトの命令（ZUNスクリプト風拡張）
 *
 * 弾のテンプレート登録、発射、ループ、角度ランダム化、回転・加速など
 * 多彩な制御が可能な命令セットを提供します。
 */
struct ShotInstruction {
    /**
     * @brief 命令の種類を表す列挙型
     */
    enum class Type {
        SetEnemyBulletType,   ///< スロットに弾のテンプレートと色を設定
        SetBulletDefault,     ///< デフォルトの弾テンプレートと色を設定
        Fire,                 ///< 単発弾発射
        FireSpread,           ///< 扇形弾発射
        FireSpreadCentered,   ///< 中心扇形弾発射.
        FireSpreadShuffled,   ///< 中心扇形弾ランダム順に発射させる.
        FireSpreadRandomized, // 中心角度 + 扇形範囲内でランダム角度ばらつき発射
        RegisterBullet,       ///< スロットに弾の角度・速度を登録
        OnFire,               ///< 登録済みスロットの弾を発射
        RandomizeAngle,       ///< 発射角度にランダムノイズを設定
        LoopStart,            ///< ループ開始（指定回数）
        LoopEnd,              ///< ループ終了
        Wait,                 ///< 待機（フレーム数）
        End,                  ///< スクリプト終了
        SetBulletScript,      ///< 弾スクリプトを設定（未使用/拡張用）
        SetEnemyType,         ///< 敵タイプ設定（未使用/拡張用）
        EmitEffect,           ///< 発射時エフェクト再生
        EmitSpeedUp,          ///< 発射速度加速（簡易）
        EmitDelay,            ///< 発射遅延（簡易）
        Offset,               ///< 発射位置オフセット設定（X,Y）
        Rotate,               ///< 弾の角速度設定
        Accelerate,           ///< 弾の加速度設定
        AddBaseAngle,         ///< 基準角度に加算
        SetAngle,             ///< 発射角度を絶対指定
        SetPlayerAngle,       ///< baseAngle を自機狙い角度に設定
        FireMultiSpeed,       ///< 複数速度で発射.
        PlaySE,               ///< サウンド（SE）を再生
    };

    Type type = Type::End; ///< 命令種別

    // --- 命令パラメータ ---
    int count = 0;           ///< 発射数、ループ回数、待機フレーム数など
    float angle = 0.f;       ///< 角度（deg）（Fire系命令の発射角など）
    float totalAngle = 0.f;  ///< FireSpreadの扇形範囲角度（deg）
    float speed = 0.f;       ///< 弾速度またはオフセットYなど用途依存
    int slot = -1;           ///< 弾登録スロット番号、エフェクトIDなど
    int extra = 0;           ///< 追加パラメータ（弾色など）
    float angle2 = 0.f;      ///< 追加パラメータ（polarityや角速度符号など）
    float maxOffset = 0.f;   ///< ランダム角度最大ぶれ（deg）
    std::string label;       ///< ラベルやID（必要に応じて）
    std::vector<float> speeds;

    std::string soundName;     ///< 再生するサウンド名（PlaySound用）
    float       volume = 1.0f;
    // ===== Static Factory Methods =====

    /**
     * @brief スロットに弾のテンプレートと色を設定する命令を作成
     * @param slot 登録スロット番号
     * @param templateId 弾テンプレートID
     * @param colorId 弾色ID
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction SetEnemyBulletType(int slot, BulletParentID templateId, BulletColor colorId) {
        ShotInstruction i{};
        i.type = Type::SetEnemyBulletType;
        i.slot = slot;
        i.count = static_cast<int>(templateId);
        i.extra = static_cast<int>(colorId);
        return i;
    }

    /**
     * @brief デフォルトの弾テンプレートと色を設定する命令を作成
     * @param templateId 弾テンプレートID
     * @param colorId 弾色ID
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction SetBulletDefault(BulletParentID templateId, BulletColor colorId) {
        ShotInstruction i{};
        i.type = Type::SetBulletDefault;
        i.count = static_cast<int>(templateId);
        i.extra = static_cast<int>(colorId);
        return i;
    }

    /**
     * @brief 単発弾を指定角度・速度で発射する命令を作成
     * @param fireAngleDegrees 発射角度（度）
     * @param speed 弾速度
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction Fire(float fireAngleDegrees, float speed) {
        ShotInstruction i{};
        i.type = Type::Fire;
        i.angle = fireAngleDegrees;
        i.speed = speed;
        return i;
    }

    /**
     * @brief 扇形に複数弾を発射する命令を作成（開始角度から範囲展開）
     * @param count 発射弾数
     * @param startAngleDegrees 開始角度（度）
     * @param spreadAngleDegrees 扇形範囲角度（度）
     * @param speed 弾速度
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction FireSpread(int count, float startAngleDegrees, float spreadAngleDegrees, float speed) {
        ShotInstruction i{};
        i.type = Type::FireSpread;
        i.count = count;
        i.angle = startAngleDegrees;
        i.totalAngle = spreadAngleDegrees;
        i.speed = speed;
        return i;
    }

    static ShotInstruction FireMultiSpeed(float fireAngleDegrees, const std::vector<float>& speedList) {
        ShotInstruction i{};
        i.type = Type::FireMultiSpeed;
        i.angle = fireAngleDegrees;
        i.count = static_cast<int>(speedList.size());
        i.speeds = speedList;
        return i;
    }

    /**
     * @brief 扇形に複数弾を発射する命令を作成（中心角度指定版）
     * @param count 発射弾数
     * @param centerAngleDegrees 中心角度（度）
     * @param spreadRangeDegrees 扇形範囲角度（度）
     * @param speed 弾速度
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction FireSpreadCentered(int count, float centerAngleDegrees, float spreadRangeDegrees, float speed) {
        ShotInstruction i{};
        i.type = Type::FireSpreadCentered;
        i.count = count;
        i.speed = speed;
        i.angle = centerAngleDegrees;
        i.totalAngle = spreadRangeDegrees;
        return i;
    }

    static ShotInstruction FireSpreadShuffled(int count, float speed, float totalAngle = 360.0f, float angle = 0.0f, int slot = -1) {
        ShotInstruction instr(Type::FireSpreadShuffled);
        instr.count = count;
        instr.speed = speed;
        instr.totalAngle = totalAngle;
        instr.angle = angle;
        instr.slot = slot;
        return instr;
    }

    static ShotInstruction FireSpreadRandomized(int count, float centerAngleDegrees, float spreadRangeDegrees, float speed) {
        ShotInstruction i{};
        i.type = Type::FireSpreadRandomized;
        i.count = count;
        i.angle = centerAngleDegrees;
        i.totalAngle = spreadRangeDegrees;
        i.speed = speed;
        return i;
    }
    /**
     * @brief 指定スロットに角度・速度を登録する命令を作成
     * @param slot 登録スロット番号
     * @param angleDegrees 弾角度（度）
     * @param speed 弾速度
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction RegisterBullet(int slot, float angleDegrees, float speed) {
        ShotInstruction i{};
        i.type = Type::RegisterBullet;
        i.slot = slot;
        i.angle = angleDegrees;
        i.speed = speed;
        return i;
    }

    /**
     * @brief 指定スロットの登録弾を発射する命令を作成
     * @param slot 登録スロット番号
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction OnFire(int slot) {
        ShotInstruction i{};
        i.type = Type::OnFire;
        i.slot = slot;
        return i;
    }

    /**
     * @brief 蛇行発射（Sinカーブを模した発射）
     * @param slot スロット番号
     * @param speed 弾の速度
     * @param baseAngle 発射基準角度
     * @return ShotInstruction
     */
    static ShotInstruction FireSnake(int slot, float speed, float baseAngle) {
        ShotInstruction i{};
        i.type = Type::RegisterBullet; // Snake用は Register + OnFire を繰り返す形で処理（制御側で工夫）
        i.slot = slot;
        i.angle = baseAngle;
        i.speed = speed;
        return i;
    }

    /**
     * @brief 扇形発射（centerから始まる方向にcount発射）
     * @param slot スロット番号（未使用なら -1）
     * @param count 発射数
     * @param spreadAngle 範囲角度（度）
     * @param speed 弾速度
     * @return ShotInstruction
     */
    static ShotInstruction FireFan(int slot, int count, float spreadAngle, float speed) {
        ShotInstruction i{};
        i.type = Type::FireSpreadCentered;
        i.slot = slot;
        i.count = count;
        i.totalAngle = spreadAngle;
        i.speed = speed;
        return i;
    }

    /**
     * @brief 円形弾発射（全周均等）
     * @param slot スロット番号（未使用なら -1）
     * @param count 発射数
     * @param speed 弾速度
     * @return ShotInstruction
     */
    static ShotInstruction FireCircle(int slot, int count, float speed) {
        return FireFan(slot, count, 360.0f, speed);
    }

    /**
     * @brief 螺旋発射（角度が回転する）
     * @param slot スロット番号（未使用なら -1）
     * @param count 発射数
     * @param speed 弾速
     * @param deltaAngle 各フレームごとの回転角（度）
     * @return ShotInstruction
     */
    static ShotInstruction FireSpiral(int slot, int count, float speed, float deltaAngle) {
        ShotInstruction i{};
        i.type = Type::FireSpread; // FireSpread で deltaAngle 変化は制御側実装依存
        i.slot = slot;
        i.count = count;
        i.speed = speed;
        i.angle2 = deltaAngle;
        return i;
    }

    /**
     * @brief 波状に弾を発射（中央から交互にぶれる）
     * @param slot スロット番号（未使用なら -1）
     * @param count 弾数
     * @param baseSpeed 中央弾の速度
     * @param speedDelta 外周に向かう速度差
     * @return ShotInstruction
     */
    static ShotInstruction FireWave(int slot, int count, float baseSpeed, float speedDelta) {
        ShotInstruction i{};
        i.type = ShotInstruction::Type::FireMultiSpeed;
        i.slot = slot;
        i.angle = 90.f; // 基本真下
        i.count = count;
        for (int j = 0; j < count; ++j) {
            float s = baseSpeed + (j - count / 2.0f) * speedDelta;
            i.speeds.push_back(s);
        }
        return i;
    }

    /**
     * @brief 発射角度にランダムノイズを設定する命令を作成
     * @param maxAngleDeviationDegrees 最大角度ぶれ（度）
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction RandomizeAngle(float maxAngleDeviationDegrees) {
        ShotInstruction i{};
        i.type = Type::RandomizeAngle;
        i.maxOffset = maxAngleDeviationDegrees;
        return i;
    }

    /**
     * @brief ループ開始命令を作成
     * @param loopCount ループ回数
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction LoopStart(int loopCount) {
        ShotInstruction i{};
        i.type = Type::LoopStart;
        i.count = loopCount;
        return i;
    }

    /**
     * @brief ループ終了命令を作成
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction LoopEnd() {
        return { Type::LoopEnd };
    }

    /**
     * @brief 指定フレーム数だけ待機する命令を作成
     * @param waitFrames 待機フレーム数
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction Wait(int waitFrames) {
        ShotInstruction i{};
        i.type = Type::Wait;
        i.count = waitFrames;
        return i;
    }

    /**
     * @brief スクリプト終了命令を作成
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction End() {
        return { Type::End };
    }

    /***
    * @brief 弾スクリプトを設定する命令を作成（未使用・拡張用）
    * @param type 弾スクリプトのタイプ名
    * @return ShotInstruction 命令インスタンス
    */
    static ShotInstruction SetBulletScript(std::string type) {
        ShotInstruction i{};
        i.type = Type::SetBulletScript;
        i.label = type;
        return i;
    }


    /**
     * @brief 敵タイプ設定命令を作成（未使用・拡張用）
     * @param slot スロット番号
     * @param templateId 弾テンプレートID
     * @param colorId 弾色ID
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction SetEnemyType(int slot, int templateId, int colorId) {
        ShotInstruction i{};
        i.type = Type::SetEnemyType;
        i.slot = slot;
        i.count = templateId;
        i.extra = colorId;
        return i;
    }

    /**
     * @brief 発射時エフェクト再生命令を作成
     * @param effectId エフェクトID
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction EmitEffect(int effectId) {
        ShotInstruction i{};
        i.type = Type::EmitEffect;
        i.slot = effectId;
        return i;
    }

    /**
     * @brief 発射速度加速命令を作成（簡易版）
     * @param slot スロット番号
     * @param waitType 待機タイプ（未実装）
     * @param duration 加速継続フレーム数
     * @param deltaSpeed 速度変化量
     * @param polarity 加速の方向（true: 加速, false: 減速）
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction EmitSpeedUp(int slot, int waitType, int duration, float deltaSpeed, bool polarity) {
        ShotInstruction i{};
        i.type = Type::EmitSpeedUp;
        i.slot = slot;
        i.count = waitType;
        i.extra = duration;
        i.speed = deltaSpeed;
        i.angle2 = polarity ? 1.f : 0.f;
        return i;
    }

    /**
     * @brief 発射遅延命令を作成（簡易版）
     * @param slot スロット番号
     * @param duration 遅延フレーム数
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction EmitDelay(int slot, int duration) {
        ShotInstruction i{};
        i.type = Type::EmitDelay;
        i.slot = slot;
        i.count = duration;
        return i;
    }

    /**
     * @brief 発射位置オフセット設定命令を作成
     * @param slot スロット番号
     * @param offsetX X方向オフセット
     * @param offsetY Y方向オフセット
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction Offset(int slot, float offsetX, float offsetY) {
        ShotInstruction i{};
        i.type = Type::Offset;
        i.slot = slot;
        i.angle = offsetX;
        i.speed = offsetY;
        return i;
    }

    /**
     * @brief 弾の角速度設定命令を作成
     * @param angularVelocityDegreesPerFrame 角速度（度／フレーム）
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction Rotate(float angularVelocityDegreesPerFrame) {
        ShotInstruction i{};
        i.type = Type::Rotate;
        i.angle = angularVelocityDegreesPerFrame;
        return i;
    }

    /**
     * @brief 弾の加速度設定命令を作成
     * @param accelerationPerFrame 加速度（速度変化量／フレーム）
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction Accelerate(float accelerationPerFrame) {
        ShotInstruction i{};
        i.type = Type::Accelerate;
        i.speed = accelerationPerFrame;
        return i;
    }

    /**
     * @brief 基準角度に加算する命令を作成
     * @param deltaAngle 加算する角度（度）
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction AddBaseAngle(float deltaAngle) {
        ShotInstruction i{};
        i.type = Type::AddBaseAngle;
        i.angle = deltaAngle;
        return i;
    }

    /**
     * @brief 発射角度を絶対指定する命令を作成
     * @param angle 発射角度（度）
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction SetAngle(float angle) {
        ShotInstruction i{};
        i.type = Type::SetAngle;
        i.angle = angle;
        return i;
    }

    /**
     * @brief プレイヤー方向に向ける.
     * @return ShotInstruction 命令インスタンス
     */
    static ShotInstruction SetPlayerAngle() {
        ShotInstruction i{};
        i.type = Type::SetPlayerAngle;
        return i;
    }

    /**
    * @brief サウンド（SE）を再生する命令を作成
    * @param soundName 再生するサウンド名
    * @param volume     音量（0.0-1.0）
    * @return ShotInstruction 命令インスタンス
    */
    static ShotInstruction SetPlayerSE(std::string soundName, float volume = 1.0f) {
        ShotInstruction i{};
        i.type = Type::PlaySE;
        i.soundName = soundName;
        i.volume = volume;
        return i;
    }
};
