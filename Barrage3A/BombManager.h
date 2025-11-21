#pragma once

class BombManager {
private:
    int bombs = 3;                 // 現在保持ボム数
    int bombFragments = 0;         // 現在保持欠片数
    int maxFragments = 3;          // 最大欠片数（欠片がこれ以上たまらない）
    int maxBombs = 8;              // 最大スペルカード数（ボムの最大値）

public:
    BombManager() = default;

    // ボムを直接増減（負値もOK、ただし0未満にはしない）
    void AddBombs(int value);

    // 欠片を増やす。一定数たまったらボム増加
    void AddFragments(int count);

    // ボムを1つ消費（使う）
    bool UseBomb();

    // 現在のボム数を取得
    int GetBombs() const;

    // 現在の欠片数を取得
    int GetFragments() const;

    // 最大欠片数を取得
    int GetMaxFragments() const;

    // 最大ボム数（スペルカード数）を取得
    int GetMaxBombs() const;

    // 初期化
    void Reset();

    // 欠片が最大に達しているか
    bool IsFragmentsMax() const;

    // ボム使えるか.
    bool IsBom() const;

    void ResetOnRespawn();
};
