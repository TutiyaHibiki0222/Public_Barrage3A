/*
    ◆ ShapesRenderer.cpp

    クラス名        : ShapesRenderer クラス
    作成日          : 2025/06/26 (木) 14:16:48
    最終変更日      : 2025/06/26 (木) 22:18:05
    作成者          : 𡈽屋 響
    概要            : 図形を描画するComponent
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "Filter.hpp"
#include "IDraw.h"

enum class ShapeType {
	Circle,     // 円.
	Rectangle   // 四角形.
};

// ShapesRenderer class and AppBase class
class ShapesRenderer : public AppBase, public IRendererDraw, public std::enable_shared_from_this<ShapesRenderer>
{
private:    // 非公開.
    
	ShapeType shapeType = ShapeType::Circle;    // 描画する形状のタイプ.
	Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	SortingLayer sortingLayer;                  // 描画順序を決定するためのレイヤーとオーダー.
	int   data = -1;                            // 描画する形状のデータ（例: 半径、幅、高さなど）.
	float size = 64;                            // 描画する形状のサイズ.

	std::shared_ptr<Filter> filter = nullptr; // フィルター設定.
public:     // 公開.

    // コンストラクタ.
    ShapesRenderer();
    ShapesRenderer(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Awake()     override;
	void OnDestroy() override;
    // IRendererDraw 関数.
    bool  IsDraw()    override;
    void  Draw()      override;
    int   GetSortingOrder() const override; // 描画順序は常に最大値.
    RectF GetAABB() const override;

	// 描画する形状のタイプを設定.
    void SetShapeType(ShapeType type);

    float GetSize() const { return size; }
    void SetSize(float s) { size = s; }
    void SetColor(const Color& col) { color = col; }
    SortingLayer& GetSortingLayer() { return sortingLayer; }

    // 
    void SetFilter(std::shared_ptr<Filter> _filter) {
        filter = _filter;
	}
	// フィルターを取得.
    std::shared_ptr<Filter> GetFilter() const {
        return filter;
	}
private:
    void DrawShape(const Vector2D& pos, const Vector2D& scale, float angle);
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<ShapesRenderer>(*this);
    }
};