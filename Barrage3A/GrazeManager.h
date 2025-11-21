#pragma once

class GrazeManager {
private:
    int grazeCount = 0;
    int count = 0;
public:
    void Add(int count);
    int GetGrazeCount() const;
    int GetTotalGraze() const;
    void NextStargReset();
    void Reset();
};
