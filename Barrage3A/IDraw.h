#pragma once

#include "Reflection.h"
#include "Mathf.h"
#include <cstdint>
#include <algorithm>

struct RectF {
    float x = 0, y = 0, w = 0, h = 0;
};

struct Color {
public:
    float r = 1.0f;     // 赤 (0 ～ 1.0f)
    float g = 1.0f;     // 緑 (0 ～ 1.0f)
    float b = 1.0f;     // 青 (0 ～ 1.0f)
    float a = 1.0f;     // 透明度 (0 ～ 1.0f)
	JSON(r, g, b, a)
public:
    Color() = default;

    Color(float r, float g, float b, float a = 1.0f)
        : r(Mathf::Clamp01(r)), g(Mathf::Clamp01(g)), b(Mathf::Clamp01(b)), a(Mathf::Clamp01(a)) {
    }

    Color(int r255, int g255, int b255, int a255 = 255)
        : r(Mathf::Clamp01(r255 / 255.0f))
        , g(Mathf::Clamp01(g255 / 255.0f))
        , b(Mathf::Clamp01(b255 / 255.0f))
        , a(Mathf::Clamp01(a255 / 255.0f)) {
    }

    Color(uint32_t packedColor) {
        uint8_t a8 = (packedColor >> 24) & 0xFF;
        if (a8 != 0) {
            a = Mathf::Clamp01(static_cast<float>(a8) / 255.0f);
        }
        else {
            a = 1.0f;
        }
        r = Mathf::Clamp01(static_cast<float>((packedColor >> 16) & 0xFF) / 255.0f);
        g = Mathf::Clamp01(static_cast<float>((packedColor >> 8) & 0xFF) / 255.0f);
        b = Mathf::Clamp01(static_cast<float>(packedColor & 0xFF) / 255.0f);
    }

    uint32_t ToPackedRGBA() const {
        return (Mathf::Round<int>(a * 255) << 24)
            |  (Mathf::Round<int>(r * 255) << 16)
            |  (Mathf::Round<int>(g * 255) << 8)
            |   Mathf::Round<int>(b * 255);
    }

    uint32_t ToPackedRGB() const {
        return (Mathf::Round<int>(r * 255) << 16)
            |  (Mathf::Round<int>(g * 255) << 8)
            |   Mathf::Round<int>(b * 255);
    }

    int R255() const { return Mathf::Round<int>(r * 255); }
    int G255() const { return Mathf::Round<int>(g * 255); }
    int B255() const { return Mathf::Round<int>(b * 255); }
    int A255() const { return Mathf::Round<int>(a * 255); }

    void SetFloat(float r_, float g_, float b_, float a_ = 1.0f) {
        r = Mathf::Clamp01(r_);
        g = Mathf::Clamp01(g_);
        b = Mathf::Clamp01(b_);
        a = Mathf::Clamp01(a_);
    }

    void SetInt(int r255_, int g255_, int b255_, int a255_ = 255) {
        r = Mathf::Clamp01(r255_ / 255.0f);
        g = Mathf::Clamp01(g255_ / 255.0f);
        b = Mathf::Clamp01(b255_ / 255.0f);
        a = Mathf::Clamp01(a255_ / 255.0f);
    }

    void SetPacked(uint32_t packedColor) {
        *this = Color(packedColor);
    }

    void LerpSelf(const Color& to, float t) {
        r = Mathf::Lerp(r, to.r, t);
        g = Mathf::Lerp(g, to.g, t);
        b = Mathf::Lerp(b, to.b, t);
        a = Mathf::Lerp(a, to.a, t);
    }

    // オペレーター

    bool operator==(const Color& other) const {
        constexpr float epsilon = 1e-5f;
        return  (fabs(r - other.r) < epsilon) &&
                (fabs(g - other.g) < epsilon) &&
                (fabs(b - other.b) < epsilon) &&
                (fabs(a - other.a) < epsilon);
    }

    bool operator!=(const Color& other) const {
        return !(*this == other);
    }

    Color operator+(const Color& other) const {
        return Color(
            Mathf::Clamp01(r + other.r),
            Mathf::Clamp01(g + other.g),
            Mathf::Clamp01(b + other.b),
            Mathf::Clamp01(a + other.a)
        );
    }

    Color& operator+=(const Color& other) {
        *this = *this + other;
        return *this;
    }

    Color operator-(const Color& other) const {
        return Color(
            Mathf::Clamp01(r - other.r),
            Mathf::Clamp01(g - other.g),
            Mathf::Clamp01(b - other.b),
            Mathf::Clamp01(a - other.a)
        );
    }

    Color& operator-=(const Color& other) {
        *this = *this - other;
        return *this;
    }

    Color operator*(float scalar) const {
        return Color(
            Mathf::Clamp01(r * scalar),
            Mathf::Clamp01(g * scalar),
            Mathf::Clamp01(b * scalar),
            Mathf::Clamp01(a * scalar)
        );
    }

    Color& operator*=(float scalar) {
        *this = *this * scalar;
        return *this;
    }

    Color operator*(const Color& other) const {
        return Color(
            Mathf::Clamp01(r * other.r),
            Mathf::Clamp01(g * other.g),
            Mathf::Clamp01(b * other.b),
            Mathf::Clamp01(a * other.a)
        );
    }

    Color& operator*=(const Color& other) {
        *this = *this * other;
        return *this;
    }

    Color operator+(int scalar255) const {
        float scalar = scalar255 / 255.0f;
        return Color(
            Mathf::Clamp01(r + scalar),
            Mathf::Clamp01(g + scalar),
            Mathf::Clamp01(b + scalar),
            Mathf::Clamp01(a + scalar)
        );
    }

    Color& operator+=(int scalar255) {
        *this = *this + scalar255;
        return *this;
    }

    Color operator-(int scalar255) const {
        float scalar = scalar255 / 255.0f;
        return Color(
            Mathf::Clamp01(r - scalar),
            Mathf::Clamp01(g - scalar),
            Mathf::Clamp01(b - scalar),
            Mathf::Clamp01(a - scalar)
        );
    }

    Color& operator-=(int scalar255) {
        *this = *this - scalar255;
        return *this;
    }

    Color operator*(int scalar255) const {
        float scalar = scalar255 / 255.0f;
        return *this * scalar;
    }

    Color& operator*=(int scalar255) {
        *this = *this * scalar255;
        return *this;
    }

public: // static 関数

    static Color Lerp(const Color& from, const Color& to, float t) {
        return Color(
            Mathf::Lerp(from.r, to.r, t),
            Mathf::Lerp(from.g, to.g, t),
            Mathf::Lerp(from.b, to.b, t),
            Mathf::Lerp(from.a, to.a, t)
        );
    }

    static const Color Red()    { return Color(1.0f, 0.0f, 0.0f); }
    static const Color Green()  { return Color(0.0f, 1.0f, 0.0f); }
    static const Color Blue()   { return Color(0.0f, 0.0f, 1.0f); }
    static const Color White()  { return Color(1.0f, 1.0f, 1.0f); }
    static const Color Black()  { return Color(0.0f, 0.0f, 0.0f); }
    static const Color Yellow() { return Color(1.0f, 1.0f, 0.0f); }
    static const Color Cyan()   { return Color(0.0f, 1.0f, 1.0f); }
    static const Color Magenta(){ return Color(1.0f, 0.0f, 1.0f); }
    static const Color Gray()   { return Color(0.5f, 0.5f, 0.5f); }
    static const Color Clear()  { return Color(0.0f, 0.0f, 0.0f, 0.0f); }
};


// 描画順を決定するための構造体(レイヤーとオーダーを持つ).
struct SortingLayer {
	int layer = 0;          // レイヤー番号.
	int orderInLayer = 0;   // レイヤー内の順序.

    int GetSortingOrder() const {
        return layer * 1000 + orderInLayer;
    }
};

// 描画順を決定するための列挙型(Canvasなどなど).
enum class RenderMode {
	ScreenSpaceOverlay, // 画面上にオーバーレイするモード.
	ScreenSpaceCamera,  // カメラの前面に描画するモード.
	WorldSpace          // ワールド空間に描画するモード(通常描画処理はこれ).
};

class IRendererDraw {
public:
    virtual bool IsDraw() { return true; }
    virtual RectF GetAABB() const { return RectF(); }
    virtual RenderMode GetRenderMode() const { return RenderMode::WorldSpace; }
    virtual void Draw() = 0;
    virtual int GetSortingOrder() const { return 0; }
    virtual ~IRendererDraw() = default;
};

class IUIDraw {
public:
    virtual void DrawUI() = 0;
    virtual int GetCanvasOrder() const { return 0; }
    virtual ~IUIDraw() = default;
};
