// SpriteFont.cpp
#include "SpriteFont.h"
#include "DxLib.h"

using namespace GameEngine;

void SpriteFont::SetCharPixelWidth(char c, int pixelWidth) {
    charPixelWidthTable[c] = pixelWidth;
}

int SpriteFont::GetCharPixelWidth(char c) const {
    auto it = charPixelWidthTable.find(c);
    return (it != charPixelWidthTable.end()) ? it->second : glyphWidth;
}

bool SpriteFont::LoadFontTextureWithMap(const std::string& filePath, const std::string& characters, int glyphW, int glyphH, int cols_) {
    fontImage = LoadGraph(filePath.c_str());
    if (fontImage == -1) return false;

    glyphWidth = glyphW;
    glyphHeight = glyphH;
    cols = cols_;
    charToIndex.clear();

    for (size_t i = 0; i < characters.size(); ++i) {
        charToIndex[characters[i]] = static_cast<int>(i);
    }

    return true;
}

float SpriteFont::MeasureTextWidth(const std::string& text, const DrawParam& param) const {
    float total = 0.0f;
    float s = param.scale;
    float penX = 0.0f;

    for (size_t i = 0; i < text.size(); ++i) {
        char c = text[i];
        auto [offX, offY] = GetCharOffset(c);
        float charWidth  = (float)GetCharPixelWidth(c);

        // 文字のオフセット分を加味してpenXを調整
        penX += offX * s;

        // advance幅は文字幅＋letterSpacing＋kerningを加算
        float advance = (charWidth + param.letterSpacing + GetCharKerning(c)) * s;

        penX += advance;
    }
    return penX;
}



std::vector<std::string> SpriteFont::WrapText(const std::string& text, float maxWidth, const DrawParam& param) const {
    std::vector<std::string> lines;
    std::string currentLine;

    for (char c : text) {
        if (c == '\n') {
            lines.push_back(currentLine);
            currentLine.clear();
            continue;
        }

        std::string trialLine = currentLine + c;
        if (MeasureTextWidth(trialLine, param) > maxWidth && !currentLine.empty()) {
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

void SpriteFont::DrawGlyph(float x, float y, char c, const DrawParam& param) const {
    if (fontImage == -1) return;

    auto it = charToIndex.find(c);
    if (it == charToIndex.end()) return;

    int index = it->second;
    int col = index % cols;
    int row = index / cols;

    int srcX = col * glyphWidth;
    int srcY = row * glyphHeight;

    float pixelW = static_cast<float>(GetCharPixelWidth(c));
    float dstW = pixelW * param.scale;
    float dstH = glyphHeight * param.scale;
    // --- 背景色描画 ---
    if (param.bgColor != -1) {
        Color bg(param.bgColor);

        if (param.useAlphaBlend)
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, bg.A255());

        SetDrawBright(bg.R255(), bg.G255(), bg.B255());

        DrawBoxAA(
            x, y,
            x + dstW, y + dstH,
            GetColor(bg.R255(), bg.G255(), bg.B255()),
            TRUE
        );

        if (param.useAlphaBlend)
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        SetDrawBright(255, 255, 255);
    }

    if (param.outlineWidth > 0) {
        // 黒の縁取り色（必要に応じて変更可能）
        Color outlineColor(0, 0, 0, 255);

        // 縁取り描画（上下左右および斜めも）
        for (int dx = -param.outlineWidth; dx <= param.outlineWidth; ++dx) {
            for (int dy = -param.outlineWidth; dy <= param.outlineWidth; ++dy) {
                if (dx == 0 && dy == 0) continue; // 中心は後で描く

                if (param.useAlphaBlend)
                    SetDrawBlendMode(DX_BLENDMODE_ALPHA, outlineColor.A255());

                SetDrawBright(outlineColor.R255(), outlineColor.G255(), outlineColor.B255());

                DrawRectExtendGraphF(
                    x + dx, y + dy,
                    x + dx + dstW, y + dy + dstH,
                    srcX, srcY, glyphWidth, glyphHeight,
                    fontImage, TRUE
                );

                if (param.useAlphaBlend)
                    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

                SetDrawBright(255, 255, 255);
            }
        }
    }

    // 本体の文字描画
    if (param.useAlphaBlend)
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, param.color.A255());

    SetDrawBright(param.color.R255(), param.color.G255(), param.color.B255());

    DrawRectExtendGraphF(
        x, y,
        x + dstW, y + dstH,
        srcX, srcY, glyphWidth, glyphHeight,
        fontImage, TRUE
    );

    if (param.useAlphaBlend)
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    SetDrawBright(255, 255, 255);
}


void SpriteFont::DrawGlyphRotated(float x, float y, char c, const DrawParam& param, float angle) const {
    if (fontImage == -1) return;

    auto it = charToIndex.find(c);
    if (it == charToIndex.end()) return;

    int index = it->second;
    int col = index % cols;
    int row = index / cols;

    int srcX = col * glyphWidth;
    int srcY = row * glyphHeight;

    float pixelW = static_cast<float>(GetCharPixelWidth(c));
    float dstW = pixelW * param.scale;
    float dstH = glyphHeight * param.scale;

    if (param.useAlphaBlend)
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, param.color.A255());

    SetDrawBright(param.color.R255(), param.color.G255(), param.color.B255());

    // 中心位置を計算（DrawRectRotaGraph3F は中心位置が必要）
    float centerX = x + dstW / 2.0f;
    float centerY = y + dstH / 2.0f;

    // 拡大率X, Yを分けて設定
    float scaleX = dstW / glyphWidth;
    float scaleY = dstH / glyphHeight;

    // DrawRectRotaGraph3F を使って回転描画
    DrawRectRotaGraph3F(
        centerX, centerY,                   // 描画中心位置
        srcX, srcY,                         // ソース画像の位置
        glyphWidth, glyphHeight,            // ソース画像のサイズ
        dstW / 2.0f, dstH / 2.0f,           // 回転中心（描画内での中心）
        scaleX, scaleY,                     // 拡大率
        angle,                              // 回転角度（ラジアンではなく度）
        fontImage, TRUE                     // グラフィックハンドル, 透過描画ON
    );

    if (param.useAlphaBlend)
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    SetDrawBright(255, 255, 255);
}


void SpriteFont::DrawString(const std::string& text, float x, float y, const DrawParam& param) const {
    float scale = ComputeScale(param.fontSize) * param.scale;
    float posX = x;
    float posY = y;

    // 透明度設定（255が不透明、0が透明）
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, param.color.A255());
    SetDrawBright(param.color.R255(), param.color.G255(), param.color.B255());

    float spacing = param.letterSpacing * scale;
    char prevChar = 0;
    for (char c : text) {
        if (prevChar != 0) {
            posX += GetCharKerning(prevChar, c) * scale;
        }

        auto [offsetX, offsetY] = GetCharOffset(c);
        offsetX *= scale;
        offsetY *= scale;

        int glyphW = GetCharPixelWidth(c);
        float drawX = posX + offsetX;
        float drawY = posY + offsetY;

        auto it = charToIndex.find(c);
        if (it != charToIndex.end()) {
            int index = it->second;
            int col = index % cols;
            int row = index / cols;
            DrawRectGraph(drawX, drawY, col * glyphWidth, row * glyphHeight, glyphWidth, glyphHeight, fontImage, TRUE);
        }

        posX += glyphW * scale + spacing;
        prevChar = c;
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // 元に戻す
    SetDrawBright(255, 255, 255);
}
