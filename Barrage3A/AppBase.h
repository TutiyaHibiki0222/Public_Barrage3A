#pragma once
#include "headers.h"
#include "WeakAccessor.hpp"

#if (_MSVC_LANG >= 202002L)

#include "Coroutine.hpp"

#endif


// 前方宣言.
class GameObject;
class Transform2D;

// AppBase . . . Unityのコンポーネントと似たもの.
class AppBase {
    friend class Prefab;        // Prefab のみ.
    friend class GameObject;    // GameObject のみ.
private:
    std::string name;       // パーツ名.
    bool hasStarted;        // Start() が呼ばれたかどうかのフラグ.
    bool hasAwakened;       // Awake() が呼ばれたかどうかのフラグ.
protected:
    bool enabled = true;    // モジュールを有効/無効.
    System::WeakAccessor<GameObject>  gameObject;
    System::WeakAccessor<Transform2D> transform;

    // 定義名.
    AppBase(const std::string& _name) : name(_name), gameObject(), transform(), enabled(true), hasStarted(false), hasAwakened(false) { }
    AppBase(const std::string&, std::shared_ptr<GameObject>);
protected:
    // 仮想デストラクタ.
    virtual ~AppBase() = default;
/////////////////////// イベント ///////////////////////
    // 処理の関数.

    // 最優先に初期化します。         0.
    virtual void Awake()        { hasAwakened = true; }
    // オブジェクトが有効時 初期化。  1.
    virtual void Start()        { hasStarted  = true; }
    // 固定フレームごと.              2.
    virtual void FixedUpdate()  { }
    // 1フレームごとに更新.           3.
    virtual void Update()       { }
    // Update後のイベント.            4.
    virtual void LateUpdate()   { }
    // オブジェクトが削除される場合.  Destroy時 実行.
    virtual void OnDestroy()    { }

/////////////////////// 当たり判定イベント. ///////////////////////

    // 初めて当たっている瞬間のイベント.
    virtual void OnCollisionEnter(GameObject* other)    { }
    // 当たっている瞬間のイベント.
    virtual void OnCollisionStay(GameObject* other)     { }
    // 外れた際のイベント.
    virtual void OnCollisionExit(GameObject* other)     { }

    // 有効化イベント
    // オブジェクトが有効化されたとき.
    virtual void OnEnable()     { }
    // オブジェクトが無効化されたとき.
    virtual void OnDisable()    { }
    // 終了時実行.
    virtual void OnApplicationQuit() { }
public:
////////////////////// その他 //////////////////////
    // 名前を取得.
    std::string GetName() const { return name; }
    // GameObjectの削除を行うメソッド.
    void Destroy();
    void Destroy(float time);
    void Destroy(std::weak_ptr<GameObject>);
    void Destroy(std::weak_ptr<GameObject>,float);
    // AppBaseのゲームオブジェクトを取得.
    std::shared_ptr<GameObject> GetGameObject() const;
    // Start() が呼ばれたかどうかを確認するメソッド.
    bool GetHasStarted() const { return hasStarted; }
    // Awake() が呼ばれたかどうかを確認するメソッド.
    bool GetHasAwakened() const { return hasAwakened; }
    void SetEnabled(bool value);
    bool IsEnabled() const { return enabled; }
protected:
    // 名前を設定.
    void SetName(std::string _name) { name = _name; }
    // Start() が呼ばれたフラグを設定するメソッド.
    void SetHasStarted(bool value) { hasStarted = value; }
    // Awake() が呼ばれたフラグを設定するメソッド.
    void SetHasAwakened(bool value) { hasAwakened = value; }

    virtual std::shared_ptr<AppBase> Clone() const = 0;
    void SetGameObject(const std::shared_ptr<GameObject>&);
    void SetTransform2D(std::shared_ptr<Transform2D>);

#if (_MSVC_LANG >= 202002L)
    // オーダー実行形式のStartCoroutine,
    std::shared_ptr<System::Coroutine> StartCoroutine(System::Coroutine coroutine) {
        return System::CoroutineManager::GetInstance().StartCoroutine(std::move(coroutine), this);
    }
    // オーダー実行形式ののみ StopCoroutine,
    void StopAllCoroutine() {
        System::CoroutineManager::GetInstance().StopCoroutinesByOwner(this);
    }
#endif
};
