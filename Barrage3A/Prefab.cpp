#include "Prefab.h"
#include "GameObject.h"
#include "AppBase.h"

///////// Prefab ///////////
Prefab::~Prefab()
{
    prefabs.clear();
}

Prefab::Prefab(std::shared_ptr<GameObject> gameObject) {
    name = gameObject->GetName();
    tag  = gameObject->GetTag();
    // 子TransformのGameObjectもPrefabに追加
    CloneChildren(gameObject->transform);

    pos     = gameObject->transform->position;
    size    = gameObject->transform->scale;
    rot     = gameObject->transform->rotation;

    for (const auto& app : gameObject->GetAppBases()) {
        auto cl = app->Clone();
        if (cl) {
            apps.push_back(cl);
            cl->SetHasAwakened(false);
            cl->SetHasStarted(false);
        }
    }
}

void Prefab::CloneChildren(std::shared_ptr<Transform2D> transform) {
    for (const auto& childTransform : transform->GetChildren()) {
        if (auto childGameObject = std::make_shared<GameObject>(*childTransform->GetGameObject())) {
            auto prefab = std::make_shared<Prefab>(childGameObject);
            prefabs.push_back(prefab);
            CloneChildren(childTransform); // 子 の要素に追加あるか.
        }
    }
}
 // プレハブから新しいインスタンスを生成するメソッド
std::vector<std::shared_ptr<GameObject>> Prefab::Instantiate() {
    std::vector<std::shared_ptr<GameObject>> objects;
    // 新しい GameObject を作成
    auto newObj = std::make_shared<GameObject>();
    newObj->transform = std::make_shared<Transform2D>(newObj);
    // 座標類.
    newObj->transform->position     = pos;
    newObj->transform->scale        = size;
    newObj->transform->rotation     = rot;

    // プレハブの GameObject を元に新しいオブジェクトを構成
    newObj->SetName(name);
    newObj->SetTag(tag);
    newObj->SetLayer(layer);
    // コンポーネントを再度追加.
    for (const auto& app : apps) {
        auto newApp = app->Clone();
        newObj->AddAppBase(newApp);
    }
    objects.push_back(newObj);
    // 子たち.
    for (const auto& prefab : prefabs) {
        auto childObjects = prefab->Instantiate();
        for (auto& childObj : childObjects) {
            if (newObj->transform && childObj->transform) {
                newObj->transform->AddChild(childObj->transform);
            }
            objects.push_back(childObj);
        }
    }

    return objects;
}

std::vector<std::shared_ptr<GameObject>> Prefab::Clone(std::shared_ptr<GameObject> target) {
    Prefab my(target);
    return my.Instantiate();
}

///////// Mgr //////////////
 // プレハブを追加するメソッド.
void PrefabManager::AddPrefab(const std::string& name, std::shared_ptr<GameObject> gameObject) {
    if (!gameObject) return;
    // 名前が重複していないことを確認.
    if (prefabs.find(name) != prefabs.end()) {
        return;
    }
    // プレハブとして登録.
    prefabs[name] = std::make_shared<Prefab>(gameObject);
}

// GameObject オブジェクト自体を受け取るバージョン
void PrefabManager::AddPrefab(const std::string& name, GameObject& gameObject) {
    // 名前が重複していないことを確認.
    if (prefabs.find(name) != prefabs.end()) {
        return;  // 重複する場合は上書きしない.
    }

    // オブジェクトを std::shared_ptr に変換してから登録.
    prefabs[name] = std::make_shared<Prefab>(std::make_shared<GameObject>(gameObject));
}

void PrefabManager::AddPrefab(const std::string& name, std::shared_ptr<Prefab> obj) {
    if (!obj) return;
    // 名前が重複していないことを確認.
    if (prefabs.find(name) != prefabs.end()) {
        return;
    }
    prefabs[name] = obj;
}