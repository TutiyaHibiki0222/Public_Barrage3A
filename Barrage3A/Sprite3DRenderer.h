/*
    ◆ Sprite3DRenderer.h

    クラス名        : Sprite3DRenderer クラス
    作成日          : 2025/07/27 (日) 22:17:23
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "IDraw.h"
#include "Texture2D.h"

using namespace GameEngine;

// Sprite3DRenderer class and AppBase class
class Sprite3DRenderer : public AppBase, public IRendererDraw, public std::enable_shared_from_this<Sprite3DRenderer> {
private:    // 非公開.
    std::shared_ptr<Sprite> sprite;
    SortingLayer sortingLayer;
    bool isGameObjectZrotFriend = false;
	Vector3D rotation = { 0.0f, 0.0f, 0.0f }; // 3D空間での回転
    Color color = Color(0xFFFFFF);
    Vector2D anchor = { 0.5f, 0.5f };
    Vector2D offset = { 0.0f, 0.0f }; // オフセット位置
public:     // 公開.

    // コンストラクタ.
    Sprite3DRenderer();
    Sprite3DRenderer(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Awake()    override;
    void OnDestroy()override;
    // IRendererDraw 関数.
    bool IsDraw()   override;
    void Draw()     override;
    RectF GetAABB() const override;

    void SetSprite(const Sprite& s) { sprite = std::make_shared<Sprite>(s); }
    void SetSprite(const std::shared_ptr<Sprite>& s) { sprite = s; }
    std::shared_ptr<Sprite> GetSprite() const { return sprite; }


    void SetAnchor(const Vector2D& a) { anchor = a; }
    Vector2D GetAnchor() const { return anchor; }

    void SetOffset(const Vector2D& o) { offset = o; }
    Vector2D GetOffset() const { return offset; }

    void SetLayer(int);
    int  GetLayer();
    Vector2DInt GetSize();

    int GetSortingOrder() const override { return sortingLayer.GetSortingOrder(); }

    SortingLayer& GetSortingLayer() { return sortingLayer; }
    Color& GetColor() { return color; }
    void SetColor(const Color& c) { color = c; }

	Vector3D& GetRotation() { return rotation; }
    void SetRotation(const Vector3D& r) { rotation = r; }
    void SetGameObjectZrotFriend(bool isFriend) { isGameObjectZrotFriend = isFriend; }
	bool IsGameObjectZrotFriend() const { return isGameObjectZrotFriend; }
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Sprite3DRenderer>(*this);
    }
};