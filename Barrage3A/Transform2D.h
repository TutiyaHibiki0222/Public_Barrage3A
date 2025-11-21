#pragma once
#include "headers.h"
#include "AppBase.h"

namespace GameEngine {
    class GameWorldManager;
}

// Transform2D class and AppBase class
class Transform2D : public AppBase, public std::enable_shared_from_this<Transform2D> {
    friend GameEngine::GameWorldManager;
protected:
    // 親の Transform2D を指すポインタ.
    std::weak_ptr<Transform2D> parent;
    // 子の Transform2D を保持するリスト.
    std::vector<std::shared_ptr<Transform2D>> children;
public:     // 公開 <メンバ変数>.
    // 座標.
    Vector2D position;
    // スケール.
    Vector2D scale;
    // 回転角度.
    float rotation;
public:     // 公開.

    // デフォルトコンストラクタ.
    Transform2D()
        : AppBase("Transform2D"), position(0.0f, 0.0f), scale(1.0f, 1.0f), rotation(0.0) {}

    // ゲームオブジェクトを指定するコンストラクタ.
    Transform2D(std::shared_ptr<GameObject> owner)
        : AppBase("Transform2D", owner), position(0.0f, 0.0f), scale(1.0f, 1.0f), rotation(0.0) {}

    // デストラクタ.
    ~Transform2D() {

        // 親が設定されている場合、この Transform2D を親の子リストから削除.
        if (auto p = parent.lock()) {
            p->RemoveChild(this);
        }
        
        // 子供の Transform2D の親を nullptr に設定し、子リストをクリア.
        for (auto& child : children) {
            child->parent.reset(); // shared_ptr による管理に変更.
        }
        children.clear(); // Clear the children vector
    }

    // 新しい親を設定し、以前の親から削除
    void SetParent(std::shared_ptr<Transform2D> newParent, bool worldPositionStays = true) {
        Vector2D worldPos;

        if (worldPositionStays) {
            worldPos = GetWorldPosition(); // 親を変える前にワールド座標を記録
        }

        // 古い親から自身を削除
        if (auto oldParent = parent.lock()) {
            oldParent->RemoveChild(shared_from_this());
        }

        // 親を設定
        parent = newParent;

        if (newParent) {
            newParent->AddChild(shared_from_this());
        }

        if (worldPositionStays) {
            SetLocalPositionFromWorld(worldPos); // ワールド座標を維持するようローカル座標を補正
        }
    }
    
    // 現在の親を取得
    std::shared_ptr<Transform2D> GetParent() const { return parent.lock(); }

    // 子 Transform2D を追加
    void AddChild(std::shared_ptr<Transform2D> child) {
        if (child) {
            children.push_back(child);
            child->parent = shared_from_this(); // Update parent
        }
    }

    // 子 Transform2D を削除
    void RemoveChild(std::shared_ptr<Transform2D> child) {
        if (child) {
            children.erase(std::remove(children.begin(), children.end(), child), children.end());
            child->parent.reset(); // Remove parent reference
        }
    }
    void RemoveChild(Transform2D* child) {
        if (child) {
            // 子オブジェクトをリストから削除
            children.erase(
                std::remove_if(children.begin(), children.end(),
                    [&child](const std::shared_ptr<Transform2D>& c) { return c.get() == child; }),
                children.end()
            );
            // 親の参照をクリア
            child->parent.reset();
        }
    }
    // ワールド座標を取得
    virtual Vector2D GetWorldPosition() const;

    // ワールドスケールを取得
    Vector2D GetWorldScale() const {
        Vector2D worldScale = scale;
        if (auto p = parent.lock()) {
            Vector2D parentScale = p->GetWorldScale();
            worldScale.x *= parentScale.x;
            worldScale.y *= parentScale.y;
        }
        return worldScale;
    }

    Transform2D GetWorldTransform2D() {
        Transform2D worldTransform = *this;

        // ワールド位置を取得
        worldTransform.position = GetWorldPosition();

        // ワールドスケールを取得
        worldTransform.scale = GetWorldScale();

        // ワールド回転を取得
        worldTransform.rotation = GetWorldRotation();

        return worldTransform;
    }

    // ワールド回転を取得
    float GetWorldRotation() const {
        float worldRotation = rotation;
        if (auto p = parent.lock()) {
            worldRotation += p->GetWorldRotation();
        }
        return worldRotation;
    }

    // Transform2Dの子オブジェクトを取得
    std::shared_ptr<Transform2D> GetChild(size_t index) const {
        if (index < children.size()) {
            return children[index];
        }
        return nullptr; // インデックスが範囲外の場合は nullptr を返す
    }

    size_t GetChildCount() const { return children.size(); }
    const std::vector<std::shared_ptr<Transform2D>>& GetChildren() const { return children; }

    // 名前で子 Transform2D を検索.
    std::shared_ptr<Transform2D> FindName(const std::string& name) const;
    std::shared_ptr<Transform2D> FindByPath(const std::string& name) const;
    // 
    template <typename T, typename = std::enable_if_t<std::is_base_of_v<AppBase, T>>>
    std::vector<std::shared_ptr<T>> GetAppBasesInChildren(bool all = false) const {
        std::vector<std::shared_ptr<T>> appBases;

        for (auto&& app : GetGameObject()->GetAppBases()) {
            if (auto result = std::dynamic_pointer_cast<T>(app)) {
                appBases.push_back(result);
            }
        }


        // 子オブジェクトを再帰的に探索
        for (const auto& child : children) {
            if (!child->IsInHierarchy() && !all) continue;
            // 再帰的に子孫を探索
            auto childComponents = child->GetAppBasesInChildren<T>();
            appBases.insert(appBases.end(), childComponents.begin(), childComponents.end());
        }

        return appBases;
    }

    template <typename T, typename = std::enable_if_t<std::is_base_of_v<AppBase, T>>>
    std::shared_ptr<T> GetAppBaseInChildren() const {
        // 自身のGameObjectのAppBase<T>を取得
        if (auto p = this->GetGameObject()->template GetAppBase<T>()) {
			return p;
        }

        // 子オブジェクトを再帰的に探索
        for (const auto& child : children) {
            auto childComponents = child->GetAppBaseInChildren<T>();
            if (childComponents) {
                return childComponents; // 子オブジェクトで見つかった場合はそれを返す
			}
        }

        return nullptr;
    }

    // デバッグ用のメソッド
    void PrintDebugInfo(const std::string& prefix = "", bool isLast = true) const;

    bool IsInHierarchy() const;

    Vector2D GetLocalPosition() const;
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Transform2D>(*this);
    }


private:
        void SetLocalPositionFromWorld(const Vector2D& worldPosition) {
            if (auto p = parent.lock()) {
                Vector2D parentWorldPos = p->GetWorldPosition();
                position = worldPosition - parentWorldPos;
            }
            else {
                position = worldPosition;
            }
        }
};
