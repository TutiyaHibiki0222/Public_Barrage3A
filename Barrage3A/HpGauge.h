/*
    ◆ HpGauge.h

    クラス名        : HpGauge クラス
    作成日          : 2025/09/21 (日) 23:45:01
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

struct OutsideLine {
    float range = 0.0f;
    float size = 1.0f;
    Color color = Color(255, 0, 0, 175);
};

// HpGauge class and AppBase class
class HpGauge : public AppBase, public IRendererDraw, public std::enable_shared_from_this<HpGauge>
{
private:    // 非公開.

    std::shared_ptr<Sprite> sprite;
    std::shared_ptr<Sprite> point;
    std::vector<OutsideLine> outsideLines;

    Color color = Color(255, 255, 255, 175);
    SortingLayer sortingLayer{ 20 };

    // ゲージ設定
    double maxHp = 100.0;           // デフォルト 100（変更可能）
    double currentHp = 100.0;       // 実際に描画される現在値（補間済み）
    double targetHp = 100.0;        // 外部から SetHp() された目標値
    float scale = 1.0f;
    float range = 168.0f / 2.0f;    // 半径

    // スペルカードエリア.
    std::vector<double> dividers = { 20,50 };


public:     // 公開.

    // コンストラクタ.
    HpGauge();
    HpGauge(std::shared_ptr<GameObject>);

    void SetSprite(const Sprite& s) { sprite = std::make_shared<Sprite>(s); }
    void SetSprite(const std::shared_ptr<Sprite>& s) { sprite = s; }
    void SetPointSprite(const Sprite& s) { point = std::make_shared<Sprite>(s); }
    void SetPointSprite(const std::shared_ptr<Sprite>& s) { point = s; }

	void SetRange(float r) { range = r; }

    void SetMaxHp(double m) {
        maxHp = (m > 0.0) ? m : 1.0;
        targetHp = std::clamp(targetHp, 0.0, maxHp);
        currentHp = std::clamp(currentHp, 0.0, maxHp);
    }

    void AddOutsideLine(float range, float size = 1.0f, const Color& col = Color(255, 0, 0, 175)) {
        outsideLines.push_back({ range, size, col });
    }

    void SetHp(double hp) {
        currentHp = std::clamp(hp, 0.0, maxHp);
    }

    void Damage(double value) {
        SetHp(currentHp - value);
    }

    void Heal(double value) {
        SetHp(currentHp + value);
    }

    double GetHp() const { return currentHp; }
    double GetHpRate() const { return currentHp / maxHp; }
    double GetHpPercent() const {
        return (maxHp > 0) ? (currentHp * 100 / maxHp) : 0;
    }
    // AppBase 関数.
    void Awake()    override;
    void OnDestroy()   override;
    void Draw()     override;
    int  GetSortingOrder() const override { return sortingLayer.GetSortingOrder(); }
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<HpGauge>(*this);
    }
};