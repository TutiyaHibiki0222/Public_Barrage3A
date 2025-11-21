#pragma once

#include <random>
#include <chrono>

class Random {
private:
    std::mt19937 engine;  // メルセンヌ・ツイスター法による擬似乱数生成器.

    // プライベートコンストラクタ（シングルトンパターン）.
    Random() {
        // シードを現在時刻に基づいて設定.
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        engine = std::mt19937(static_cast<unsigned int>(seed));
    }

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

public:
    // シングルトンインスタンスの取得.
    static Random& Instance() {
        static Random instance;
        return instance;
    }

    // 浮動小数点数の範囲ランダム（[min, max)）
    template <typename T>
    T Range(T min, T max) {
        if (min > max) std::swap(min, max);
        if (min == max) return max;
        std::uniform_real_distribution<T> dist(min, max);
        return dist(engine);
    }

    // 整数の範囲ランダム（[min, max] の閉区間）
    int Range(int min, int max) {
        if (min > max) std::swap(min, max);
        if (min == max) return max;
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine);
    }



    // ランダムな値を 0.0f から 1.0f の範囲で生成.
    float Value() {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(engine);
    }

    // ランダムな正規分布に基づく値を生成.
    float Normal(float mean, float stddev) {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(engine);
    }

    // ランダムなベクトル2Dを生成 (0.0f から 1.0f の範囲内).
    std::pair<float, float> Random2D() {
        return { Value(), Value() };
    }

    float Rotation(float range) {
        std::uniform_real_distribution<float> dist(-range, range);
        return dist(engine);
    }

    // ランダムエンジンの参照を返す関数を追加
    std::mt19937& GetEngine() {
        return engine;
    }
};

// ランダムclass;
#define Random  Random::Instance()