// TextRenderer.h
#pragma once
#include <string>
#include <vector>
#include "IDraw.h"
#include "Canvas.h"
#include "Font.hpp"
#include "SpriteFont.h"
#include "RectTransform2D.h"
#include "Vector.h"
#include "Text.h"

namespace GameEngine {

    class TextRenderer {
    public:
        static void DrawWithFont(
            const std::vector<std::string>& lines,
            const Quad2D& quad,
            const std::shared_ptr<Font>& font,
            const Color& color,
            float scaleX,
            float scaleY,
            float lineSpacing,
            int maxVisibleLines,
            TextAnchor anchor
        );

        static void DrawWithSpriteFont(
            const std::vector<std::string>& lines,
            const Quad2D& quad,
            const std::shared_ptr<GameEngine::SpriteFont>& spriteFont,
            const Color& color,
            float fontSize,
            float lineSpacing,
            int maxVisibleLines,
            TextAnchor anchor
        );
    };

}
