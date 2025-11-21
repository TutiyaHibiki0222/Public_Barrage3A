#pragma once

#include "GameObject.h"
#include "Transform2D.h"
#include "JsonUtility.h"
#include "SystemIO.h"
#include "Vector.h"
#include "Texture2DManager.hpp"
#include "UiJsonCommon.hpp"

// Component系.
#include "RectTransform2D.h"
#include "GraphicRaycaster.h"
#include "Canvas.h"
#include "Image.h"
#include "Button.h"
#include "Text.h"

struct TextData {
    std::string text = "";
    Color color{};
    int fontSize = 0;

    int alignment = 0;
	int horizontalOverflow = 0;
    int verticalOverflow = 0;
    float lineSpacing = 0.0f;
    int fontStyle = 0;

    std::string systemFontName = "";

	JSON(text, color, fontSize, alignment, horizontalOverflow, verticalOverflow, lineSpacing, fontStyle, systemFontName)
};

// C++ 側の FontStyle enum 定義（
enum class FontStyle : int {
    Normal = 0,
    Bold = 1,
    Italic = 2,
    BoldAndItalic = 3
};

struct ColorBlockData {
    Color normalColor		= Color(0xFFFFFFFF);
    Color highlightedColor	= Color(0xFFFFFFFF);
    Color pressedColor		= Color(0xFFFFFFFF);
    Color disabledColor		= Color(0xFFFFFFFF);
    float colorMultiplier = 0;
    float fadeDuration = 0;
	JSON(normalColor, highlightedColor, pressedColor, disabledColor, colorMultiplier, fadeDuration)
};

struct ButtonData {
    bool interactable = 0;
    Color color;
    int transition = 0;
    ColorBlockData colors;
	JSON(interactable, color, transition, colors)
};


struct PasKeyData
{
    std::string pathKey = "";    // ディレクトリなどに使うキー
    std::string key = "";        // リソースのキー
    std::string textKey = "";    // 表示用テキストキー
    std::string spritePath = ""; // スプライトリソースのパス
	JSON(pathKey, key, textKey, spritePath)
};

struct ImageData
{
    Color color = Color(0xFFFFFFFF);
    PasKeyData imgKey;      // PasKeyデータもインスタンス生成
    int type = 0;               // Image.Type を int にして格納
    bool preserveAspect = false;
	JSON(color, imgKey, type, preserveAspect)
};

struct CanvasData {
    int canvasOrder = 0;
    int renderMode = 0;  // RenderMode enum → int に変換（例：ScreenSpaceOverlay = 0 等）
    int scaleMode = 0;   // CanvasScaler::ScaleMode enum → int
    Vector2D referenceResolution = { 800.0f, 600.0f };

    JSON(canvasOrder, renderMode, scaleMode, referenceResolution)
};

class CreateUI
{
public:

    static std::shared_ptr<GameObject> CreateElement(const UIElement& element) {
        auto go = GameObject::Instantiate(element.gameObject.name);
        go->SetTag(element.gameObject.tag);
        go->SetActive(element.gameObject.isActive);
        // RectTransform
        {
            auto rect = go->AddAppBase<RectTransform2D>();
            rect->anchoredPosition = element.rectTransform.anchoredPosition;
            rect->SetSize(element.rectTransform.sizeDelta);
            rect->anchorMin = element.rectTransform.anchorMin;
            rect->anchorMax = element.rectTransform.anchorMax;
			rect->pivot = element.rectTransform.pivot;
			// Transform2D設定.
            Vector2D position(element.gameObject.position.x, element.gameObject.position.y);
            Vector2D scale(element.gameObject.scale.x, element.gameObject.scale.y);
            float  rotation = element.gameObject.rotation.z;
            go->transform->position = position;
            go->transform->rotation = rotation;
            go->transform->scale = scale;
        }

        // Components
        for (const auto& comp : element.components) {
            if (comp.type == "Canvas") {
                auto canvas = go->AddAppBase<Canvas>();
				auto canvasData = JsonUtility::FromJson<CanvasData>(comp.json);
				canvas->SetOrder(canvasData.canvasOrder);
				canvas->SetRenderMode(static_cast<RenderMode>(canvasData.renderMode));
				canvas->SetScaleMode(static_cast<CanvasScaler::ScaleMode>(canvasData.scaleMode));
                canvas->SetReferenceResolution(Vector2D(canvasData.referenceResolution.x, canvasData.referenceResolution.y));
            }
            else if (comp.type == "GraphicRaycaster") {
                auto raycaster = go->AddAppBase<GraphicRaycaster>();
            }
            else if (comp.type == "Image") {
                auto img = go->AddAppBase<Image>();
				auto imageData = JsonUtility::FromJson<ImageData>(comp.json);
                img->GetColor() = imageData.color;
                img->SetImageType(static_cast<Image::ImageType>(imageData.type));
                if (!imageData.imgKey.pathKey.empty()) {
                    auto sprite = Texture2DManager::GetInstance().GetTexture2D(imageData.imgKey.pathKey, imageData.imgKey.key);
                    if (sprite) {
                        img->SetSprite(sprite->GetTextureData(imageData.imgKey.textKey));
                    }
				}
                else
                {
                    img->SetSprite(nullptr);
                }
            }
            else if (comp.type == "Button") {
                auto btn = go->AddAppBase<Button>();
				auto buttonData = JsonUtility::FromJson<ButtonData>(comp.json);
				ColorBlock colorBlock;
                colorBlock.normalColor = buttonData.colors.normalColor;
                colorBlock.highlightedColor = buttonData.colors.highlightedColor;
                colorBlock.pressedColor = buttonData.colors.pressedColor;
                colorBlock.disabledColor = buttonData.colors.disabledColor;
				btn->SetFadeDuration(buttonData.colors.fadeDuration);
                btn->SetColorBlock(colorBlock);
            }
            else if (comp.type == "Text") {
                auto txt = go->AddAppBase<Text>();
				auto textData = JsonUtility::FromJson<TextData>(comp.json);
                txt->SetText(textData.text);
                txt->SetColor(textData.color);
				txt->SetTextAnchor(static_cast<TextAnchor>(textData.alignment));
				txt->SetHorizontalOverflow(static_cast<HorizontalOverflow>(textData.horizontalOverflow));
				txt->SetVerticalOverflow(static_cast<VerticalOverflow>(textData.verticalOverflow));
                txt->SetFontSize(textData.fontSize);
                if(!textData.systemFontName.empty())
                    txt->SetFont(std::make_shared<Font>(textData.systemFontName, textData.fontSize, ((textData.fontStyle + 1) % 2 == 0) ? 8 : 0));
            }
            else {
                // 不明なコンポーネント
            }
        }

        // 子要素
        for (const auto& child : element.children) {
            auto childGo = CreateElement(child);
            go->transform->AddChild(childGo->transform);
        }

        return go;
    }



public:
	static std::shared_ptr<GameObject> CreateUiJsonData(const std::string& file) {
		std::string jsonText;
		try {
			jsonText = System::IO::File::ReadAllText(file);
		}
		catch (...) {
            Debug::ErrorLog("Failed to read file: " + file);
			return nullptr; // ファイル読み込み失敗
		}
		auto uiData = JsonUtility::FromJson<UI>(jsonText);

		if (uiData.elements.empty()) {
			return nullptr; // UIデータが空
		}

        return CreateElement(uiData.elements[0]);
	}
};