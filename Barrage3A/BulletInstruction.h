#pragma once
#include "Vector.h"
#include "BulletType.h"

/**
 * @brief 命令ベースの弾挙動制御データ構造
 *
 * 弾の速度設定、加速、回転、ホーミング、分裂など
 * 多彩な動きを BulletScript で組み立て可能にする。
 */
struct BulletInstruction {
    enum class Type {
        // 弾自体の設定を変更.
        SetBulletType,  ///< 弾のタイプを設定.
		// 命令タイプ.
        SetVelocity,    ///< 速度と角度を指定して移動開始
        Accelerate,     ///< 一定時間加速または減速
        Rotate,         ///< 回転し続ける
        Wait,           ///< 何もせず停止
        LerpVelocity,   ///< 現在の速度から目標速度へ線形補間
        RotateTo,       ///< 現在の進行角を一定時間で目標角度へ向ける
        AddRotateTo,    ///< 現在の角度から相対的に回転する.
        AimAtPlayer,    ///< 発射時に自機を狙う角度をセット
        Seek,           ///< 一定時間ホーミング（途中で追尾）
        RandomizeAngle, ///< 角度にランダムばらつきを付与
        Branch,         ///< 分裂して複数の子弾を発射
        DelayedFire,    ///< 指定フレーム後にもう一度発射
        Wave,           ///< 進行方向に対してサイン波的に蛇行
        Zigzag,         ///< 直線往復ジグザグ
        PlaySE,         ///< SEを鳴らす.
        LoopStart,      ///< ループ開始（回数指定）
        LoopEnd,        ///< ループ終了
        End,            ///< スクリプト終了
    };

    Type type;          ///< 命令タイプ
    float value1 = 0.f; ///< 汎用パラメータ1 (速度、加速度、角度、振幅など)
    float value2 = 0.f; ///< 汎用パラメータ2 (補助角度、周波数など)
    int duration = 0;   ///< 継続フレーム数 (加速時間、待機時間、周期など)
    std::string soundName = "";
    float       volume = 1.0f;

    BulletParentID parentID = BulletParentID::Count;
    BulletColor    color    = BulletColor::Red;

    /**
    * @brief 弾自体のタイプを変更する.
    * @param id     弾の実体定義
    * @param clo    弾の色
    * @return 設定済み命令オブジェクト
    */
    static BulletInstruction SetBulletType(BulletParentID id, BulletColor clo) {
        BulletInstruction i{ Type::SetBulletType };
		i.parentID = id;
        i.color = clo;
		return i;
    }

    /**
     * @brief 指定速度と角度で移動を開始
     * @param speed  移動速度
     * @param angle  移動角度 (度)
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction SetVelocity(float speed, float angle) {
        return { Type::SetVelocity, speed, angle };
    }

    /**
     * @brief 一定時間かけて加速度を加える
     * @param accel    加速度（正=加速、負=減速）
     * @param duration 適用フレーム数
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction Accelerate(float accel, int duration) {
        return { Type::Accelerate, accel, 0.f, duration };
    }

    /**
     * @brief 毎フレーム、進行方向を回転させ続ける
     * @param angularVelocity 毎フレームの角度変化量 (度/フレーム)
     * @param duration         継続フレーム数
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction Rotate(float angularVelocity, int duration) {
        return { Type::Rotate, angularVelocity, 0.f, duration };
    }

    /**
     * @brief 一定フレーム何もしない
     * @param frames 待機フレーム数
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction Wait(int frames) {
        return { Type::Wait, 0.f, 0.f, frames };
    }
    /**
    * @brief 現在速度から目標速度へ一定時間かけて補間
    * @param targetSpeed 目標速度
    * @param duration    補間にかけるフレーム数
    */
    static BulletInstruction LerpVelocity(float targetSpeed, int duration) {
        return { Type::LerpVelocity, targetSpeed, 0.f, duration };
    }

    /**
    * @brief 現在角度から目標角度へ一定時間かけて補間
    * @param targetAngle 目標角度（deg）
    * @param duration    補間にかけるフレーム数
    */
    static BulletInstruction RotateTo(float targetAngle, int duration) {
        return { Type::RotateTo, targetAngle, 0.f, duration };
    }

    /**
    * @brief 現在の角度から相対的に回転させる命令を作成
    * @param deltaAngle 追加回転角度 (度)
    * @param duration   継続フレーム数
    * @return 設定済み命令オブジェクト
    */
    static BulletInstruction AddRotateTo(float deltaAngle, int duration) {
        return { Type::AddRotateTo, deltaAngle, 0.f, duration };
    }

    /**
     * @brief 発射時に自機の方向を即座に狙う
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction AimAtPlayer() {
        return { Type::AimAtPlayer };
    }

    /**
     * @brief 一定時間、ホーミング (追尾) させる
     * @param homingAccel ホーミング加速度 (速度への寄せ)
     * @param duration    適用フレーム数
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction Seek(float homingAccel, int duration) {
        return { Type::Seek, homingAccel, 0.f, duration };
    }

    /**
     * @brief 進行角度にランダムばらつきを一度だけ付与
     * @param maxDeviation 最大偏差 (度)
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction RandomizeAngle(float maxDeviation) {
        return { Type::RandomizeAngle, maxDeviation };
    }

    /**
     * @brief 分裂して複数の子弾を同時発射
     * @param childCount  子弾数
     * @param spreadAngle 全体の扇形開き (度)
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction Branch(int childCount, float spreadAngle) {
        return { Type::Branch, float(childCount), spreadAngle };
    }

    /**
     * @brief 指定フレーム後に再度 Fire 処理を呼ぶ
     * @param afterFrames 遅延フレーム数
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction DelayedFire(int afterFrames) {
        return { Type::DelayedFire, 0.f, 0.f, afterFrames };
    }

    /**
     * @brief サイン波的な波打ち移動を行う
     * @param amplitude 蛇行振幅
     * @param frequency 周期数 (1 秒あたり?)
     * @param duration  継続フレーム数
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction Wave(float amplitude, float frequency, int duration) {
        BulletInstruction i{ Type::Wave };
        i.value1 = amplitude;
        i.value2 = frequency;
        i.duration = duration;
        return i;
    }

    /**
     * @brief 直線往復 (ジグザグ) 移動を行う
     * @param angleDelta 折り返し時の角度オフセット (deg)
     * @param period     往復周期フレーム数
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction Zigzag(float angleDelta, int period) {
        BulletInstruction i{ Type::Zigzag };
        i.value1 = angleDelta;
        i.duration = period;
        return i;
    }

    /**
     * @brief スクリプトの終了命令
     * @return 設定済み命令オブジェクト
     */
    static BulletInstruction End() {
        return { Type::End };
    }

    /**
    * @brief サウンド（SE）を再生する命令を作成
    * @param soundName 再生するサウンド名
    * @param volume     音量（0.0-1.0）
    * @return BulletInstruction 命令インスタンス
    */
    static BulletInstruction SetPlayerSE(std::string soundName, float volume = 1.0f) {
        BulletInstruction i{};
        i.type = Type::PlaySE;
        i.soundName = soundName;
        i.volume = volume;
        return i;
    }

    /**
    * @brief ループ開始命令を作成
    * @param count ループ回数
    * @return 命令インスタンス
    */
    static BulletInstruction LoopStart(int count) {
        return { Type::LoopStart, float(count) };
    }

    /**
    * @brief ループ終了命令を作成
    * @return 命令インスタンス
    */
    static BulletInstruction LoopEnd() {
        return { Type::LoopEnd };
    }
};
