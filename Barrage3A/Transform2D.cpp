/*
	Transform2D class
*/
#include "Transform2D.h"
#include "GameObject.h"
#include "GameWorldManager.hpp"
#include <sstream>

// 名前で子 Transform2D を検索
std::shared_ptr<Transform2D> Transform2D::FindName(const std::string& name) const {
    for (auto child : children) {
        if (child->GetGameObject()->GetName() == name) {
            return child;
        }
    }
    return nullptr; // 見つからない場合
}

// Transform2D クラスに追加
std::shared_ptr<Transform2D> Transform2D::FindByPath(const std::string& path) const {
    std::istringstream ss(path);
    std::string token;
    std::shared_ptr<const Transform2D> current = shared_from_this();

    // パスを '/' で分割して順に探索.
    while (std::getline(ss, token, '/')) {
        bool found = false;
        if (!current) return nullptr;

        // 自身の名前と一致したらスルー（初回用）.
        if (token == current->GetGameObject()->GetName()) {
            found = true;
            continue; // 次のトークンへ.
        }

        // 子の中から名前一致を探す.
        for (const auto& child : current->children) {
            if (child->GetGameObject() && child->GetGameObject()->GetName() == token) {
                current = child;
                found = true;
                break;
            }
        }

        if (!found) {
            // 該当パス無し.
            return nullptr;
        }
    }

    // 最終的にたどり着いた Transform2D を返す
    // const→shared_ptrの変換は const_cast か const外しで対処
    return std::const_pointer_cast<Transform2D>(current);
}

Vector2D Transform2D::GetWorldPosition() const{
    if (auto p = parent.lock()) {
        return position + p->GetWorldPosition();
    }
    Vector2D offset = GameEngine::GameWorldManager::GetInstance().WorldOffSet();

    // Y軸反転
    Vector2D pos = position;
    pos.y = -pos.y;

    return offset + pos;
}

Vector2D Transform2D::GetLocalPosition() const {
    if (auto p = parent.lock()) {
        return position + p->GetLocalPosition();
    }
    return position;
}

void Transform2D::PrintDebugInfo(const std::string& prefix, bool isLast) const {
    std::string connector = isLast ? "└─ " : "├─ ";
    std::string childPrefix = isLast ? "   " : "│  ";

    // ゲームオブジェクトの名前とTransform2Dの情報を表示
    std::cout << prefix << connector << "GameObject: " << GetGameObject()->GetName() << std::endl;
    std::cout << prefix << childPrefix << "Transform2D - Position: " << position << " Scale: " << scale << " Rotation: ( " << rotation << " )" << std::endl;
    std::cout << prefix << childPrefix << "World Position:" << GetWorldPosition() << std::endl;

    // 親オブジェクトの情報を表示
    auto parentPtr = parent.lock();
    if (parentPtr) {
        std::cout << prefix << childPrefix << "Parent: GameObject: " << parentPtr->GetGameObject()->GetName()
            << " - Transform2D (Position: " << parentPtr->position << " )" << std::endl;
    }
    else {
        std::cout << prefix << childPrefix << "Parent: None" << std::endl;
    }

    std::cout << prefix << childPrefix << "[ Children Count: " << children.size() << " ]" << std::endl;

    // 子オブジェクトを再帰的に表示
    for (size_t i = 0; i < children.size(); ++i) {
        children[i]->PrintDebugInfo(prefix + (isLast ? "   " : "│  "), i == children.size() - 1);
    }
}

bool Transform2D::IsInHierarchy() const {
    auto go = gameObject.lock();
    if (!go || !go->IsActive()) return false;

    auto parentPtr = parent.lock();
    if (parentPtr) {
        return parentPtr->IsInHierarchy();
    }

    return true;
}