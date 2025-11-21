#include "PowerManager.h"
#include <algorithm>

void PowerManager::Add(float value) {
    power += value;
    if (power > maxPower) power = maxPower;
    if (power < minPower) power = minPower;
}

void PowerManager::SetMin(float minValue) {
    minPower = minValue;
    if (power < minPower) power = minPower;
}

void PowerManager::SetMax(float maxValue) {
    maxPower = maxValue;
    if (power > maxPower) power = maxPower;
}

float PowerManager::GetPower() const {
    return power;
}

float PowerManager::GetMinPower() const {
    return minPower;
}

float PowerManager::GetMaxPower() const {
    return maxPower;
}


void PowerManager::Reset() {
    power = minPower;
}

void PowerManager::SetPower(float pp) {
    power = pp;
}
