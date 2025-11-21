#pragma once
#include "headers.h"

class Collider2D;

struct QuadTruePoint {
    float x, y;
    void* userData;
    QuadTruePoint(float _x, float _y, void* data = nullptr) : x(_x), y(_y), userData(data) {}
};

class MyRectangle {
public:
    float x;      // 矩形の左上のX座標
    float y;      // 矩形の左上のY座標
    float width;  // 矩形の幅
    float height; // 矩形の高さ

    MyRectangle(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height) {}

    // 矩形が他の矩形と交差しているかをチェックするメソッド
    bool Intersects(const MyRectangle& other) const {
        return (x < other.x + other.width && x + width > other.x &&
                y < other.y + other.height && y + height > other.y);
    }

    // 矩形が他の矩形を完全に含んでいるかをチェックするメソッド
    bool Contains(const MyRectangle& other) const {
        return (x <= other.x && x + width >= other.x + other.width &&
                y <= other.y && y + height >= other.y + other.height);
    }

    float GetOverlap(const MyRectangle& other, const Vector2D& normal) const {
        float overlapX = Mathf::Max(0.0f, Mathf::Min(this->width, other.width) - Mathf::Max(this->x, other.x));
        float overlapY = Mathf::Max(0.0f, Mathf::Min(this->height, other.height) - Mathf::Max(this->y, other.y));


        // 衝突の法線方向に応じて、適切な重なり量を返す
        if (std::abs(normal.x) > std::abs(normal.y)) {
            return overlapX;  // X軸方向の重なり量
        }
        else {
            return overlapY;  // Y軸方向の重なり量
        }
    }
};
/// <summary>
/// 当たり判定の処理を分解.
/// </summary>
class QuadTree {
private:
    MyRectangle bounds; // AABB;
    std::vector<std::weak_ptr<Collider2D>> objects;
    std::array<std::unique_ptr<QuadTree>, 4> nodes;
    int maxObjects;
    int maxLevels;
    int level;
public:
    QuadTree(MyRectangle bounds, int maxObjects, int maxLevels, int level = 0)
        : bounds(bounds), maxObjects(maxObjects), maxLevels(maxLevels), level(level) {}

    void Clear();
    void Split();
    void Insert(std::shared_ptr<Collider2D> collider);
    void Retrieve(const MyRectangle& area, std::vector<std::shared_ptr<Collider2D>>& returnObjects);
    void Draw() const; // 描画メソッドの宣言.

    // すべてのリーフノードを取得するメソッド
    std::vector<QuadTree*> GetAllLeafNodes() {
        std::vector<QuadTree*> leafNodes;

        // 子ノードが存在するなら再帰的に探索
        if (nodes[0]) { // 子ノードがある場合
            for (auto& node : nodes) {
                auto childLeaves = node->GetAllLeafNodes();
                leafNodes.insert(leafNodes.end(), childLeaves.begin(), childLeaves.end());
            }
        }
        else { // 子ノードがない（リーフノード）
            leafNodes.push_back(this);
        }

        return leafNodes;
    }

    // GetObjectsメソッド: ノード内のオブジェクトを返す
    const std::vector<std::weak_ptr<Collider2D>>& GetObjects() const {
        return objects;
    }
private:
    std::vector<int> GetIndexes(const MyRectangle& area);
};


class LinearQuadtree {
private:
    std::unordered_map<uint32_t, std::vector<std::shared_ptr<Collider2D>>> quadtree;
private:
    uint32_t EncodeMorton2D(uint16_t x, uint16_t y) const;

    std::vector<uint32_t> GetMortonNodesFromAABB(const MyRectangle& aabb, int level, int width, int height);

public:
  

    void InsertCollider(const std::shared_ptr<Collider2D>& collider, int level, int width, int height);

    const std::unordered_map<uint32_t, std::vector<std::shared_ptr<Collider2D>>>& GetQuadtree() const;
};

