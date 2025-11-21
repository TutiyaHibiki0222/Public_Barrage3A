#pragma once
#include "headers.h"
#include "AppBase.h"
#include "RectTransform2D.h"
#define UI_BASE_ON_EVENT_MESSAGE (false)

// UiBase class and AppBase class
class UiBase : public AppBase {
    friend class GraphicRaycaster;
    friend class Canvas;
protected:
    std::weak_ptr<RectTransform2D> rectTransform;
    bool raycastTarget      = true;
    bool isHoveredLastFrame = false;
    Vector2D dragStartPos;
    bool isRenderer = true;
    // コンストラクタ.
    UiBase();
    UiBase(std::shared_ptr<GameObject>);

public:

    std::shared_ptr<RectTransform2D> GetRectTransform();

protected:     // friend 継承 classのみ.

    void UiBaseDraw(const std::shared_ptr<Canvas> canvas);

    bool Raycast(const Vector2D& clickPosition) {
        if (!raycastTarget)
            return false;

        auto rt = rectTransform.lock();
        if (!rt)
            return false;

        Quad2D quad = rt->GetWorldQuad();
        return IsPointInQuad(clickPosition, quad);
    }
    //----------- イベント -------------//

    virtual void OnUiDraw(const Quad2D& quad, const std::shared_ptr<Canvas>& canvas) {}

    // クリックイベント(離された際)
    virtual void OnClick() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "クリックイベントが発生しました。" << std::endl;
#endif
    }
    
    // ダブルクリックイベント
    virtual void OnDoubleClick() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "ダブルクリックイベントが発生しました。" << std::endl;
#endif
    }
    
    // 右クリックイベント
    virtual void OnRightClick() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "右クリックイベントが発生しました。" << std::endl;
#endif
    }
    
    // 触れている間.
    virtual void OnHover() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "ホバーイベントが発生しました。" << std::endl;
#endif
    }
    // 触れた瞬間.
    virtual void OnFocus() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "フォーカスを取得しました。" << std::endl;
#endif
    }
    // 離れた瞬間.
    virtual void OnFocusLost() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "フォーカスが失われました。" << std::endl;
#endif
    }
    // ドラッグ(長押し中動かしたら).
    virtual void OnDrag() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "ドラッグイベントが発生しました。" << std::endl;
#endif
    }
    // ドラッグ(長押しした瞬間).
    virtual void OnBeginDrag() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "ドラッグイベントが開始しました。" << std::endl;
#endif
    }
    // ドラッグ(長押しから離した瞬間).
    virtual void OnEndDrag() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "ドラッグイベントが終了しました。" << std::endl;  
#endif
    }

    virtual void OnRelease() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "リリースイベントが発生しました。" << std::endl;
#endif
    }

    virtual void OnMouseDown() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "マウスボタンが押されました。" << std::endl;
#endif
    }

    virtual void OnMouseUp() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "マウスボタンが離されました。" << std::endl;
#endif
    }

    virtual void OnMouseEnter() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "マウスがオブジェクトに入ったときのイベントが発生しました。" << std::endl;
#endif
    }

    virtual void OnMouseExit() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "マウスがオブジェクトから出たときのイベントが発生しました。" << std::endl;
#endif
    }

    virtual void OnScroll(int scrollAmount) {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "スクロールイベントが発生しました。スクロール量: " << scrollAmount << std::endl;
#endif
    }

    virtual void OnKeyPress(int keyCode) {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "キーが押されました。キーコード: " << keyCode << std::endl;
#endif
    }

    virtual void OnKeyRelease(int keyCode) {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "キーが離されました。キーコード: " << keyCode << std::endl;
#endif
    }

    virtual void OnTouch() {
#if UI_BASE_ON_EVENT_MESSAGE
        std::cout << "タッチイベントが発生しました。" << std::endl;
#endif
    }
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<UiBase>(*this);
    }

private:
    bool IsPointInTriangle(const Vector2D& p, const Vector2D& a, const Vector2D& b, const Vector2D& c) {
        auto Sign = [](const Vector2D& p1, const Vector2D& p2, const Vector2D& p3) {
            return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
            };

        bool b1 = Sign(p, a, b) <= 0.0f;
        bool b2 = Sign(p, b, c) <= 0.0f;
        bool b3 = Sign(p, c, a) <= 0.0f;

        return (b1 == b2) && (b2 == b3);
    }


    bool IsPointInQuad(const Vector2D& p, const Quad2D& quad) {
        return  IsPointInTriangle(p, quad.topLeft, quad.topRight, quad.bottomLeft) ||
                IsPointInTriangle(p, quad.topLeft, quad.bottomRight, quad.bottomLeft);
    }
    std::shared_ptr<Canvas> GetRootCanvas() const;
};