#pragma once

class LifeManager {
private:
    const int maxLives = 7;
    const int fragmentsToLife = 3; // 例：欠片3つで残機+1
    const int continueMaxLife = 3; // 例：コンテニュー3;

    int continueLife = continueMaxLife;
    int life = 3;
    int lifeFragments = 0;

public:
    void AddLives(int value);
    void AddFragments(int value);  // 欠片追加用
    void OnDeath();
    int GetLife() const;
    int GetFragments() const;
    int GetFragmentsToLife() const;
    void Reset();
    void Continue();
    bool IsContinueCheck();
};