#include "GameObjectMgr.h"
#include "GameObject.h"

/// <summary>
/// 新規に作成する場合.
/// </summary>
/// <param name="_name"> 名前 </param>
/// <returns>作成完了したGameObject</returns>
std::shared_ptr<GameObject> GameObjectMgr::Instantiate(const std::string _name) {
    // オブジェクトを作成.
    std::shared_ptr<GameObject> obj = std::make_shared<GameObject>(_name);
    obj->transform = std::make_shared<Transform2D>(obj);
    // オブジェクトを追加.
    AddGameObject(obj);
    // 作成したオブジェクトを返す.
    return obj;
}

/// <summary>
/// 新規に作成する場合.
/// </summary>
/// <param name="_name"> 名前 </param>
/// <param name="_position"> 初期生成 座標値 </param>
/// <returns></returns>
std::shared_ptr<GameObject> GameObjectMgr::Instantiate(const std::string _name, Vector2D _position)
{
    // オブジェクトを作成.
    std::shared_ptr<GameObject> obj = std::make_shared<GameObject>(_name);
    obj->transform = std::make_shared<Transform2D>(obj);
    // オブジェクトを追加.
    AddGameObject(obj);
    // 座標をずらす.
    if (obj->transform)obj->transform->position = _position;
    // 作成したオブジェクトを返す.
    return obj;
}
/// <summary>
/// あらかじめ 作成したGameObjectを再生成.
/// </summary>
/// <param name="_obj"></param>
/// <returns>作成完了したGameObject</returns>
std::shared_ptr<GameObject> GameObjectMgr::Instantiate(std::shared_ptr<GameObject> _obj) {
    // もしも nullptr だったら nullptrを返す.
    if (!_obj) return nullptr;
    // オブジェクトを増やす.
    AddGameObject(_obj);
    // 増やしたオブジェクトを返す.
    return _obj;
}

std::shared_ptr<GameObject> GameObjectMgr::Instantiate(const std::vector<std::shared_ptr<GameObject>> _objs) {
    
    if (_objs.empty()) return nullptr;

    AddGameObject(_objs);

    return _objs[0];
}