#include "LifeManager.h"

void LifeManager::AddLives(int value) {
    life += value;
    if (life > maxLives) life = maxLives;
    if (life < 0) life = 0;
}

void LifeManager::AddFragments(int value) {
    lifeFragments += value;

    // —­‚Ü‚Á‚½Œ‡•Ð‚ÅŽc‹@‰ñ•œ
    while (lifeFragments >= fragmentsToLife) {
        lifeFragments -= fragmentsToLife;
        AddLives(1);
    }
}

void LifeManager::OnDeath() {
    if (life >= 0) {
        life--;
    }
}

int LifeManager::GetLife() const {
    return life;
}

int LifeManager::GetFragments() const {
    return lifeFragments;
}

int LifeManager::GetFragmentsToLife() const {
    return fragmentsToLife;
}

void LifeManager::Reset() {
    life = 2;
    continueLife = continueMaxLife;
    lifeFragments = 0;
}

void LifeManager::Continue()
{
    life = 2;
    lifeFragments = 0;
    continueLife--;
}

bool LifeManager::IsContinueCheck() {
    return continueLife > 0;
}