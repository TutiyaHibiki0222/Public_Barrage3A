#pragma once

#include "PlayerPrefs.h"
#include <cstdint>

class ScoreManager {
private:
    static constexpr int64_t kScorePerHit = 20;
    static constexpr int64_t kMaxBaseScore = 9'999'999'990LL;
    static constexpr int64_t kMaxBaseScorePerItem = 999'999'999LL;
    static constexpr const char* kHiScoreKey = "HiScore";

    int64_t currentScore = 0;
    int64_t hiScore = 0;
    int64_t baseScorePerItem = 0;

    void ClampBaseScore();

public:
    void Add(int64_t value);
    int64_t GetScore() const;
    int64_t GetHiScore() const;
    int64_t GetScorePerItem() const;

    void Reset();

    void OnGraze(int grazeCount);
    void OnBombClear(int bulletsCleared);
    void OnEnemyHit();

    void LoadHiScore();
    void SaveHiScore();
};
