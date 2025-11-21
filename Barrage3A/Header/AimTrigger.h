#pragma once
#include "../AppBase.h"
#include "../GameObject.h"
#include "../headers.h"
#include "../SpriteRenderer.h"
#include "../Debug.hpp"
#include "../GameObjectMgr.h"
#include "Player.h"
#include "ItemBase.h"
using namespace GameEngine;

//class ItemBase;
//
//class AimTrigger : public AppBase {
//public:
//	AimTrigger() : AppBase("AimTrigger") {}
//	AimTrigger(std::shared_ptr<GameObject> _obj) :AppBase("AimTrigger", _obj) {};
//
//	void OnCollisionEnter(GameObject* other) override {
//		if (other->GetName() == "Player") {
//			// êeÇÃ ItemBase ÇéÊìæ
//			var parent = gameObject->transform->GetParent()->GetGameObject();
//			var item = parent->GetAppBase<ItemBase>();
//			if (item) {
//				item->SetAimed(true); // èÛë‘ïœçX
//			}
//		}
//	}
//
//	std::shared_ptr<AppBase> Clone() const override {
//		return std::make_shared<AimTrigger>(*this);
//	}
//};