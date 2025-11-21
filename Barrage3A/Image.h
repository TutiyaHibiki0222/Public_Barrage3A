#pragma once
#include "headers.h"
#include "UiBase.h"
#include "GameObject.h"
#include "RectTransform2D.h"
#include "IDraw.h"
#include "Texture2D.h"


// Image class and AppBase class
class Image : public UiBase {
    friend class Button;
public:
    enum class ImageType {
        Simple, Sliced, Tiled, Filled
    };

    enum class FillMethod {
        Horizontal,
        Vertical,
        Radial360
    };

    enum class FillOriginHorizontal {
        Left,
        Right
    };

    enum class FillOriginVertical {
        Bottom,
        Top
    };

    enum class FillOriginRadial360 {
        Bottom,
        Left,
        Top,
        Right
    };
private:
    struct FilledOptions {
        FillMethod method = Image::FillMethod::Horizontal;
        bool clockwise = true;

        FillOriginHorizontal    horizontalOrigin = Image::FillOriginHorizontal::Left;
        FillOriginVertical      verticalOrigin = Image::FillOriginVertical::Bottom;
        FillOriginRadial360     radial360Origin = Image::FillOriginRadial360::Bottom;
    };

private:    // 非公開.
    std::weak_ptr<Sprite> sprite;
    Color color         = Color(1.0f,1.0f,1.0f,1.0f);
    ImageType type      = ImageType::Simple;

    // Filled 用
    FilledOptions filledOptions;
    float   fillAmount = 1.0f;
    int     fillOrigin = 0;

    float pixelsPerUnitMultiplier = 1.0f;
public:     // 公開.

    // コンストラクタ.
    Image();
    Image(std::shared_ptr<GameObject>);

    void Awake() override;

    void SetSprite(const std::shared_ptr<Sprite>& _sprite);
    void SetImageType(const ImageType _type);
    Color& GetColor();
    void SetColor(const Color&);
    FilledOptions& GetFilledOptions() { return filledOptions; }

    void SetFillMethod(FillMethod method) { filledOptions.method = method; }

    void SetFillAmount(float amount) { fillAmount = Mathf::Clamp(amount, 0.0f, 1.0f); }

    void SetFillClockwise(bool clockwise) { filledOptions.clockwise = clockwise; }

    void SetFillOrigin(int origin) {   fillOrigin = origin;  }

    void SetPixelsPerUnitMultiplier(float multiplier) {
        pixelsPerUnitMultiplier = multiplier > 0.0f ? multiplier : 1.0f;
    }

private:    // 描画関数.
    void OnUiDraw(const Quad2D& quad, const std::shared_ptr<Canvas>& rect) override;
    void DrawSimpleColor(const Quad2D& quad);
    void DrawSimple(const Quad2D& quad, const std::shared_ptr<Sprite>& tex);
    void DrawTiled(const Quad2D& quad, const std::shared_ptr<Sprite>& tex);
    void DrawSliced(const Quad2D& quad, const std::shared_ptr<Sprite>& tex);
    void DrawFilled(const Quad2D& quad, const std::shared_ptr<Sprite>& tex);
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Image>(*this);
    }
};