#pragma once
#include "BulletControllerBase.h"
#include "BulletInstruction.h"
#include <stack>

class BulletScript : public BulletControllerBase
{
private:
    std::vector<BulletInstruction> instructions;  ///< 命令列
    size_t ip = 0;          ///< 実行中の命令インデックス
    int    frame = 0;       ///< 命令ごとのフレームカウンタ
    float  speed = 0.f;     ///< 現在の移動速度
    float  angle = 0.f;     ///< 現在の移動角度 (deg)
    
    struct LoopContext {
        int startIp;
        int remaining;

        LoopContext(int start, int count)
            : startIp(start), remaining(count) {
        }
    };

    std::stack<LoopContext> loopStack;

    float lerpStartSpeed = 0.0f;
    float lerpStartAngle = 0.0f;
    float targetAngle = 0.f;
    float lastWaveOffset = 0.f;
    float baseAngle = 0.f;
    bool lerpInit = false;
public:
    BulletScript() {};
    /**
    * @brief 命令を一つ追加します
    * @param instr BulletInstruction オブジェクト
    */
    void AddInstruction(const BulletInstruction& instr) {
        instructions.push_back(instr);
    }

    void Update(std::shared_ptr<Transform2D>, BulletBase*) override;

    void InitSpeed(float _spped) {
        speed = _spped;
    }

    void InitAngle(float _angle) {
        angle = _angle;
    }

    void Reset() {
        ip = 0;
        frame = 0;
        speed = 0.f;
        angle = 0.f;
        lerpInit = false;
        lerpStartSpeed = 0.f;
        lerpStartAngle = 0.f;
    }

    std::shared_ptr<BulletScript> Clone() {
        auto bullet = std::make_shared<BulletScript>(*this);
        bullet->Reset();
        return bullet;
    }

};
