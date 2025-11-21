#include "ScoreManager.h"

void ScoreManager::Add(int64_t value) {
    currentScore += value;
    if (currentScore > kMaxBaseScore)
        currentScore = kMaxBaseScore;
    if (currentScore > hiScore) {
        hiScore = currentScore;
        SaveHiScore();
    }

}

int64_t ScoreManager::GetScore() const {
    return currentScore;
}

int64_t ScoreManager::GetHiScore() const {
    return hiScore;
}

int64_t ScoreManager::GetScorePerItem() const {
    return baseScorePerItem;
}

void ScoreManager::Reset() {
    currentScore = 0;
    baseScorePerItem = 0;
}

void ScoreManager::OnGraze(int grazeCount) {
    baseScorePerItem += grazeCount;
    ClampBaseScore();
}

void ScoreManager::OnBombClear(int bulletsCleared) {
    baseScorePerItem += bulletsCleared * 2;
    ClampBaseScore();
}

void ScoreManager::OnEnemyHit() {
    Add(kScorePerHit);
}

void ScoreManager::LoadHiScore() {
    if (PlayerPrefs::HasKey(kHiScoreKey)) {
        std::string str = PlayerPrefs::GetString(kHiScoreKey, "0");
        try {
            hiScore = std::stoll(str);
        }
        catch (...) {
            hiScore = 0;
        }
    }
    else {
        hiScore = 0;
    }
}

void ScoreManager::SaveHiScore() {
    PlayerPrefs::SetString(kHiScoreKey, std::to_string(hiScore));
}

void ScoreManager::ClampBaseScore() {
    if (baseScorePerItem > kMaxBaseScorePerItem)
        baseScorePerItem = kMaxBaseScorePerItem;
}
