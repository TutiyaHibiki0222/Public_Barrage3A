/*
    ◆ Text.cpp

    クラス名        : Text クラス
    作成日          : 2025/06/23 (月) 13:58:05
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/
#include "Text.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "TextSegment.hpp"
// コンストラクタ 
Text::Text() : UiBase(){

}
Text::Text(std::shared_ptr<GameObject> owner) : UiBase(owner) {
    

}

void Text::Awake() {
	raycastTarget = false; // Textは通常、Raycastの対象外
}

// --- リッチテキスト用折り返し ---
std::vector<std::string> Text::WrapRichText(
    const std::string& richText,
    float maxWidth,
    GameEngine::SpriteFont* spriteFont,
    float baseFontSize
) {
    auto segments = ParseRichText(richText);

    std::vector<std::string> lines;
    std::string currentLine;
    float penX = 0.f;

    for (const auto& seg : segments) {
        float segFontSize = baseFontSize * seg.sizePercent / 100.f;
        float scale = spriteFont->ComputeScale(segFontSize);

        for (char c : seg.text) {
            float charWidth = (spriteFont->GetCharPixelWidth(c) + spriteFont->GetCharKerning(c)) * scale;

            if (penX + charWidth > maxWidth && !currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine.clear();
                penX = 0.f;
            }
            currentLine += c;
            penX += charWidth;
        }
    }
    if (!currentLine.empty())
        lines.push_back(currentLine);

    return lines;
}

// --- OnUiDraw 実装 ---
void Text::OnUiDraw(const Quad2D& quad, const std::shared_ptr<Canvas>& rect) {
    // 1) 表示領域サイズを取得
    float width  = quad.topRight.x - quad.topLeft.x;
    float height = quad.bottomLeft.y - quad.topLeft.y;

    // 2) ラップして描画する行を取得
    auto lines      = PrepareLines(text, width);

    // 3) 描画用寸法・スケールを計算
    TextLayout layout = ComputeLayout(lines, quad, width, height);

    // 4) 実際の描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, color.A255());
    if (spriteFont) {
        DrawWithSpriteFont(lines, layout);
    }
    else if(font) {
        DrawWithDxFont(lines, layout);
    }
    else {
        DrawFallbackText(lines, quad);
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
/*
// --- OnUiDraw 実装 ---
void Text::OnUiDraw(const Quad2D& quad, const std::shared_ptr<Canvas>& rect) {
    float width = quad.topRight.x - quad.topLeft.x;
    float height = quad.bottomLeft.y - quad.topLeft.y;

    std::vector<std::string> lines;

    if (hOverflow == HorizontalOverflow::Wrap) {
        if (font) {
            lines = WrapText(text, font->GetFontHandle(), width);
        }
        else if (spriteFont) {
            lines = WrapRichText(text, width, spriteFont.get(), static_cast<float>(fontSize));
        }
        else {
            lines = { text };
        }
    }
    else {
        lines = { text };
    }

    int fontHeight = font ? font->GetFontSize() : (spriteFont ? spriteFont->GetGlyphHeight() : fontSize);

    int maxVisibleLines = (vOverflow == VerticalOverflow::Truncate)
        ? Mathf::Min((int)(height / (fontHeight * lineSpacing)), (int)lines.size())
        : (int)lines.size();

    float totalTextHeight = fontHeight * lineSpacing * maxVisibleLines;

    float scaleX = 1.f, scaleY = 1.f;
    if (bestFit && font) {
        float rawTextWidth = static_cast<float>(GetDrawStringWidthToHandle(text.c_str(), (int)text.size(), font->GetFontHandle()));
        scaleX = width / rawTextWidth;
        scaleY = height / totalTextHeight;

        float scale = Mathf::Min(scaleX, scaleY);
        scale = Mathf::Clamp(scale, minSize / fontHeight, maxSize / fontHeight);
        scaleX = scaleY = scale;
    }

    Vector2D transformScale = transform->GetWorldScale();
    scaleX *= transformScale.x;
    scaleY *= transformScale.y;

    float baseY = quad.topLeft.y;
    if (anchor == TextAnchor::MiddleLeft || anchor == TextAnchor::MiddleCenter || anchor == TextAnchor::MiddleRight)
        baseY += (height - totalTextHeight) / 2.f;
    else if (anchor == TextAnchor::LowerLeft || anchor == TextAnchor::LowerCenter || anchor == TextAnchor::LowerRight)
        baseY += (height - totalTextHeight);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, color.A255());

    if (!spriteFont) {
        int fontHandle = font->GetFontHandle();

        for (int i = 0; i < maxVisibleLines; ++i) {
            const std::string& line = lines[i];
            float lineWidth = GetDrawStringWidthToHandle(line.c_str(), (int)line.size(), fontHandle) * scaleX;

            float drawX = quad.topLeft.x;
            if (anchor == TextAnchor::UpperCenter || anchor == TextAnchor::MiddleCenter || anchor == TextAnchor::LowerCenter)
                drawX += (width - lineWidth) / 2.f;
            else if (anchor == TextAnchor::UpperRight || anchor == TextAnchor::MiddleRight || anchor == TextAnchor::LowerRight)
                drawX += (width - lineWidth);

            float drawY = baseY + i * fontHeight * lineSpacing * scaleY;

            DrawExtendStringToHandle(
                static_cast<int>(drawX),
                static_cast<int>(drawY),
                scaleX, scaleY,
                line.c_str(),
                color.ToPackedRGB(),
                fontHandle
            );
        }
    }
    else {
        GameEngine::SpriteFont::DrawParam baseParam;
        baseParam.fontSize = static_cast<float>(fontSize);
        baseParam.letterSpacing = 0;
        baseParam.color = color;
        baseParam.scale = spriteFont->ComputeScale(baseParam.fontSize);

        for (int i = 0; i < maxVisibleLines; ++i) {
            const std::string& line = lines[i];
            std::vector<TextSegment> segments = ParseRichText(line, true);

            // 1) 行幅計算
            float lineWidth = 0.f;
            for (const auto& seg : segments) {
                GameEngine::SpriteFont::DrawParam param = baseParam;
                param.fontSize = baseParam.fontSize * seg.sizePercent / 100.f;
                param.scale = spriteFont->ComputeScale(param.fontSize);
                lineWidth += spriteFont->MeasureTextWidth(seg.text, param);
            }

            // 2) 水平アンカー計算
            float drawX = quad.topLeft.x;
            if (anchor == TextAnchor::UpperCenter || anchor == TextAnchor::MiddleCenter || anchor == TextAnchor::LowerCenter) {
                drawX += (width - lineWidth) * 0.5f;  // 中央揃え
            }
            else if (anchor == TextAnchor::UpperRight || anchor == TextAnchor::MiddleRight || anchor == TextAnchor::LowerRight) {
                drawX += (width - lineWidth);          // 右揃え
            }
            // 左揃えはdrawXそのまま

            // 3) 垂直位置計算
            float lineHeight = spriteFont->GetGlyphHeight() * lineSpacing * baseParam.scale;
            float drawY = baseY + i * lineHeight;

            // 4) ペン位置を初期化
            float penX = drawX;

            // 5) 描画
            for (const auto& seg : segments) {
                GameEngine::SpriteFont::DrawParam param = baseParam;
                param.fontSize = baseParam.fontSize * seg.sizePercent / 100.f;
                param.scale = spriteFont->ComputeScale(param.fontSize);

                if (seg.color != -1) {
                    param.color = Color(seg.color);
                }

                param.bgColor = seg.bgColor;
                param.bold = seg.bold;
                param.italic = seg.italic;
                param.outlineWidth = seg.outlineWidth;
                param.letterSpacing = (seg.letterSpacing >= 0.0f) ? seg.letterSpacing : baseParam.letterSpacing;

                float offsetY = spriteFont->GetGlyphHeight() * (baseParam.scale - param.scale);
                float offsetX = spriteFont->GetCharPixelWidth(' ') * (baseParam.scale - param.scale) * 0.5f;

                for (char c : seg.text) {
                    auto [offX, offY] = spriteFont->GetCharOffset(c);

                    float dx = penX + offX * param.scale + offsetX;
                    float dy = drawY + offY * param.scale + offsetY;

                    spriteFont->DrawGlyph(dx, dy, c, param);

                    float advance = (spriteFont->GetCharPixelWidth(c) + param.letterSpacing + spriteFont->GetCharKerning(c)) * param.scale;
                    penX += advance;
                }
            }
        }
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
*/

// --- 既存 WrapText（単純文字列） ---
std::vector<std::string> Text::WrapText(const std::string& text, int fontHandle, float maxWidth) {
    std::vector<std::string> lines;
    std::string currentLine;

    for (char c : text) {
        if (c == '\n') {
            lines.push_back(currentLine);
            currentLine.clear();
            continue;
        }

        std::string trialLine = currentLine + c;
        int w = GetDrawStringWidthToHandle(trialLine.c_str(), (int)trialLine.size(), fontHandle);

        if (w > maxWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine = c;
        }
        else {
            currentLine += c;
        }
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}

// --- 指定幅で折り返した行の中で最大幅を計算 ---
int Text::GetMaxLineWidth(const std::vector<std::string>& lines, int fontHandle) {
    int maxWidth = 0;
    for (const auto& line : lines) {
        int w = GetDrawStringWidthToHandle(line.c_str(), (int)line.size(), fontHandle);
        if (w > maxWidth) maxWidth = w;
    }
    return maxWidth;
}

// --- 指定幅に収まるようにテキストを途中で切る ---
std::string Text::TruncateTextToWidth(const std::string& text, int fontHandle, float maxWidth) {
    std::string result;
    for (size_t i = 0; i < text.size(); ++i) {
        std::string candidate = result + text[i];
        if (GetDrawStringWidthToHandle(candidate.c_str(), (int)candidate.size(), fontHandle) > maxWidth) {
            break;
        }
        result.push_back(text[i]);
    }
    return result;
}

std::vector<std::string> Text::PrepareLines(const std::string& text, float width) {
    if (hOverflow == HorizontalOverflow::Wrap) {
        if (font) {
            return WrapText(text, font->GetFontHandle(), width);
        }
        else if (spriteFont) {
            return WrapRichText(text, width, spriteFont.get(), (float)fontSize);
        }
        else {
            return { text };
        }
    }
    return { text };
}

TextLayout Text::ComputeLayout(
    const std::vector<std::string>& lines,
    const Quad2D& quad,
    float width,
    float height)
{
    TextLayout lo{};
    lo.width            = width;
    lo.fontHandle       = font ? font->GetFontHandle() : -1;
    int rawFontHeight   = font ? font->GetFontSize() : fontSize;

    // 行数制限
    lo.maxLines = (vOverflow == VerticalOverflow::Truncate)
        ? Mathf::Min((int)(height / (rawFontHeight * lineSpacing)), (int)lines.size())
        : (int)lines.size();

    // 合計テキスト高さ
    float totalH = rawFontHeight * lineSpacing * lo.maxLines;

    // 拡大率
    lo.scaleX = lo.scaleY = 1.f;
    if (bestFit && font) {
        float textW = (float)GetDrawStringWidthToHandle(text.c_str(), (int)text.size(), lo.fontHandle);
        float sx = width / textW;
        float sy = height / totalH;
        float s = Mathf::Clamp(Mathf::Min(sx, sy), minSize / rawFontHeight, maxSize / rawFontHeight);
        lo.scaleX = lo.scaleY = s;
    }
    Vector2D ts = transform->GetWorldScale();
    lo.scaleX *= ts.x;  lo.scaleY *= ts.y;

    // 基準座標
    lo.baseX = quad.topLeft.x;
    lo.baseY = quad.topLeft.y;
    // 垂直アンカー調整
    if (anchor == TextAnchor::MiddleLeft || anchor == TextAnchor::MiddleCenter || anchor == TextAnchor::MiddleRight)
        lo.baseY += (height - totalH) * 0.5f;
    else if (anchor == TextAnchor::LowerLeft || anchor == TextAnchor::LowerCenter || anchor == TextAnchor::LowerRight)
        lo.baseY += (height - totalH);

    // フォント高さ
    lo.lineHeight = rawFontHeight;

    return lo;
}

void Text::DrawWithDxFont(
    const std::vector<std::string>& lines,
    const TextLayout& lo)
{
    for (int i = 0; i < lo.maxLines; ++i) {
        const auto& line = lines[i];
        // 行幅計算
        float lineW = GetDrawStringWidthToHandle(
            line.c_str(),
            (int)line.size(),
            lo.fontHandle
        ) * lo.scaleX;

        // 水平アンカー
        float dx = lo.baseX;
        if (anchor == TextAnchor::UpperCenter
            || anchor == TextAnchor::MiddleCenter
            || anchor == TextAnchor::LowerCenter)
        {
            dx += (lo.width - lineW) * 0.5f;
        }
        else if (anchor == TextAnchor::UpperRight
            || anchor == TextAnchor::MiddleRight
            || anchor == TextAnchor::LowerRight)
        {
            dx += (lo.width - lineW);
        }

        float dy = lo.baseY + i * lo.lineHeight * lineSpacing * lo.scaleY;

        if (lo.fontHandle >= 0) {
            DrawExtendStringToHandle(
                (int)dx, (int)dy,
                lo.scaleX, lo.scaleY,
                line.c_str(),
                color.ToPackedRGB(),
                lo.fontHandle
            );
        }
        else {
            DrawString(
                (int)dx, (int)dy,
                line.c_str(),
                color.ToPackedRGB()
            );
        }
    }
}

void Text::DrawWithSpriteFont(const std::vector<std::string>& lines, const TextLayout& lo) {
    GameEngine::SpriteFont::DrawParam baseParam;
    baseParam.fontSize = static_cast<float>(fontSize);
    baseParam.scale = spriteFont->ComputeScale(baseParam.fontSize);
    baseParam.letterSpacing = 0.0f;
    baseParam.color = color;

    float baseY = lo.baseY;
    float width = lo.width;

    for (int i = 0; i < lo.maxLines; ++i) {
        const std::string& line = lines[i];
        std::vector<TextSegment> segments = ParseRichText(line, true);

        // --- 1. 行幅計算（セグメントごと）
        float lineWidth = 0.0f;
        for (const auto& seg : segments) {
            GameEngine::SpriteFont::DrawParam param = baseParam;
            param.fontSize = baseParam.fontSize * seg.sizePercent / 100.f;
            param.scale = spriteFont->ComputeScale(param.fontSize);
            lineWidth += spriteFont->MeasureTextWidth(seg.text, param);
        }

        // --- 2. Xアンカー補正
        float drawX = lo.baseX;
        if (anchor == TextAnchor::UpperCenter || anchor == TextAnchor::MiddleCenter || anchor == TextAnchor::LowerCenter)
            drawX += (width - lineWidth) * 0.5f;
        else if (anchor == TextAnchor::UpperRight || anchor == TextAnchor::MiddleRight || anchor == TextAnchor::LowerRight)
            drawX += (width - lineWidth);

        // --- 3. Y計算（ここは正しく baseY + i * 行高）
        float drawY = baseY + i * spriteFont->GetGlyphHeight() * lineSpacing * baseParam.scale;

        // --- 4. ペン初期化
        float penX = drawX;

        // --- 5. セグメント描画
        for (const auto& seg : segments) {
            GameEngine::SpriteFont::DrawParam param = baseParam;
            param.fontSize *= seg.sizePercent / 100.f;
            param.scale = spriteFont->ComputeScale(param.fontSize);
            param.letterSpacing = (seg.letterSpacing >= 0.f) ? seg.letterSpacing : baseParam.letterSpacing;
            param.color = (seg.color != -1) ? Color(seg.color) : baseParam.color;
            param.bgColor = seg.bgColor;
            param.bold = seg.bold;
            param.italic = seg.italic;
            param.outlineWidth = seg.outlineWidth;

            float offsetY = spriteFont->GetGlyphHeight() * (baseParam.scale - param.scale);
            float offsetX = spriteFont->GetCharPixelWidth(' ') * (baseParam.scale - param.scale) * 0.5f;

            for (char c : seg.text) {
                auto [ox, oy] = spriteFont->GetCharOffset(c);

                float dx = penX + ox * param.scale + offsetX;
                float dy = drawY + oy * param.scale + offsetY;

                spriteFont->DrawGlyph(dx, dy, c, param);

                float advance = (spriteFont->GetCharPixelWidth(c) + param.letterSpacing + spriteFont->GetCharKerning(c)) * param.scale;
                penX += advance;
            }
        }
    }
}

// フォント未設定時のフォールバック描画
void Text::DrawFallbackText(const std::vector<std::string>& lines, const Quad2D& quad) {
    const int defaultFontHeight = static_cast<int>(fontSize * lineSpacing);
    float width = quad.topRight.x - quad.topLeft.x;
    float height = quad.bottomLeft.y - quad.topLeft.y;

    float baseY = quad.topLeft.y;

    if (anchor == TextAnchor::MiddleLeft || anchor == TextAnchor::MiddleCenter || anchor == TextAnchor::MiddleRight)
        baseY += (height - defaultFontHeight * lines.size()) / 2.f;
    else if (anchor == TextAnchor::LowerLeft || anchor == TextAnchor::LowerCenter || anchor == TextAnchor::LowerRight)
        baseY += (height - defaultFontHeight * lines.size());

    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
        const std::string& line = lines[i];
        int lineWidth = GetDrawStringWidth(line.c_str(), (int)line.size());

        float drawX = quad.topLeft.x;
        if (anchor == TextAnchor::UpperCenter || anchor == TextAnchor::MiddleCenter || anchor == TextAnchor::LowerCenter) {
            drawX += (width - lineWidth) / 2.f;
        }
        else if (anchor == TextAnchor::UpperRight || anchor == TextAnchor::MiddleRight || anchor == TextAnchor::LowerRight) {
            drawX += (width - lineWidth);
        }
        // Left: drawX はそのまま

        float drawY = baseY + i * defaultFontHeight;

        DrawString(
            static_cast<int>(drawX),
            static_cast<int>(drawY),
            line.c_str(),
            color.ToPackedRGB()
        );
    }
}

