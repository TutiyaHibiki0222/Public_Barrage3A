#pragma once

#include "headers.h"
#include "Transform2D.h"
#include "RectTransform2D.h"
#include "Collider2D.h"
#include "ColliderManager.h"
#include "Layer.h"

using namespace GameEngine;

class AppBase;

// 一つのオブジェクトの main GameObject class;
class GameObject : public std::enable_shared_from_this<GameObject>
{
    friend class GameObjectMgr;
	friend class Transform2D;
    friend class CollisionManager;
    friend class Prefab;
private:
    bool dontDestroyOnLoad = false;
    bool isActive;										// GameObject非表示 (処理を無効).
    std::string tag;									// 取得する際や識別に使用.
    std::string name;									// GameObject名.
    std::vector<std::shared_ptr<AppBase>> newAppBaseList;	// AppBaseの追加リスト.
    std::vector<std::shared_ptr<AppBase>> appBases;	    // AppBaseのクラスリスト.
    bool newAppBase;                                    // 追加AppBaseある.

    Layer layer = Layer::Default; // デフォルト0.
public:
    std::shared_ptr<Transform2D> transform;             // 三種のステータス & 親子関係.
public:
    // コンストラクタ.
    GameObject(std::string _name = "GameObject") : name(_name), tag(""), isActive(true), newAppBase(false)
    {}

    // Unityで言うと AddComponent
    template <typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<AppBase, T>>>
    std::shared_ptr<T> AddAppBase(Args&&... args) {
        static_assert(!std::is_same_v<T, Transform2D>, "Transform2D cannot be added using AddAppBase.");

        if constexpr (std::is_same_v<T, RectTransform2D>) {
            transform = std::make_shared<T>(std::forward<Args>(args)...);
			transform->SetGameObject(shared_from_this());
            for (const auto app : appBases) {
                app->SetTransform2D(transform);
            }
            for (const auto app : newAppBaseList) {
                app->SetTransform2D(transform);
            }
            return std::static_pointer_cast<T>(transform);
        }

        auto app = std::make_shared<T>(std::forward<Args>(args)...);
        app->SetGameObject(shared_from_this());
        app->SetTransform2D(transform);
        newAppBaseList.push_back(app);

        newAppBase = true;
        app->Awake();

        return app;
    }

    // shared_ptr 渡し版
    template <typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<AppBase, T>>>
    std::shared_ptr<T> AddAppBase(std::shared_ptr<T> app) {
        static_assert(!std::is_same_v<T, Transform2D>,
            "Transform2D cannot be added using AddAppBase.");
        if (!app) return nullptr;
        if constexpr (std::is_same_v<T, RectTransform2D>) {
            transform = app;
            transform->SetGameObject(shared_from_this());
            for (const auto app : appBases) {
                app->SetTransform2D(transform);
            }
            for (const auto app : newAppBaseList) {
                app->SetTransform2D(transform);
            }
            return std::static_pointer_cast<T>(transform);
        }
        // 通常 AppBase
        app->SetGameObject(shared_from_this());
        app->SetTransform2D(transform);
        newAppBaseList.push_back(app);

        newAppBase = true;
        app->Awake();
        return app;
    }


    template <typename T, typename = std::enable_if_t<std::is_base_of_v<AppBase, T>>>
    std::shared_ptr<T> GetAppBase() {
        if constexpr (std::is_same_v<T, Transform2D> || std::is_same_v<T, RectTransform2D>) {
            if (auto trns = std::dynamic_pointer_cast<T>(transform)) {
                return trns;
            }
        }

        for (const auto& comp : appBases) {
            std::shared_ptr<T> result = std::dynamic_pointer_cast<T>(comp);
            if (result) return result;
        }

        for (const auto& comp : newAppBaseList) {
            std::shared_ptr<T> result = std::dynamic_pointer_cast<T>(comp);
            if (result) return result;
        }

        return nullptr;
    }
    // 型指定で AppBase を削除
    template <typename T>
    void RemoveAppBase() {
        if constexpr (std::is_same_v<T, RectTransform2D>) {
            if (auto rect = std::dynamic_pointer_cast<RectTransform2D>(transform)) {
                auto newTransform = std::make_shared<Transform2D>(shared_from_this());
                transform = newTransform;
                for (const auto& app : appBases) {
                    app->SetTransform2D(transform);
                }
                for (const auto& app : newAppBaseList) {
                    app->SetTransform2D(transform);
                }
                return;
            }
        }

        // appBases側を削除
        auto it1 = std::remove_if(appBases.begin(), appBases.end(), [](const std::shared_ptr<AppBase>& comp) {
            return std::dynamic_pointer_cast<T>(comp) != nullptr;
            });

        for (auto i = it1; i != appBases.end(); ++i) {
            (*i)->OnDestroy();
        }
        appBases.erase(it1, appBases.end());

        // newAppBaseList 側も削除
        auto it2 = std::remove_if(newAppBaseList.begin(), newAppBaseList.end(), [](const std::shared_ptr<AppBase>& comp) {
            return std::dynamic_pointer_cast<T>(comp) != nullptr;
            });

        for (auto i = it2; i != newAppBaseList.end(); ++i) {
            (*i)->OnDestroy();
        }
        newAppBaseList.erase(it2, newAppBaseList.end());
    }


    // 直接指定で AppBase を削除
    void RemoveAppBase(const std::shared_ptr<AppBase>& target) {
        if (!target) return;

        auto pred = [&target](const std::shared_ptr<AppBase>& comp) {
            return comp == target;
            };

        // appBases側から削除
        auto it1 = std::remove_if(appBases.begin(), appBases.end(), pred);
        for (auto i = it1; i != appBases.end(); ++i) {
            (*i)->OnDestroy();
        }
        appBases.erase(it1, appBases.end());

        // newAppBaseList側からも削除
        auto it2 = std::remove_if(newAppBaseList.begin(), newAppBaseList.end(), pred);
        for (auto i = it2; i != newAppBaseList.end(); ++i) {
            (*i)->OnDestroy();
        }
        newAppBaseList.erase(it2, newAppBaseList.end());
    }

    std::string GetName() const { return name; }
    void SetName(const std::string& _name) { name = _name; }

    std::string GetTag() const { return tag; }
    void SetTag(const std::string& _tag) { tag = _tag; }

    void  SetLayer(Layer _layer) { layer = _layer; }
    Layer GetLayer() const { return layer; }

    bool IsActive() const { return isActive; }
    void SetActive(bool visible) 
    { 
        if (isActive == visible) return;

        isActive = visible; 

        if (isActive) OnEnable();
        else          OnDisable();
    }

    void SetDontDestroyOnLoad(bool flag) { dontDestroyOnLoad = flag; }
    bool IsDontDestroyOnLoad() const { return dontDestroyOnLoad; }

    void Destroy();
protected:
    // 全ての AppBase を取得 <Prefabのみ使用>.
    std::vector<std::shared_ptr<AppBase>> GetAppBases() {
        std::vector<std::shared_ptr<AppBase>> result = appBases;
        result.insert(result.end(), newAppBaseList.begin(), newAppBaseList.end());
        return result;
    }

    // 処理イベント.
    void Awake() {
        for (const auto& appBase : appBases) {
            if (!appBase->GetHasAwakened()) {
                appBase->Awake();
                appBase->SetHasAwakened(true);
            }
        }
    }
	// 追加された AppBase の Start.
    // <前は Start AddAppBase すると ゲームが落ちる問題がありました。>
    void Start()  {
        if (!isActive || newAppBaseList.empty()) return;

        // 一時的に現在のリストを退避して、newAppBaseList を空にしておく
        std::vector<std::shared_ptr<AppBase>> tmp = std::move(newAppBaseList);
        newAppBaseList.clear();
        std::vector<std::shared_ptr<AppBase>> starts;   // 発火リスト.
        for (const auto& appBase : tmp) {
            if (appBase && appBase->enabled && !appBase->GetHasStarted()) {
                appBase->SetHasStarted(true);
                appBases.push_back(appBase);
                starts.push_back(appBase);
            }
            else {
                newAppBaseList.push_back(appBase); // 無効 or すでに Start 済み → 次フレームに回す
            }
        }

        for (const auto& start : starts) {
            start->Start();
        }

        if (newAppBaseList.empty()) {
            newAppBase = false;
        }
    }


    void FixedUpdate()
    {
        if (!isActive) return;
        for (const auto& appBase : appBases) {
            if (appBase->GetHasStarted() && appBase->enabled) appBase->FixedUpdate();
        }
    }

    void Update() {
        if (!isActive) return;
        for (const auto& appBase : appBases) {
            if (appBase->GetHasStarted() && appBase->enabled) appBase->Update();
        }
    }

    void LateUpdate() {
        if (!isActive) return;
        for (const auto& appBase : appBases) {
            if (appBase->GetHasStarted() && appBase->enabled) appBase->LateUpdate();
        }
    }

    void OnDestroy() {
        for (const auto& appBase : GetAppBases()) {
            appBase->OnDestroy();
            appBase->StopAllCoroutine();
        }

        appBases.clear();
        transform.reset();
    }

    void OnCollisionEnter(GameObject* obj) {
        for (const auto& appBase : appBases) {
            if (appBase->GetHasStarted() && appBase->enabled) appBase->OnCollisionEnter(obj);
        }
    }

    void OnCollisionExit(GameObject* obj) {
        for (const auto& appBase : appBases) {
            if (appBase->GetHasStarted() && appBase->enabled) appBase->OnCollisionExit(obj);
        }
    }

    void OnCollisionStay(GameObject* obj) {
        for (const auto& appBase : appBases) {
            if (appBase->GetHasStarted() && appBase->enabled) appBase->OnCollisionStay(obj);
        }
    }

    void OnEnable() {
        for (const auto& appBase : appBases) {
            if (appBase->GetHasStarted() && appBase->enabled) appBase->OnEnable();
        }
    }

    void OnDisable() {
        for (const auto& appBase : appBases) {
            if (appBase->GetHasStarted() && appBase->enabled) appBase->OnDisable();
        }
    }

    void OnApplicationQuit() {
        for (const auto& appBase : appBases) {
             appBase->OnApplicationQuit();
        }
    }

    bool IsNewAppBase() {
        return newAppBase;
    }

public:
    static std::shared_ptr<GameObject> Instantiate(const std::string & = "GameObject", Vector2D = {}, float = 0);
    

};