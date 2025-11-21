#pragma once


#ifndef MATHF_H
#define MATHF_H

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif


#include <cmath>
#include <vector>
/// <summary>
/// 数学関数.
/// </summary>
struct Mathf {
public:
    static constexpr float PI = 3.14159265359f;
    static constexpr float PI2 = 6.28318530718f; // ← 2π
    static constexpr float HALF_PI = 1.57079632679f;
    static constexpr float Deg2Rad = PI / 180.0f;
    static constexpr float Rad2Deg = 180.0f / PI;

    // --- 基本機能 (float版) ---

    // 絶対値 (float)
    static float Abs(float value) {
        return std::fabs(value);
    }

    // 絶対値 (int)
    static int Abs(int value) {
        return std::abs(value);
    }

    // 最大値 (float)
    static float Max(float a, float b) {
        return std::fmax(a, b);
    }

    // 最大値 (int)
    static int Max(int a, int b) {
        return std::max(a, b);
    }

    // 最大値 (float 配列)
    static float Max(const std::vector<float>& values) {
        return *std::max_element(values.begin(), values.end());
    }

    // 最大値 (int 配列)
    static int Max(const std::vector<int>& values) {
        return *std::max_element(values.begin(), values.end());
    }

    // 最小値 (float)
    static float Min(float a, float b) {
        return std::fmin(a, b);
    }

    // 最小値 (int)
    static int Min(int a, int b) {
        return std::min(a, b);
    }

    // 最小値 (float 配列)
    static float Min(const std::vector<float>& values) {
        return *std::min_element(values.begin(), values.end());
    }

    // 最小値 (int 配列)
    static int Min(const std::vector<int>& values) {
        return *std::min_element(values.begin(), values.end());
    }

    // --- その他の機能 ---

    // 線形補間 (float)
    static float Lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    // 線形補間 (int)
    static int Lerp(int a, int b, float t) {
        return static_cast<int>(a + t * (b - a));
    }

    // クランプ (float)
    static float Clamp(float value, float _min, float _max) {
        return std::fmax(_min, std::fmin(_max, value));
    }

    // クランプ (int)
    static int Clamp(int value, int _min, int _max) {
        return std::max(_min, std::min(_max, value));
    }
    // クランプ (0 ～ 1 float)
    static float Clamp01(float value)
    {
        if (value < 0.0f)
        {
            return 0.0f;
        }
        if (value > 1.0f)
        {
            return 1.0f;
        }

        return value;
    }

    // --- 三角関数 (float) ---

    static float Sin(float radians) {
        return std::sin(radians);
    }

    static float Cos(float radians) {
        return std::cos(radians);
    }

    static float Tan(float radians) {
        return std::tan(radians);
    }

    // --- 角度変換 ---

    static float DegToRad(float degrees) {
        return degrees * Deg2Rad;
    }

    static float RadToDeg(float radians) {
        return radians * Rad2Deg;
    }

    static float DeltaAngle(float current, float target) noexcept {
        float diff = fmodf((target - current), 360.0f);
        if (diff > 180.0f) diff -= 360.0f;
        if (diff < -180.0f) diff += 360.0f;
        return diff;
    }
    static float LerpAngle(float a, float b, float t) noexcept {
        float diff = DeltaAngle(a, b);
        return a + diff * Clamp01(t);
    }

    static float NormalizeAngle360(float angle) noexcept {
        angle = fmodf(angle, 360.0f);
        if (angle < 0) angle += 360.0f;
        return angle;
    }

    static float NormalizeAngle180(float angle) noexcept {
        angle = fmodf(angle + 180.0f, 360.0f);
        if (angle < 0) angle += 360.0f;
        return angle - 180.0f;
    }

    // --- 配列処理をサポート ---

    // 配列の合計 (float)
    static float Sum(const std::vector<float>& values) {
        float sum = 0.0f;
        for (float value : values) {
            sum += value;
        }
        return sum;
    }

    // 配列の合計 (int)
    static int Sum(const std::vector<int>& values) {
        int sum = 0;
        for (int value : values) {
            sum += value;
        }
        return sum;
    }

    // 配列の平均 (float)
    static float Average(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        return Sum(values) / values.size();
    }

    // 配列の平均 (int)
    static float Average(const std::vector<int>& values) {
        if (values.empty()) return 0;
        return static_cast<float>(Sum(values)) / values.size();
    }

    static float Atan2(float y, float x) {
        return std::atan2(y, x);
    }

    static float Sqrt(float x) {
        return std::sqrt(x);
    }

    static float Pow(float base, float exponent) {
        return std::pow(base, exponent);
    }

    static float Sign(float x) {
        return (x > 0.0f) ? 1.0f : (x < 0.0f ? -1.0f : 0.0f);
    }

    static float Repeat(float t, float length) {
        return t - std::floor(t / length) * length;
    }

    static float PingPong(float t, float length) {
        t = Repeat(t, length * 2.0f);
        return length - std::abs(t - length);
    }

    static float SmoothStep(float a, float b, float t) {
        t = Clamp01(t);
        t = t * t * (3.0f - 2.0f * t);
        return a + (b - a) * t;
    }

    // current, target, maxDelta はすべてラジアン単位
    static float MoveAngle(float current, float target, float maxDelta) {
        float delta = target - current;

        // 角度差を -π〜+π の範囲に正規化
        while (delta > 3.14159265359f) delta -= 2 * 3.14159265359f;
        while (delta < -3.14159265359f) delta += 2 * 3.14159265359f;

        // 最大回転量を超えないようにクランプ
        if (delta > maxDelta) delta = maxDelta;
        if (delta < -maxDelta) delta = -maxDelta;

        return current + delta;
    }



    // ----- 変換処理 -----

    // 与えられた浮動小数点数を四捨五入し、指定された型に変換して返す関数.
    template<typename RetType, typename FromType>
    static RetType Round(FromType v) {
        if constexpr (std::is_integral_v<RetType> && std::is_floating_point_v<FromType>) {
            return static_cast<RetType>(std::round(v));
        }
        else if constexpr (std::is_floating_point_v<RetType> && std::is_floating_point_v<FromType>) {
            return static_cast<RetType>(std::round(v));
        }
        else {
            static_assert(sizeof(RetType) == 0, "Unsupported combination for Round");
        }
    }
    // 与えられた浮動小数点数を切り捨てて、指定された型に変換して返す関数.
    template<typename RetType, typename FromType>
    static RetType Floor(FromType v) {
        if constexpr (std::is_integral_v<RetType> && std::is_floating_point_v<FromType>) {
            return static_cast<RetType>(std::floor(v));
        }
        else if constexpr (std::is_floating_point_v<RetType> && std::is_floating_point_v<FromType>) {
            return static_cast<RetType>(std::floor(v));
        }
        else {
            static_assert(sizeof(RetType) == 0, "Unsupported combination for Floor");
        }
    }
    // 与えられた浮動小数点数を切り上げて、指定された型に変換して返す関数.
    template<typename RetType, typename FromType>
    static RetType Ceil(FromType v) {
        if constexpr (std::is_integral_v<RetType> && std::is_floating_point_v<FromType>) {
            return static_cast<RetType>(std::ceil(v));
        }
        else if constexpr (std::is_floating_point_v<RetType> && std::is_floating_point_v<FromType>) {
            return static_cast<RetType>(std::ceil(v));
        }
        else {
            static_assert(sizeof(RetType) == 0, "Unsupported combination for Ceil");
        }
    }
    // 与えられた浮動小数点数を0方向へ切り捨て（整数部を抽出）して、指定された型に変換して返す関数.
    template<typename RetType, typename FromType>
    static RetType Trunc(FromType v) {
        if constexpr (std::is_integral_v<RetType> && std::is_floating_point_v<FromType>) {
            return static_cast<RetType>(std::trunc(v));
        }
        else if constexpr (std::is_floating_point_v<RetType> && std::is_floating_point_v<FromType>) {
            return static_cast<RetType>(std::trunc(v));
        }
        else {
            static_assert(sizeof(RetType) == 0, "Unsupported combination for Trunc");
        }
    }
};

#endif // MATHF_H