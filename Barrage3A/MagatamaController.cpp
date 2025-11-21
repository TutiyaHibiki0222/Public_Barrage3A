/*
    ◆ MagatamaController.cpp

    クラス名        : MagatamaController クラス
    作成日          : 2025/07/10 (木) 0:50:18
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "MagatamaController.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "SpriteRenderer.h"
#include "Magatama.h"

// コンストラクタ 
MagatamaController::MagatamaController() : AppBase("MagatamaController"){

}
MagatamaController::MagatamaController(std::shared_ptr<GameObject> owner) : AppBase("MagatamaController", owner) {

}

// AppBase Event.
void MagatamaController::Init(int max, std::shared_ptr<GameEngine::Sprite> spriteTemplate, Vector2D pos) {
    magatamas.clear(); // 念のため初期化

    for (int i = 0; i < max; ++i) {
        auto obj = GameObject::Instantiate();
        auto mag = obj->AddAppBase<Magatama>();
        mag->player = this->gameObject.lock();
        mag->currentOffset = pos; // 初期位置はゼロ
        // SpriteRenderer設定
        auto renderer = obj->AddAppBase<SpriteRenderer>();
        renderer->SetSprite(spriteTemplate);
        renderer->SetLayer(2);
        obj->SetActive(false);
        magatamas.push_back(mag);
    }
}

void MagatamaController::SetMagatamaCount(int index) {
    for (int i = 0; i < magatamas.size(); ++i) {
        bool active = (i < index);
        magatamas[i]->GetGameObject()->SetActive(active);

        if (active) {
            magatamas[i]->normalOffset = normalOffsets[index - 1][i];
            magatamas[i]->focusedOffset = focusedOffsets[index - 1][i];
            magatamas[i]->currentOffset = magatamas[i]->normalOffset;

            magatamas[i]->GetGameObject()->transform->position = transform->position;
        }
    }
}

std::vector<Vector2D> MagatamaController::GetMagatamaPositions() {
    std::vector<Vector2D> poss;
    for (auto obj : magatamas) {
        if (auto os = obj->GetGameObject()) {
            if(os->IsActive()) poss.push_back(os->transform->position);
        }
    }
    std::sort(poss.begin(), poss.end(), [](const Vector2D& a, const Vector2D& b) {
        return a.x > b.x;
        });
    return poss;
}
