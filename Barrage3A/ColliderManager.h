#pragma once
#include "headers.h"
#include "Collider2D.h"
#include "QuadTree.h"
#include "LinerQuaternaryTreeManager.hpp"

#define DEBUG_COLLIDER  (_DEBUG && true)
#include <set>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

struct pair_hash {
    template <typename T1, typename T2>
    std::size_t operator ( )(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // ハッシュ値の組み合わせ
        return h1 ^ (h2 << 1);  // ハッシュの組み合わせ方法を選択
    }
};

enum class CollisionEventType {
    Enter,  // 衝突が開始されたとき
    Stay,   // 衝突している間
    Exit    // 衝突が終了したとき
};

enum class CollisionCheckMode {
    QuadTree,               // 4分木を使用した当たり判定処理.
    Layer_Vs_Layer,         // Layer Vs Layer (通常 QuadTreeだと処理が重い場合軽量に設定ができる).

    Beta_LinearQuadTree     // 正式版ではないが QuatTreeよりは高速版.
};

class CollisionManager {
private:
    // 判定モード.
    CollisionCheckMode mode = CollisionCheckMode::QuadTree;

    using CollisionPair = std::pair<std::shared_ptr<GameObject>, std::shared_ptr<GameObject>>;

    // 衝突ペア履歴（Enter/Exit管理用）
    std::set<CollisionPair> previousCollisions;
    std::vector<std::shared_ptr<Collider2D>> colliders; // 登録されたコライダーのリスト

    // QuadTrue Auto;
    bool useQuadTree = true;
    bool isQuadTrueSizeAuto;
    MyRectangle myRectangleSize{ 0,0,WIDTH, HEIGHT };
    int maxObjects = 4;
    int maxLevels  = 5;
    // コンストラクタをプライベートにしてインスタンスの生成を制限
    CollisionManager() : isQuadTrueSizeAuto(true){}

public:
    // コピーコンストラクタと代入演算子を削除して、インスタンスの複製を防ぐ
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;

    // シングルトンのインスタンスを取得
    static CollisionManager& GetInstance() {
        static CollisionManager instance;
        return instance;
    }

    void AddCollider(const std::shared_ptr<Collider2D>& collider) {
        colliders.push_back(collider);
    }

    void RemoveCollider(const std::shared_ptr<Collider2D>& collider) {
        colliders.erase(std::remove(colliders.begin(), colliders.end(), collider), colliders.end());
    }

    void Reset() {
        previousCollisions.clear();
        colliders.clear();
    }

    void RemoveCollisionObject(std::shared_ptr<GameObject>);

    void CheckCollisions();

    void SetIsQuadTrueSizeAuto(bool is) {
        isQuadTrueSizeAuto = is;
    }

    void SetColliderCheckMode(CollisionCheckMode newMode) {
        if (newMode == mode) return;
        mode = newMode;
    }

    void SetMyRectangleSize(MyRectangle size) {
        myRectangleSize = size;
    }

    void SetQuadTrueMaxLevel(int level) {
        maxLevels = level;
    }

    void SetQuadTrueMaxObject(int size) {
        maxObjects = size;
    }

    void SetQuadTreeSetting(bool is) {
        useQuadTree = is;
    }
private:
    MyRectangle CalculateWorldBounds();
    // QuadTreeを使用した当たり判定.
    void CheckCollisionsQuadTreeMode(std::vector<std::pair<CollisionPair, CollisionEventType>>&);

    void CheckCollisionsLayerVsLayerMode(std::vector<std::pair<CollisionPair, CollisionEventType>>&);

    uint64_t MakeCollisionKey(uintptr_t a, uintptr_t b) {
        return (static_cast<uint64_t>(std::min(a, b)) << 32) | static_cast<uint64_t>(std::max(a, b));
    }

    CollisionPair MakeOrderedPair(const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b) {
        return (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
    }
};