/*
    ◆ HpGauge.cpp

    クラス名        : HpGauge クラス
    作成日          : 2025/09/21 (日) 23:45:01
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "HpGauge.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "RendererManager.h"

// コンストラクタ 
HpGauge::HpGauge() : AppBase("HpGauge"){

}
HpGauge::HpGauge(std::shared_ptr<GameObject> owner) : AppBase("HpGauge", owner) {

}

// AppBase Event.
void HpGauge::Awake() {
	RendererManager::GetInstance().AddRenderer(shared_from_this());
}

void HpGauge::OnDestroy() {
    RendererManager::GetInstance().Remove(shared_from_this());
}

void HpGauge::Draw() {
    if (!sprite) return;

	Vector2D pos = transform->GetWorldPosition();
    
    SetDrawBright(color.R255(), color.G255(), color.B255());
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, color.A255());

	// ゲージ本体
    DrawCircleGaugeF(pos.x, pos.y, GetHpPercent(), sprite->spriteData, 0, scale, TRUE);
    	
    // 外枠.
    for (const auto& line : outsideLines) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, line.color.A255());
        DrawCircleAA(pos.x, pos.y,line.range,100, line.color.ToPackedRGB(), FALSE,line.size);
    }

    // ポイント.
    if (point) {
        for (double divider : dividers) {
            if (GetHpPercent() <= divider) {
                continue;
            }

            // HP がまだ上ならポイント描画
            float angle = 360.0f * (1.0f - (float)divider / 100.0f);
            float rad = Mathf::DegToRad(angle - 90.0f);

            Vector2D pointPos = {
                pos.x + cosf(rad) * range * scale,
                pos.y + sinf(rad) * range * scale
            };

            DrawRotaGraphFast3F(
                pointPos.x,
                pointPos.y,
                point->width * 0.5f,
                point->height * 0.5f,
                scale,
                scale,
                rad + Mathf::DegToRad(90.0f),
                point->spriteData,
                TRUE
            );
        }
    }

    SetDrawBright(255, 255, 255);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}