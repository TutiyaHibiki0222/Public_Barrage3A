/*
    ◆ Text.cpp

    クラス名        : Text クラス
    作成日          : 2025/06/23 (月) 13:58:05
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "UiBase.h"
#include "IDraw.h"
#include "Font.hpp" 
#include "SpriteFont.h"


// テキストのアンカー位置を表す列挙型.
enum class TextAnchor {
	UpperLeft,      // 左上.
	UpperCenter,    // 中央上.
	UpperRight,     // 右上.
	MiddleLeft,     // 左中央.
	MiddleCenter,   // 中央.
	MiddleRight,    // 右中央.
	LowerLeft,      // 左下.
	LowerCenter,    // 中央下.
	LowerRight      // 右下.
};

// 水平方向のオーバーフローを表す列挙型.
enum class HorizontalOverflow {
	Wrap,       // 文字がはみ出す場合、テキストを切り捨てる.
	Overflow    // 文字がはみ出しても描画する。
};
// 垂直方向のオーバーフローを表す列挙型.
enum class VerticalOverflow {
	Truncate,   // テキストがはみ出す場合、切り捨てる.
	Overflow    // テキストがはみ出す場合、切り捨てるか、表示可能な領域を超えて表示する.
};

struct TextLayout {
    int    fontHandle;      // DxLib フォントハンドル（font がある場合）
    float  scaleX, scaleY;  // X/Y の拡大率
    float  baseX, baseY;    // 左上原点からのオフセット
    int    lineHeight;      // １行分の高さ
    int    maxLines;        // 実際に描画する行数

    float  width;
};

// Text class and AppBase class
class Text : public UiBase {
private:    // 非公開.
    std::string text = "";
	int  fontSize = 24;                     // フォントサイズ（デフォルトは24）
	std::shared_ptr<Font> font = nullptr;   // フォントハッシュ.
    std::shared_ptr<GameEngine::SpriteFont> spriteFont; // 画像型のフォント.
	float lineSpacing = 1.0f; // 行間の設定（デフォルトは1.0）
	Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);

    TextAnchor anchor = TextAnchor::MiddleCenter;
    HorizontalOverflow  hOverflow = HorizontalOverflow::Wrap;
    VerticalOverflow    vOverflow = VerticalOverflow::Truncate;

	bool bestFit = false; // ベストフィットの有無
    float minSize = 0.0f; // 最小フォントサイズ
    float maxSize = 64.0f; // 最大フォントサイズ
    bool autoComma = false; // 自動カンマ挿入を有効にする

public:     // 公開.
    // コンストラクタ.
    Text();
    Text(std::shared_ptr<GameObject>);

    void SetText(const std::string& t) { text = t; }
    const std::string& GetText() const { return text; }

    void SetFont(const std::shared_ptr<Font>& f) { font = f; }
    const std::shared_ptr<Font>& GetFont() const { return font; }

    void SetSpriteFont(const std::shared_ptr<GameEngine::SpriteFont>& f) { spriteFont = f; }
    const std::shared_ptr<GameEngine::SpriteFont>& GetSpriteFont() const { return spriteFont; }

	void SetLineSpacing(float spacing) { lineSpacing =  spacing; }

    void SetColor(const Color& c) { color = c; }
    const Color& GetColor() const { return color; }

    void SetTextAnchor(TextAnchor align) { anchor = align; }

    void SetHorizontalOverflow(HorizontalOverflow val) { hOverflow = val; }
    void SetVerticalOverflow(VerticalOverflow val) { vOverflow = val; }

	void SetBestFit(bool fit)   { bestFit = fit;  }
	bool GetBestFit() const     { return bestFit; }
	void SetMinSize(float size) { minSize = size; }
	float GetMinSize() const    { return minSize; }
	void SetMaxSize(float size) { maxSize = size; }
	float GetMaxSize() const    { return maxSize; }

    void SetFontSize(int size)  { fontSize = size;}

    void Awake() override;
private:
    void OnUiDraw(const Quad2D& quad, const std::shared_ptr<Canvas>& rect) override;
    std::vector<std::string> WrapText(const std::string& text, int fontHandle, float maxWidth);
    std::vector<std::string> WrapRichText(const std::string& richText, float maxWidth, GameEngine::SpriteFont* spriteFont, float baseFontSize);
    int GetMaxLineWidth(const std::vector<std::string>& lines, int fontHandle);
    // 横Overflow時：指定幅内でテキストを切り捨てる
    std::string TruncateTextToWidth(const std::string& text, int fontHandle, float maxWidth);

    TextLayout ComputeLayout(const std::vector<std::string>& lines, const Quad2D& quad, float width, float height);
    
    std::vector<std::string> PrepareLines(const std::string& text, float width);
    void DrawWithDxFont(const std::vector<std::string>& lines, const TextLayout& lo);
    void DrawWithSpriteFont(const std::vector<std::string>& lines, const TextLayout& lo);
    void DrawFallbackText(const std::vector<std::string>& lines, const Quad2D& quad);
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Text>(*this);
    }

};