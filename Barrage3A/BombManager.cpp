#include "BombManager.h"

void BombManager::AddBombs(int value) {
    bombs += value;
    if (bombs < 0) bombs = 0;
    if (bombs > maxBombs) bombs = maxBombs;
}

void BombManager::AddFragments(int count) {
    if (count <= 0) return;

    bombFragments += count;
    if (bombFragments > maxFragments) bombFragments = maxFragments;

    while (bombFragments >= maxFragments) {
        bombFragments -= maxFragments;
        if (bombs < maxBombs) {
            bombs++;
        }
        else {
            bombFragments = maxFragments;
            break;
        }
    }
}

bool BombManager::UseBomb() {
    if (bombs <= 0) return false;
    bombs--;
    return true;
}

int BombManager::GetBombs() const {
    return bombs;
}

int BombManager::GetFragments() const {
    return bombFragments;
}

int BombManager::GetMaxFragments() const {
    return maxFragments;
}

int BombManager::GetMaxBombs() const {
    return maxBombs;
}

void BombManager::Reset() {
    bombs = 3;          // ‰Šú’l‚Í3‚Æ‚©“K‹X’²®
    bombFragments = 0;
}

bool BombManager::IsFragmentsMax() const {
    return bombFragments >= maxFragments;
}

bool BombManager::IsBom() const {
    return bombs > 0;
}

void BombManager::ResetOnRespawn() {
    bombs = 3;
}