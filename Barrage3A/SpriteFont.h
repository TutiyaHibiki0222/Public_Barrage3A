#include <unordered_map>
#include "IDraw.h"

#ifndef _SpriteFont
#define _SpriteFont

namespace GameEngine {

    class SpriteFont {
    private:
        struct pair_hash {
            template <class T1, class T2>
            std::size_t operator()(const std::pair<T1, T2>& p) const {
                return std::hash<T1>{}(p.first) ^ (std::hash<T2>{}(p.second) << 1);
            }
        };


        int fontImage = -1;
        int glyphWidth = 16;
        int glyphHeight = 16;
        int cols = 16;

        float baseScaleAdjustment = 1.0f; // デフォルト1.0（調整なし）

        std::unordered_map<char, int> charToIndex;          // 文字→インデックス
        std::unordered_map<char, int> charPixelWidthTable;  // 文字→ピクセル幅
        std::unordered_map<char, float> charKerningTable;    // 文字→追加/減算スペーシング
        std::unordered_map<char, std::pair<float, float>> charOffsetTable; // 文字ごとの描画オフセット {offsetX, offsetY}
        // 文字ペア（直前文字＋現在文字）ごとのカーニング調整テーブル
        std::unordered_map<std::pair<char, char>, float, pair_hash> charPairKerningTable;

    public:
        struct DrawParam {
            float fontSize = 16.0f;
            float letterSpacing = 0.0f;
            Color color = Color(1, 1, 1, 1);
            bool useAlphaBlend = true;
            float scale = 1.0f;

            bool bold = false;
            bool italic = false;
            int outlineWidth = 0; // 縁取りの幅（0なら縁取りなし）

            int bgColor = -1; // 背景色（-1で無し）
        };

        float ComputeScale(float fontSize) const {
            return fontSize / static_cast<float>(glyphHeight) * baseScaleAdjustment;
        }

        void SetCharPixelWidth(char c, int pixelWidth);
        int GetCharPixelWidth(char c) const;
        // 追加: 特定文字のカーニング調整値（正の値で間隔拡大、負の値で詰める）
        void SetCharKerning(char c, float adjustment) {
            charKerningTable[c] = adjustment;
        }
        void SetCharKerning(const std::string& chars, float adjustment) {
            for (char c : chars) {
                charKerningTable[c] = adjustment;
            }
        }
        float GetCharKerning(char c) const {
            auto it = charKerningTable.find(c);
            if (it != charKerningTable.end()) return it->second;
            return 0.0f;
        }
        void SetCharKerning(char prevChar, char c, float adjustment) {
            charPairKerningTable[{prevChar, c}] = adjustment;
        }

        float GetCharKerning(char prevChar, char c) const {
            auto it = charPairKerningTable.find({ prevChar, c });
            if (it != charPairKerningTable.end()) return it->second;
            return 0.0f;
        }

        // 文字ごとの描画オフセットを設定 (offsetX, offsetY) ※pixel単位
        void SetCharOffset(char c, float offsetX, float offsetY) {
            charOffsetTable[c] = { offsetX, offsetY };
        }

        // 文字ごとの描画オフセット取得
        std::pair<float, float> GetCharOffset(char c) const {
            auto it = charOffsetTable.find(c);
            if (it != charOffsetTable.end()) return it->second;
            return { 0.0f, 0.0f };
        }

        bool LoadFontTextureWithMap(const std::string& filePath, const std::string& characters, int glyphW, int glyphH, int cols);

        float MeasureTextWidth(const std::string& text, const DrawParam& param) const;
        std::vector<std::string> WrapText(const std::string& text, float maxWidth, const DrawParam& param) const;
        void DrawGlyph(float x, float y, char c, const DrawParam& param) const;
        void DrawGlyphRotated(float x, float y, char c, const DrawParam& param, float angleRad) const;

        int GetGlyphWidth() const { return glyphWidth; }
        int GetGlyphHeight() const { return glyphHeight; }

        void SetBaseScaleAdjustment(float scale) { baseScaleAdjustment = scale; }
        float GetBaseScaleAdjustment() const { return baseScaleAdjustment; }

        void DrawString(const std::string& text, float x, float y, const DrawParam& param) const;
    };

} // namespace GameEngine

#endif // !_SpriteFont