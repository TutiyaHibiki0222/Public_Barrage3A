#pragma once
#include "headers.h"
#include "Layer.h"
#include "GameObjectMgr.h"
#include <string>
#include <typeinfo>

class GameObject;  // 前方宣言.
class AppBase;
class Transform2D;

class Prefab {
    friend class PrefabManager;
private:
    std::string name;    
    std::string tag;
    GameEngine::Layer layer;
    std::vector<std::shared_ptr<Prefab>>  prefabs;
    std::vector<std::shared_ptr<AppBase>> apps;

    Vector2D pos;
    Vector2D size;
    float rot;
public:
    ~Prefab();

    Prefab(std::string _name) : name(_name),tag(""),apps(), pos(), size(1,1), rot(0), layer(GameEngine::Layer::Default){};
    Prefab(std::shared_ptr<GameObject> gameObject);
    
    void SetName(std::string _name)             { name = _name; }
    void SetTag (std::string _tag)              { tag  = _tag;  }
    void SetLayer(GameEngine::Layer _layer)     { layer = _layer; }
    // Unityで言うと AddComponent
    template <typename T, typename... Args, 
    typename = std::enable_if_t<std::is_base_of_v<AppBase, T>>>
    std::shared_ptr<T> AddAppBase(Args&&... args) { 
        // AppBaseの派生クラスTをインスタンス化
        auto set = std::make_shared<T>(std::forward<Args>(args)...); 
        // appBases（コンポーネントのリスト）に追加.
        apps.push_back(set);  
        return set;
    }

    // AddPlafab
    std::shared_ptr<Prefab> AddPrefab(std::shared_ptr<Prefab> prefab) {
        prefabs.push_back(prefab);
        return prefab;
    }
    static std::vector<std::shared_ptr<GameObject>> Clone(std::shared_ptr<GameObject>);
private:
    // 子 Transform2D を再帰的に処理してクローン
    void CloneChildren(std::shared_ptr<Transform2D>);
protected:
    // プレハブから新しいインスタンスを生成するメソッド
    std::vector<std::shared_ptr<GameObject>> Instantiate();
};

class PrefabManager {
private:
    // プレハブを名前で管理する連想配列.
    std::unordered_map<std::string, std::shared_ptr<Prefab>> prefabs;
    PrefabManager(){}
public:
    // シングルトンインスタンスを取得するメソッド.
    static PrefabManager& GetInstance() {
        static PrefabManager instance;  // 初回呼び出し時に一度だけ作成される.
        return instance;
    }

    // プレハブが存在するかを判定するメソッド
    bool HasPrefab(const std::string& name) const {
        return prefabs.find(name) != prefabs.end();
    }

    std::shared_ptr<Prefab> CreatePrefab(const std::string& name, const std::string& objName = "", bool overwrite = false) {
        if (name.empty()) {
            return nullptr;
        }
        auto it = prefabs.find(name);

        if (it != prefabs.end()) {
            if (!overwrite) {
                return nullptr;
            }
            // 上書きする場合は明示的に削除
            prefabs.erase(it);
        }

        std::shared_ptr<Prefab> obj;
        if (objName.empty()) {
            obj = std::make_shared<Prefab>(name);
        }
        else {
            obj = std::make_shared<Prefab>(objName);
        }

        prefabs[name] = obj;
        return obj;
    }

    std::shared_ptr<Prefab> CreatePrefabObj(const std::string& objName) {
        if (objName.empty()) {
            return nullptr;
        }
        return std::make_shared<Prefab>(objName);
    }

    // プレハブを追加するメソッド.
    void AddPrefab(const std::string& name, std::shared_ptr<GameObject> gameObject);

    // GameObject オブジェクト自体を受け取るバージョン
    void AddPrefab(const std::string& name, GameObject& gameObject);

    void AddPrefab(const std::string& name, std::shared_ptr<Prefab> obj);

    // プレハブを取得するメソッド.
    std::shared_ptr<Prefab> GetPrefab(const std::string& name) {
        auto it = prefabs.find(name);
        if (it != prefabs.end()) {
            return it->second;
        }
        return nullptr;  // プレハブが見つからない場合.
    }

    // プレハブを削除するメソッド.
    void RemovePrefab(const std::string& name) {
        prefabs.erase(name);
    }

    void AllRemovePrefab() {
        prefabs.clear();
    }

    // プレハブから新しいオブジェクトを生成するメソッド(自動登録).
    std::vector<std::shared_ptr<GameObject>> Instantiate(const std::string& name) {
        auto prefab = GetPrefab(name);
        auto obj = prefab ? prefab->Instantiate() : std::vector<std::shared_ptr<GameObject>>{};
    
        if (!obj.empty()) {
            GameObjectMgr::GetInstance().AddGameObject(obj);
        }

        return obj;
    }
};
inline PrefabManager& GetPrefabManager() { return PrefabManager::GetInstance(); }
#define PrefabMgr   PrefabManager::GetInstance()