/*
    ◆ PixelShaderBase.h

    クラス名        : PixelShaderBase クラス
    作成日          : 2025/07/27 (日) 14:14:41
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "IDraw.h"
// PixelShaderBase class and AppBase class
class PixelShaderBase : public AppBase, public IRendererDraw, public std::enable_shared_from_this<PixelShaderBase>
{
protected:    // 非公開.
    int shader = -1;
    int scene  = -1;
	SortingLayer sortingLayer; // 描画順を決定するためのレイヤーとオーダー.
    
    VERTEX2DSHADER vertex[4];
    
    // コンストラクタ.
    PixelShaderBase();
    PixelShaderBase(std::shared_ptr<GameObject>);
public:     // 公開.

    ~PixelShaderBase();

    // AppBase 関数.
	void Awake()        override;
    void Start()        override;
	void OnDestroy()    override;

    // 描画周り.
    bool IsDraw()   override;
    void Draw()     override;
    int  GetSortingOrder() const override {return sortingLayer.GetSortingOrder();}

    void SetSortingLayer(const SortingLayer& layer) {
        sortingLayer = layer;
	}
    void SetLayer(int layer) {
        sortingLayer.layer = layer;
    }
    void SetOrder(int order) {
        sortingLayer.orderInLayer = order;
	}
protected:
    virtual void OnUpdateShader()   = 0;
    virtual void LoadShader()       = 0;
	virtual void InitializeShader() = 0;
protected:
    virtual std::shared_ptr<AppBase> Clone() const override = 0;
};