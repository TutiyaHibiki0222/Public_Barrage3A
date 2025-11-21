#include "GrazeManager.h"
#include "GameManager.h"
void GrazeManager::Add(int count) {
    grazeCount += count;
    if (grazeCount < 0) grazeCount = 0;
    this->count++;

    if (grazeCount % 10 == 0) {
        GameManager::GetInstance().GetScoreManager().OnGraze(10);
    }
}

int GrazeManager::GetGrazeCount() const {
    return grazeCount;
}

int GrazeManager::GetTotalGraze() const {
    return count;
}

void GrazeManager::NextStargReset() {
    grazeCount = 0;
}

void GrazeManager::Reset() {
    grazeCount = 0;
    count = 0;
}
