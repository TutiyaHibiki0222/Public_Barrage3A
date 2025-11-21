#include "GameObject.h"
#include "AppBase.h"
#include "GameObjectMgr.h"

void GameObject::Destroy() {
    Object.DestroyGameObject(shared_from_this());
}

// GameObjectê∂ê¨(ç¿ïW äpìx) or GameObjectManagerÇ…ìoò^.
std::shared_ptr<GameObject> GameObject::Instantiate(const std::string& name, Vector2D pos, float rotation) {
    auto obj = std::make_shared<GameObject>(name);
    obj->transform = std::make_shared<Transform2D>(obj);
    obj->transform->position = pos;
    obj->transform->rotation = rotation;

    GameObjectMgr::GetInstance().AddGameObject(obj);

    return obj;
}