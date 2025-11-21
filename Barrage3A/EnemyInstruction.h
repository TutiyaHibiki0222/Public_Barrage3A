#pragma once
#include "Vector.h"
#include "ItemBase.h"

struct EnemyInstruction {
    enum class Type {
        // 初期化類.
        SetHp,            // 体力設定.
        SetHitboxEnable,  // 当たり判定の大きさ.
        SetDropItem,      // 落とすアイテム設定.
        SetAngle,         // デフォルト向き.
        Die,              // 死亡.
        // かく処理類.
        SetAccelerationPattern,  ///< 一定時間、加速度で速度を変化
        OscillateX,              ///< x軸に振動
        OscillateY,              ///< y軸に振動
        HomingPlayer,            ///< プレイヤー方向に角度変化で追尾
        FollowPath,              ///< カーブ/軌道移動（IDで制御）
        SetUpdateFunction,       ///< フレーム更新関数登録
        MoveTo,        ///< 指定位置へ一定時間かけて直線移動
        Wait,          ///< 一時停止
        Teleport,      ///< 瞬間移動
        Fire,          ///< ShotScriptを発射
        SetVelocity,   ///< 敵の速度を即時設定
        Accelerate,    ///< 敵の加速度を設定
        Rotate,        ///< 敵の角度を変化（角速度指定も可）
        LoopStart,     ///< ループ開始
        LoopEnd,       ///< ループ終了
        CallSubroutine,///< サブルーチン呼び出し
        SetEnemyType,  ///< 弾テンプレートや色指定
        EmitEffect,    ///< エフェクト発生
        Offset,        ///< 発射位置オフセット設定
        RandomMove,   // 指定矩形内をランダムに移動（ターゲットを一定間隔で切替）
        RandomWander, // 小さな揺れ（ゆらぎ）を継続的に行う
        End,           ///< スクリプト終了
    };

    Type type;
    Vector2D position{};        ///< 移動先座標など
    Vector2D vector{};          ///< 速度や加速度
    float angle = 0.f;          ///< 角度や回転速度
    int duration = 0;           ///< フレーム数、ループ回数など
    int extra = 0;              ///< 追加パラメータ（テンプレID等）
    int loopCount = 0;          ///< ループ回数
    std::string shotScriptID;   ///< FireやCallSubroutine用のID

    // ====== Static Factory Methods ====== //
    /**
     * @brief 敵のHPを設定する命令
     * @param hp 初期HP
     * @return EnemyInstructionのSetHp命令
     */
    static EnemyInstruction SetHp(int hp) {
        EnemyInstruction i{};
        i.type = Type::SetHp;
        i.extra = hp;
        return i;
    }
    /**
    * @brief 当たり判定の有効/無効を設定する命令
    * @param enable trueなら有効, falseなら無効
    * @return EnemyInstructionのSetHitboxEnable命令
    */
    static EnemyInstruction SetHitboxEnable(bool enable) {
        EnemyInstruction i{};
        i.type = Type::SetHitboxEnable;
        i.extra = enable ? 1 : 0;
        return i;
    }
    /**
    * @brief 敵が倒されたときに落とすアイテムIDを設定
    * @param itemId ドロップするアイテムID
    * @return EnemyInstructionのSetDropItem命令
    */
    static EnemyInstruction AddDropItem(ItemType type, int quantity) {
        EnemyInstruction i{};
        i.type = Type::SetDropItem;
        i.extra = static_cast<int>(type);
        i.duration = quantity;
        return i;
    }
    /**
    * @brief 敵の角度を設定
    * @param angle 角度（度）
    * @return EnemyInstructionのSetAngle命令
    */
    static EnemyInstruction SetAngle(float angle) {
        EnemyInstruction i{};
        i.type = Type::SetAngle;
        i.angle = angle;
        return i;
    }
    /**
    * @brief 敵を即死させる命令
    * @return EnemyInstructionのDie命令
    */
    static EnemyInstruction Die() {
        return { Type::Die };
    }

    /**
     * @brief 指定座標まで一定時間かけて移動（直線補間）
     * @param to 移動先の座標
     * @param frames 移動にかけるフレーム数
     * @return EnemyInstructionのMoveTo命令
     */
    static EnemyInstruction MoveTo(const Vector2D& to, int frames) {
        return { Type::MoveTo, to, {}, 0.f, frames };
    }

    /**
     * @brief 指定フレーム数だけ待機
     * @param frames 待機時間（フレーム）
     * @return EnemyInstructionのWait命令
     */
    static EnemyInstruction Wait(int frames) {
        return { Type::Wait, {}, {}, 0.f, frames };
    }

    /**
     * @brief 即座に指定座標へテレポート
     * @param to テレポート先の座標
     * @return EnemyInstructionのTeleport命令
     */
    static EnemyInstruction Teleport(const Vector2D& to) {
        return { Type::Teleport, to };
    }

    /**
     * @brief ShotScriptを発射する命令
     * @param shotID 発射パターンID
     * @return EnemyInstructionのFire命令
     */
    static EnemyInstruction Fire(const std::string& shotID) {
        EnemyInstruction i{};
        i.type = Type::Fire;
        i.shotScriptID = shotID;
        return i;
    }

    /**
     * @brief 敵の速度を即時設定
     * @param v 新しい速度ベクトル
     * @return EnemyInstructionのSetVelocity命令
     */
    static EnemyInstruction SetVelocity(const Vector2D& v) {
        return { Type::SetVelocity, {}, v };
    }

    /**
     * @brief 敵の加速度を設定し徐々に速度を変化させる
     * @param a 加速度ベクトル
     * @return EnemyInstructionのAccelerate命令
     */
    static EnemyInstruction Accelerate(const Vector2D& a) {
        return { Type::Accelerate, {}, a };
    }

    /**
     * @brief 敵の角度を変更する命令（角速度指定も可能）
     * @param deltaAngle 角度変化量（度/フレームなど）
     * @param frames 角度変化を行うフレーム数（デフォルト1フレーム）
     * @return EnemyInstructionのRotate命令
     */
    static EnemyInstruction Rotate(float deltaAngle, int frames = 1) {
        return { Type::Rotate, {}, {}, deltaAngle, frames };
    }

    /**
     * @brief ループ開始命令
     * @param count 繰り返し回数
     * @return EnemyInstructionのLoopStart命令
     */
    static EnemyInstruction LoopStart(int count) {
        EnemyInstruction i{};
        i.type = Type::LoopStart;
        i.loopCount = count;
        return i;
    }

    /**
     * @brief ループ終了命令
     * @return EnemyInstructionのLoopEnd命令
     */
    static EnemyInstruction LoopEnd() {
        return { Type::LoopEnd };
    }

    /**
     * @brief サブルーチン呼び出し命令
     * @param label 呼び出すサブルーチンのラベル名
     * @return EnemyInstructionのCallSubroutine命令
     */
    static EnemyInstruction CallSubroutine(const std::string& label) {
        EnemyInstruction i{};
        i.type = Type::CallSubroutine;
        i.shotScriptID = label;
        return i;
    }

    /**
     * @brief 敵弾のテンプレートや色を設定する命令
     * @param templateId 弾テンプレートID
     * @param colorId 弾の色ID
     * @return EnemyInstructionのSetEnemyType命令
     */
    static EnemyInstruction SetEnemyType(int templateId, int colorId) {
        EnemyInstruction i{};
        i.type = Type::SetEnemyType;
        i.extra = templateId;
        i.duration = colorId;
        return i;
    }

    /**
     * @brief エフェクトを発生させる命令
     * @param effectId エフェクトID
     * @return EnemyInstructionのEmitEffect命令
     */
    static EnemyInstruction EmitEffect(int effectId) {
        EnemyInstruction i{};
        i.type = Type::EmitEffect;
        i.extra = effectId;
        return i;
    }

    /**
     * @brief 発射位置のオフセットを設定する命令
     * @param slot スロット番号
     * @param x オフセットX
     * @param y オフセットY
     * @return EnemyInstructionのOffset命令
     */
    static EnemyInstruction Offset(int slot, float x, float y) {
        EnemyInstruction i{};
        i.type = Type::Offset;
        i.extra = slot;
        i.position = { x, y };
        return i;
    }


    static EnemyInstruction SetAccelerationPattern(const Vector2D& accel, int duration) {
        return { Type::SetAccelerationPattern, {}, accel, 0.f, duration };
    }

    static EnemyInstruction OscillateX(float amplitude, float period, int duration) {
        EnemyInstruction i{};
        i.type = Type::OscillateX;
        i.vector = { amplitude, period };  // x: 振幅, y: 周期
        i.duration = duration;
        return i;
    }

    /**
    * @brief y軸方向にsin波で振動する命令
    * @param amplitude 振幅（上下移動の大きさ）
    * @param period 周期（何フレームで一周するか）
    * @param duration 継続フレーム数
    * @return EnemyInstructionのOscillateY命令
    */
    static EnemyInstruction OscillateY(float amplitude, float period, int duration) {
        EnemyInstruction i{};
        i.type = Type::OscillateY;
        i.vector = { amplitude, period };  // vector.x = 振幅, vector.y = 周期
        i.duration = duration;
        return i;
    }

    static EnemyInstruction HomingPlayer(float angularSpeedDeg, int duration) {
        EnemyInstruction i{};
        i.type = Type::HomingPlayer;
        i.angle = angularSpeedDeg;
        i.duration = duration;
        return i;
    }

    static EnemyInstruction FollowPath(int pathId, int duration) {
        EnemyInstruction i{};
        i.type = Type::FollowPath;
        i.extra = pathId;
        i.duration = duration;
        return i;
    }

    static EnemyInstruction SetUpdateFunction(const std::string& funcId) {
        EnemyInstruction i{};
        i.type = Type::SetUpdateFunction;
        i.shotScriptID = funcId;  // 関数名やラベル
        return i;
    }

    // center: 中心座標、radiusX/ radiusY: 中心からの振れ幅、duration: 全体継続フレーム(0=無限)、changeInterval: ターゲット切替間隔(フレーム)
    static EnemyInstruction RandomMove(const Vector2D& center, float radiusX, float radiusY, int duration, int changeInterval = 60) {
        EnemyInstruction i{};
        i.type = Type::RandomMove;
        i.position = center;         // 中心
        i.vector = { radiusX, radiusY }; // 振れ幅
        i.duration = duration;
        i.extra = changeInterval;    // 切替間隔
        return i;
    }

    // amplitudeX/Y: 揺れの最大幅、changeInterval: 新しい目標への補間時間(フレーム)、duration: 全体継続
    static EnemyInstruction RandomWander(float amplitudeX, float amplitudeY, int changeInterval = 30, int duration = 0) {
        EnemyInstruction i{};
        i.type = Type::RandomWander;
        i.vector = { amplitudeX, amplitudeY };
        i.duration = duration;
        i.extra = changeInterval;
        return i;
    }

    /**
     * @brief スクリプト終了命令
     * @return EnemyInstructionのEnd命令
     */
    static EnemyInstruction End() {
        return { Type::End };
    }
};

