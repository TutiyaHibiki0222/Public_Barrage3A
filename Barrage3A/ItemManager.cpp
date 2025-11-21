#include "ItemManager.h"
#include "GameManager.h"
#include "SpriteRenderer.h"
#include "Texture2DShortcut.hpp"
#include "ArrowPointer.h"
using namespace GameEngine;

void ItemManager::AddItem(const std::shared_ptr<ItemBase>& item) {
    items.emplace_back(item);
}

void ItemManager::RemoveItem(const std::shared_ptr<ItemBase>& item) {
    items.erase(
        std::remove_if(items.begin(), items.end(),
            [&](const std::weak_ptr<ItemBase>& wp) {
                return wp.lock() == item;
            }),
        items.end()
    );
}

void ItemManager::Update() {
    auto player = GameManager::GetInstance().GetPlayer();
    if (!player) return;

    bool shouldAutoCollect = (player->transform->position.y > AUTO_COLLECT_BORDER_Y);

    if (shouldAutoCollect) {
        for (auto& ws : items) {
            if (auto item = ws.lock()) {           
                item->AutoCollect();
            }
        }
    }
}

void ItemManager::Clear() {
    items.clear();
}

void ItemManager::CreateItem(ItemType itemType, Vector2D pos) {
    auto itemObj = GameObject::Instantiate("Item");
    itemObj->transform->position = pos;
    auto item = itemObj->AddAppBase<ItemBase>();
    item->SetType(itemType);
    std::string key = "Items/Item:";
    switch (itemType)
    {
    case ItemType::Score:
        key += "score";
        item->SetSpinDuration(0.4f);  // 0.4•bŠÔ ‰ñ“]
        break;
	case ItemType::ScoreBig:
        key += "scoreBig";
        item->SetSpinDuration(0.4f);  // 0.4•bŠÔ ‰ñ“]
		break;
    case ItemType::Power:
        key += "power";
        item->SetSpinDuration(0.4f);  // 0.4•bŠÔ ‰ñ“]
        break;
	case ItemType::PowerBig:
        key += "powerBig";
        item->SetSpinDuration(0.4f);  // 0.4•bŠÔ ‰ñ“]
		break;
    case ItemType::LifePiece:
        key += "lifePiece";
        break;
    case ItemType::LifeFull:
        key += "lifeFull";
        break;
    case ItemType::BombPiece:
        key += "bomPiece";
        break;
    case ItemType::BombFull:
        key += "bomFull";
        break;
    case ItemType::PointExtend:
        break;
    case ItemType::Custom:
        break;
    default:
        break;
    }

    if (auto sprite = Tex[key]) {
        auto sp = itemObj->AddAppBase<SpriteRenderer>();
        sp->SetSprite(sprite);
        sp->SetLayer(1);
        sp->GetSortingLayer().orderInLayer = (int)items.size();
        if(auto arraw = Tex[key + "Arrow"]) {
            auto arrow = itemObj->AddAppBase<ArrowPointer>();
            arrow->SetTarget(item);
			arrow->SetArrowSprite(arraw);
		}
    }  
}