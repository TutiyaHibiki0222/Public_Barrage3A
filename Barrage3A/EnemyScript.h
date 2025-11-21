#pragma once

#include "EnemyInstruction.h"
#include "Mathf.h"
#include <stack>

class Enemy;  // 前方宣言

struct DropSettings {
    int powerPieceCount = 0;    // 通常のP
    int valueItemCount = 0;     // 点アイテム
    int lifePieceCount = 0;     // 残機欠片
    int bombPieceCount = 0;     // ボム欠片
    int fillLifeItem   = 0;     // フル残機1=落とす, 0=落とさない
    int fullPowerItem  = 0;     // フルパワー1=落とす, 0=落とさない
    bool guaranteedDrop = true; // 落とすことが保証されるか
};

class EnemyScript {
private:
    std::vector<EnemyInstruction> instructions;
    int ip = 0;
    int frame = 0;
    Vector2D startPos;
    bool finished = false;

    std::unordered_map<std::string, int> labelMap;
    // 命令ごとの補助ステート（ip をキー）
    std::unordered_map<int, int> instrCounters;        // 汎用カウンタ（総経過フレームなど）
    std::unordered_map<int, float> instrFloats;       // 汎用フロート（必要なら）
    std::unordered_map<int, Vector2D> instrPositions; // 現在のターゲット座標など

    // ループ制御
    std::vector<std::pair<int, int>> loopStack; // <ループ開始命令のip, 残りループ回数>
    std::stack<int> callStack;

    void Advance();
    int  FindLabel(const std::string& labelName) const;
public:
    std::shared_ptr<EnemyScript> Clone() const {
        auto clone = std::make_shared<EnemyScript>();
        clone->instructions = this->instructions;
        // 状態は開始時の状態にリセット
        clone->ip = 0;
        clone->frame = 0;
        clone->finished = false;
        clone->startPos = this->startPos;
        clone->labelMap = this->labelMap;
        clone->loopStack.clear();
        while (!clone->callStack.empty()) clone->callStack.pop();
        return clone;
    }
    void Add(const EnemyInstruction& instr);
    void Update(Enemy& enemy);
    bool IsFinished() const;
};