#pragma once
#include "headers.h"
#include "GameObject.h"
#include "WeakAccessor.hpp"
#include "Stopwatch.hpp"
/// <summary>
/// 出現しているオブジェクトを管理.
/// </summary>
class GameObjectMgr {
private:
    std::vector<std::shared_ptr<GameObject>> newGameObjects;
    // ゲームオブジェクトの配列データ.
    std::unordered_map<uintptr_t, std::shared_ptr<GameObject>> gameObjects;
    // 削除待ちのオブジェクト.
    std::vector<System::WeakAccessor<GameObject>> objectsToDestroy;
    // 一つのオブジェクトにする.
    GameObjectMgr(){}
public:

    std::shared_ptr<GameObject> FindByRawPtr(uintptr_t ptr) {
        auto it = gameObjects.find(ptr);
        return (it != gameObjects.end()) ? it->second : nullptr;
    }

    // GameObjectMgrを取得.
    static GameObjectMgr& GetInstance() {
        static GameObjectMgr instance;
        return instance;
    }
    // オブジェクトを追加.
    void AddGameObject(std::shared_ptr<GameObject> gameObject) {
        newGameObjects.push_back(gameObject);
    }
    // まとめて登録.
    void AddGameObject(std::vector<std::shared_ptr<GameObject>> _gameObjects){
        for (const auto& obj : _gameObjects) {
            if (obj) {  // nullptr チェック
                newGameObjects.push_back(obj);  // gameObjects に追加
            }
        }
    }
    // オブジェクト削除.
    void DestroyGameObject(std::shared_ptr<GameObject> gameObject) {
        if (gameObject) {
            // すでに削除待ちリストに存在しない場合のみ追加.
            if (std::find_if(objectsToDestroy.begin(), objectsToDestroy.end(),
                [&gameObject](const System::WeakAccessor<GameObject>& wptr) {
                    return wptr == gameObject; // operator== を使うことで比較
                }) == objectsToDestroy.end())
            {
                objectsToDestroy.push_back(gameObject);
            }
        }
    }

    void DestroySceneObjects() {
        for (const auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if (!obj->IsDontDestroyOnLoad()) {
                DestroyGameObject(obj); // 安全な経路
            }
        }
    }

    // ゲーム内の等しい"名前"のGameObjectを一つ取得.
    std::shared_ptr<GameObject> FindWithName(const std::string& _name) const {
        for (const auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if (obj->GetName() == _name) return obj;
        }
        return nullptr;
    }

    // ゲーム内の等しい"タグ"のGameObjectを一つ取得.
    std::shared_ptr<GameObject> FindWithTag(const std::string& _tag) const {
        for (const auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if (obj->GetTag() == _tag) return obj;
        }
        return nullptr;
    }
    // ゲーム内の等しい class を持つオブジェクトを取得.
    template <typename T>
    std::shared_ptr<T> FindObjectOfType() const {
        for (auto& pair : gameObjects) {
            const auto& obj = pair.second;
            auto tmp = obj->GetAppBase<T>();
            if (tmp) return tmp;
        }
        return nullptr;
    }
    // ゲーム内の等しい"名前"のGameObjectを全て取得.
    std::vector<std::shared_ptr<GameObject>> FindGameObjectsWithName(const std::string& _name) const {
        std::vector<std::shared_ptr<GameObject>> result;
        for (const auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if (obj->GetName() == _name) {
                result.push_back(obj);
            }
        }
        return result;
    }
    // ゲーム内の等しい"タグ"のGameObjectを全て取得.
    std::vector<std::shared_ptr<GameObject>> FindGameObjectsWithTag(const std::string& _tag) const {
        std::vector<std::shared_ptr<GameObject>> result;
        for (const auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if (obj->GetTag() == _tag) {
                result.push_back(obj);
            }
        }
        return result;
    }
    // ゲーム内の等しい class を持つオブジェクトを全て取得.
    template <typename T>
    std::vector<std::shared_ptr<T>> FindObjectsOfType() const {
        std::vector<std::shared_ptr<T>> tmps;
        for (auto& pair : gameObjects) {
            const auto& obj = pair.second;
            auto tmp = obj->GetAppBase<T>();
            if (tmp) tmps.push_back(tmp);
        }
        return tmps;
    }

    // 全てのゲームのオブジェクトの"Awake"を実行.
    void AllGameObjectAwake() {
        for (auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if(obj->IsNewAppBase()) obj->Awake();
        }
    }
    // 全てのゲームのオブジェクトの"Start"を実行.
    void AllGameObjectStart() {
        for (auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if (!obj->IsActive() || !obj->IsNewAppBase()) continue;
            obj->Start();
        }
    }

    void AllGameObjectFixedUpdate() {
        for (auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if (obj->IsActive()) {
                obj->FixedUpdate();
            }
        }
    }

    // 全てのゲームのオブジェクトの"Update"を実行.
    void AllGameObjectUpdate() {
        for (auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if (obj->IsActive()) {
                obj->Update();
            }
        }
    }

    // 全てのゲームのオブジェクトの"Update"を実行.
    void AllGameObjectLateUpdate() {
        for (auto& pair : gameObjects) {
            const auto& obj = pair.second;
            if (obj->IsActive()) {
                obj->LateUpdate();
            }
        }
    }

    void AllOnApplicationQuit() {
        for (auto& pair : gameObjects) {
            const auto& obj = pair.second;
            obj->OnApplicationQuit();
        }
    }

    void AllDestroyGameObject() {
        for (auto pair : gameObjects) {
            const auto& obj = pair.second;
            objectsToDestroy.push_back(obj);
        }
    }
    // 全てのゲームオブジェクト処理を実行.
    void AllGameObjectSystem() {
        // 一般的な処理.
        AllGameObjectAwake();
        AllGameObjectStart();
        AllGameObjectUpdate();
        CollisionManager::GetInstance().CheckCollisions();
        // 不要なオブジェクトを削除.
        ProcessDestroyQueue();
    }

    void ProcessNewObjects() {
        for (auto& obj : newGameObjects) {
            uintptr_t rawPtr = reinterpret_cast<uintptr_t>(obj.get());
            gameObjects[rawPtr] = obj;
        }
        newGameObjects.clear();
    }

    // 削除待ちリストのオブジェクトを削除.
    void ProcessDestroyQueue() {
        if (objectsToDestroy.empty()) return;
    //  DebugStopwatch::Start("DestroyQueue");
        for (auto& weakObj : objectsToDestroy) {
            if (auto sp = weakObj.lock()) {
                auto key = reinterpret_cast<uintptr_t>(sp.get());
                auto it = gameObjects.find(key);
                if (it != gameObjects.end()) {
                    auto gameObject = it->second;
                    gameObject->OnDestroy();
                    gameObjects.erase(it);
                }
            }
        }
        objectsToDestroy.clear();
    //  DebugStopwatch::PrintStop("DestroyQueue", "削除処理時間 ");
    }


    // 現在出ているゲームオブジェクト数.
    int GetGameObjectCount() const {
        return (int)gameObjects.size();
    }

    // GameObject Mgr に登録されているすべてのオブジェクト.
    std::vector<std::shared_ptr<GameObject>> GetGameObjects() {
        std::vector<std::shared_ptr<GameObject>> result;
        result.reserve(gameObjects.size());
        for (const auto& pair : gameObjects) {
            result.push_back(pair.second);
        }
        return result;
    }

    void DontDestroyOnLoad(std::shared_ptr<GameObject> gameObject) {
        if (gameObject) {
            gameObject->SetDontDestroyOnLoad(true);
        }
    }

    std::shared_ptr<GameObject> Instantiate(const std::string _name = "GameObject");
    std::shared_ptr<GameObject> Instantiate(const std::string _name,Vector2D _position);
    std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<GameObject> _obj);
    std::shared_ptr<GameObject> Instantiate(const std::vector<std::shared_ptr<GameObject>> _objs);
private:
  

};
#define Object      GameObjectMgr::GetInstance()