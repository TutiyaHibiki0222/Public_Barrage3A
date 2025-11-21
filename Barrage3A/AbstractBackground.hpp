#pragma once

#include "Dx3DCamera.h"
#include "IDraw.h"

class AbstractBackground {
public:
    virtual ~AbstractBackground() = default;
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
};


struct FogInstruction {
    enum class Type {
        SetColor,
        SetRange,
        LerpColor,
        LerpRange,
        LerpColorAndRange,
        Wait,
        End
    };

    Type type;
    int duration = 0;
    int frame = 0;

    Color colorStart;
    Color colorEnd;

    float nearStart = 0.0f;
    float nearEnd = 0.0f;
    float farStart = 0.0f;
    float farEnd = 0.0f;
    // ====== Static Factory Methods ====== //

    static FogInstruction SetColor(const Color& color) {
        FogInstruction i{};
        i.type = Type::SetColor;
        i.colorEnd = color;
        return i;
    }

    static FogInstruction SetRange(float nearVal, float farVal) {
        FogInstruction i{};
        i.type = Type::SetRange;
        i.nearEnd = nearVal;
        i.farEnd = farVal;
        return i;
    }

    static FogInstruction LerpColor(const Color& from, const Color& to, int duration) {
        FogInstruction i{};
        i.type = Type::LerpColor;
        i.colorStart = from;
        i.colorEnd = to;
        i.duration = duration;
        return i;
    }

    static FogInstruction LerpRange(float nearFrom, float farFrom, float nearTo, float farTo, int duration) {
        FogInstruction i{};
        i.type = Type::LerpRange;
        i.nearStart = nearFrom;
        i.farStart = farFrom;
        i.nearEnd = nearTo;
        i.farEnd = farTo;
        i.duration = duration;
        return i;
    }
    /**
     * @brief 色とフォグの範囲（near/far）を同時に線形補間する命令を作成する
     *
     * @param fromColor 補間開始時の色（Color型）
     * @param toColor 補間終了時の色（Color型）
     * @param nearFrom 補間開始時のフォグのnear距離
     * @param farFrom 補間開始時のフォグのfar距離
     * @param nearTo 補間終了時のフォグのnear距離
     * @param farTo 補間終了時のフォグのfar距離
     * @param duration 補間にかけるフレーム数（0以下の場合は即時変更扱い）
     * @return FogInstruction LerpColorAndRangeタイプのFogInstruction構造体
    */
    static FogInstruction LerpColorAndRange(
        const Color& fromColor, const Color& toColor,
        float nearFrom, float farFrom, float nearTo, float farTo,
        int duration)
    {
        FogInstruction i{};
        i.type = Type::LerpColorAndRange;
        i.colorStart = fromColor;
        i.colorEnd = toColor;
        i.nearStart = nearFrom;
        i.farStart = farFrom;
        i.nearEnd = nearTo;
        i.farEnd = farTo;
        i.duration = duration;
        return i;
    }

    static FogInstruction Wait(int duration) {
        FogInstruction i{};
        i.type = Type::Wait;
        i.duration = duration;
        return i;
    }

    static FogInstruction End() {
        return { Type::End };
    }
};

class FogScript {
private:
    std::vector<FogInstruction> instructions;
    int ip = 0;
    bool finished = false;
public:
    void Add(const FogInstruction& instr) {
        instructions.push_back(instr);
    }

    void Update() {
        if (finished || ip >= (int)instructions.size()) return;

        FogInstruction& instr = instructions[ip];
        instr.frame++;

        switch (instr.type) {
        case FogInstruction::Type::SetColor:
            Dx3DCamera::Instance().SetFogColor(
                instr.colorEnd.R255(),
                instr.colorEnd.G255(),
                instr.colorEnd.B255()
            );
            ip++;
            break;

        case FogInstruction::Type::SetRange:
            Dx3DCamera::Instance().SetFogRange(instr.nearEnd, instr.farEnd);
            ip++;
            break;

        case FogInstruction::Type::LerpColor: {
            float t = std::clamp(float(instr.frame) / instr.duration, 0.0f, 1.0f);
            Color c = Color::Lerp(instr.colorStart, instr.colorEnd, t);
            Dx3DCamera::Instance().SetFogColor(c.R255(), c.G255(), c.B255());
            if (instr.frame >= instr.duration) ip++;
            break;
        }

        case FogInstruction::Type::LerpRange: {
            float t = std::clamp(float(instr.frame) / instr.duration, 0.0f, 1.0f);
            float nearVal = Mathf::Lerp(instr.nearStart, instr.nearEnd, t);
            float farVal = Mathf::Lerp(instr.farStart, instr.farEnd, t);
            Dx3DCamera::Instance().SetFogRange(nearVal, farVal);
            if (instr.frame >= instr.duration) ip++;
            break;
        }

        case FogInstruction::Type::LerpColorAndRange: {
            float t = std::clamp(float(instr.frame) / instr.duration, 0.0f, 1.0f);
            Color c = Color::Lerp(instr.colorStart, instr.colorEnd, t);
            Dx3DCamera::Instance().SetFogColor(c.R255(), c.G255(), c.B255());

            float nearVal = Mathf::Lerp(instr.nearStart, instr.nearEnd, t);
            float farVal = Mathf::Lerp(instr.farStart, instr.farEnd, t);
            Dx3DCamera::Instance().SetFogRange(nearVal, farVal);

            if (instr.frame >= instr.duration) ip++;
            break;
        }

        case FogInstruction::Type::Wait:
            if (instr.frame >= instr.duration) ip++;
            break;

        case FogInstruction::Type::End:
            finished = true;
            break;
        }
    }

    bool IsFinished() const {
        return finished;
    }

    void Reset() {
        ip = 0;
        finished = false;
        for (auto& instr : instructions) {
            instr.frame = 0;
        }
    }
};
