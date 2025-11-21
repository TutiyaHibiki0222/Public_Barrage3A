#pragma once

class PowerManager {
private:
    float power = 0.0f;
    float minPower = 1.0f;
    float maxPower = 4.0f;
public:
    void Add(float value);
    void SetMin(float minValue);
    void SetMax(float maxValue);
    float GetPower() const;
    float GetMinPower() const;
    float GetMaxPower() const;
    void Reset();
    void SetPower(float pp);
};