#pragma once

#include "ItemBase.h"

class ItemManager {
private:
	std::vector<std::weak_ptr<ItemBase>> items;
	float AUTO_COLLECT_BORDER_Y = 140.0f;
	ItemManager() {};
public:

	static ItemManager& GetInstance() {
		static ItemManager instance;
		return instance;
	}

    void AddItem(const std::shared_ptr<ItemBase>& item);

	void RemoveItem(const std::shared_ptr<ItemBase>& item);

	void Update();

	void Clear();

	void CreateItem(ItemType itemType,Vector2D pos);
};
