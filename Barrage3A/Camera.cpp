/*
	Camera class
*/
#include "Camera.h"
#include "GameObject.h"
#include "GameWorldManager.hpp"
// コンストラクタ 
Camera::Camera() : AppBase("Camera"){

}
Camera::Camera(std::shared_ptr<GameObject> owner) : AppBase("Camera", owner) {
    if (owner != nullptr && owner->transform != nullptr) {
        GameEngine::GameWorldManager::GetInstance().SetCameraPosition(owner->transform);
    }
}

void Camera::OnDestroy(){
    GameEngine::GameWorldManager::GetInstance().SetCameraPosition(nullptr);
}
