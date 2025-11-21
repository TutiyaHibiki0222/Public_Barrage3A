/*
    ◆ ScoreEffect.cpp

    クラス名        : ScoreEffect クラス
    作成日          : 2025/07/21 (月) 19:23:10
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "ScoreEffect.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "RendererManager.h"
// コンストラクタ 
ScoreEffect::ScoreEffect() : AppBase("ScoreEffect"){

}
ScoreEffect::ScoreEffect(std::shared_ptr<GameObject> owner) : AppBase("ScoreEffect", owner) {

}

// AppBase Event.
void ScoreEffect::Awake() {
	RendererManager::GetInstance().AddRenderer(shared_from_this());
}

void ScoreEffect::Update() {
    lifetime += Time.deltaTime;

    float t = lifetime / duration;
    if (t >= 1.0f) {
        Destroy(gameObject);  // 自動削除
        return;
    }

    color.a = 1.0f - t;

    // 浮かび上がる
    if (auto trs = transform.lock()) {
        trs->position.y += floatSpeed * Time.deltaTime;
    }

    // Update内
    if (lifetime >= duration) {
        Destroy();
        return;
    }

}

void ScoreEffect::OnDestroy() {
    RendererManager::GetInstance().Remove(shared_from_this());
}


void ScoreEffect::Draw() {
    if (!font) return;

    GameEngine::SpriteFont::DrawParam param;
    param.color = color; // alphaを透過値としてセット
    param.fontSize = 11.0f; // 適宜調整
    param.scale = 0.6f;

	Vector2D pos = transform->GetWorldPosition();

    font->DrawString(std::to_string(score), pos.x, pos.y, param);
}
