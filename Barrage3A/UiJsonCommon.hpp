#pragma once
#include <string>
#include <vector>
#include <map>
#include <array>
#include <iostream>
#include "Reflection.h"  // 先ほどのReflectionヘッダ
#include "Vector.h"

struct GameObjectData {
    bool isActive;
    std::string name;
    std::string tag;
    Vector3D position;
    Vector3D rotation;
    Vector3D scale;
    JSON(isActive,name, tag, position, rotation, scale)
};

struct RectTransformData {
    Vector2D anchoredPosition;
    Vector2D sizeDelta;
    Vector2D anchorMin;
    Vector2D anchorMax;
    Vector2D pivot;
    JSON(anchoredPosition, sizeDelta, anchorMin, anchorMax, pivot)
};

struct ComponentData {
    std::string type;
    std::string json;  // json文字列として保持（ここは展開しない）

    JSON(type, json)
};

struct UIElement {
    GameObjectData gameObject;
    RectTransformData rectTransform;
    std::vector<ComponentData> components;
    std::vector<UIElement> children;

    JSON(gameObject, rectTransform, components, children)
};

struct UI {

    std::vector<UIElement> elements;

    JSON(elements)
};