/*
    ◆ ScoreEffect.h

    クラス名        : ScoreEffect クラス
    作成日          : 2025/07/21 (月) 19:23:10
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "SpriteFont.h"
#include "IDraw.h"
// ScoreEffect class and AppBase class
class ScoreEffect : public AppBase, public std::enable_shared_from_this<ScoreEffect>, public IRendererDraw
{
private:    // 非公開.
    int score = 0;                         // 表示するスコア
    float lifetime = 0.0f;                // 経過時間
    float duration = 1.f;                 // 表示時間（秒）
    float floatSpeed = 30.0f;             // 上昇速度（px/sec）
    Color color = Color(255,255,255,255); // 透明度
    Vector2D initialPosition;             // 初期位置
    std::shared_ptr<GameEngine::SpriteFont> font;     // 数字フォント
public:     // 公開.

    // コンストラクタ.
    ScoreEffect();
    ScoreEffect(std::shared_ptr<GameObject>);

    void SetFont(const std::shared_ptr<GameEngine::SpriteFont>& f) {
        font = f;
	}

    void SetScore(int s) {
        score = s;
	}

    void SetColor(const Color& c) {
        color = c;
    }

    // AppBase 関数.
	void Awake()    override;
    void Update()   override;
	void OnDestroy() override;

    void Draw() override;

    bool IsDraw() override {
        return true;
    }

protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<ScoreEffect>(*this);
    }
};