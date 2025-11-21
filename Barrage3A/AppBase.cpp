#include "AppBase.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "Transform2D.h"
#include "Invoke.hpp"

AppBase::AppBase(const std::string& _name, std::shared_ptr<GameObject> _obj) : name(_name), gameObject(_obj),enabled(true),hasStarted(false), hasAwakened(false) { transform = _obj->transform; }

void AppBase::Destroy() {
    gameObject->Destroy();
}

void AppBase::Destroy(float time) {
    std::weak_ptr<GameObject> self = gameObject.lock();
    System::Invoke([self]() {
        if (auto obj = self.lock()) {
            Object.DestroyGameObject(obj);
        }
        }, time);
}

void AppBase::Destroy(std::weak_ptr<GameObject> obj) {
    if (obj.lock()) {
        Object.DestroyGameObject(obj.lock());
    }
}

void AppBase::Destroy(std::weak_ptr<GameObject> obj, float time) {
    System::Invoke([obj]() {
        if (auto shared = obj.lock()) {
            Object.DestroyGameObject(shared);
        }
        }, time);
}

std::shared_ptr<GameObject> AppBase::GetGameObject() const {
    return gameObject;
}

void AppBase::SetGameObject(const std::shared_ptr<GameObject>& obj)
{
    gameObject = obj;
}

void AppBase::SetTransform2D(std::shared_ptr<Transform2D> newTrans) {
    transform = newTrans;
}

void AppBase::SetEnabled(bool value) {
    if (enabled == value) return;

    enabled = value;

    if (enabled) OnEnable();
    else OnDisable();

}