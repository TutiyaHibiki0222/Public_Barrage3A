#include "QuadTree.h"
#include "Collider2D.h"
#include "ColliderManager.h"

void QuadTree::Clear() {
    objects.clear();

    for (auto& node : nodes) {
        if (node) {
            node->Clear();
            node.reset();
        }
    }
}

void QuadTree::Split() {
    // すでに分割されている場合は何もしない
    if (nodes[0] != nullptr) return;

    // 新しいノードの境界を計算
    float subWidth = bounds.width / 2;
    float subHeight = bounds.height / 2;
    float x = bounds.x;
    float y = bounds.y;

    // 4つの子ノードを作成
    nodes[0] = std::make_unique<QuadTree>(MyRectangle(x + subWidth, y, subWidth, subHeight), maxObjects, maxLevels, level + 1);
    nodes[1] = std::make_unique<QuadTree>(MyRectangle(x, y, subWidth, subHeight), maxObjects, maxLevels, level + 1);
    nodes[2] = std::make_unique<QuadTree>(MyRectangle(x, y + subHeight, subWidth, subHeight), maxObjects, maxLevels, level + 1);
    nodes[3] = std::make_unique<QuadTree>(MyRectangle(x + subWidth, y + subHeight, subWidth, subHeight), maxObjects, maxLevels, level + 1);
}

void QuadTree::Insert(std::shared_ptr<Collider2D> collider) {
    if (!bounds.Intersects(collider->GetBounds())) return; // 領域外

    if (nodes[0]) {
        auto indexes = GetIndexes(collider->GetBounds());
        for (int idx : indexes) {
            nodes[idx]->Insert(collider);
        }
        return;
    }

    objects.push_back(collider);

    if (objects.size() > maxObjects && level < maxLevels) {
        if (!nodes[0]) Split();

        // ここで再配置する
        for (auto it = objects.begin(); it != objects.end();) {
            if (auto col = it->lock()) {
                auto indexes = GetIndexes(col->GetBounds());
                if (!indexes.empty()) {
                    for (int idx : indexes) {
                        nodes[idx]->Insert(col);
                    }
                    it = objects.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }
}

void QuadTree::Retrieve(const MyRectangle& area, std::vector<std::shared_ptr<Collider2D>>& returnObjects) {
    auto indexes = GetIndexes(area);

    for (int idx : indexes) {
        if (nodes[idx]) {
            nodes[idx]->Retrieve(area, returnObjects);
        }
    }

    for (auto& weak : objects) {
        if (auto obj = weak.lock()) {
            returnObjects.push_back(obj);
        }
    }
}

std::vector<int> QuadTree::GetIndexes(const MyRectangle& area) {
    std::vector<int> indexes;
    float verticalMidpoint = bounds.x + (bounds.width / 2);
    float horizontalMidpoint = bounds.y + (bounds.height / 2);

    MyRectangle quadrants[4] = {
        MyRectangle(verticalMidpoint, bounds.y, bounds.width / 2, bounds.height / 2),         // 0: 右上
        MyRectangle(bounds.x, bounds.y, bounds.width / 2, bounds.height / 2),                 // 1: 左上
        MyRectangle(bounds.x, horizontalMidpoint, bounds.width / 2, bounds.height / 2),       // 2: 左下
        MyRectangle(verticalMidpoint, horizontalMidpoint, bounds.width / 2, bounds.height / 2) // 3: 右下
    };

    for (int i = 0; i < 4; ++i) {
        if (quadrants[i].Intersects(area)) {
            indexes.push_back(i);
        }
    }

    return indexes;
}

// Drawメソッドの定義
void QuadTree::Draw() const {
    // ノードの境界を描画
    DrawBoxAA(bounds.x, bounds.y, bounds.x + bounds.width, bounds.y + bounds.height, GetColor(255, 0, 0), FALSE); // 赤色で描画

    // 子ノードが存在する場合は再帰的に描画
    for (const auto& node : nodes) {
        if (node) {
            node->Draw();
        }
    }
}


uint32_t LinearQuadtree::EncodeMorton2D(uint16_t x, uint16_t y) const {
    uint32_t answer = 0;
    for (int i = 0; i < 16; ++i) {
        answer |= ((x & (1 << i)) << i) | ((y & (1 << i)) << (i + 1));
    }
    return answer;
}

std::vector<uint32_t> LinearQuadtree::GetMortonNodesFromAABB(const MyRectangle& aabb, int level, int width, int height) {
    std::vector<uint32_t> mortons;

    int gridSize = 1 << level; // 2^level
    float cellWidth = width / static_cast<float>(gridSize);
    float cellHeight = height / static_cast<float>(gridSize);

    int minX = std::clamp(static_cast<int>(aabb.x / cellWidth), 0, gridSize - 1);
    int minY = std::clamp(static_cast<int>(aabb.y / cellHeight), 0, gridSize - 1);
    int maxX = std::clamp(static_cast<int>((aabb.x + aabb.width) / cellWidth), 0, gridSize - 1);
    int maxY = std::clamp(static_cast<int>((aabb.y + aabb.height) / cellHeight), 0, gridSize - 1);


    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            mortons.push_back(EncodeMorton2D(x, y));
        }
    }
    return mortons;
}




void LinearQuadtree::InsertCollider(const std::shared_ptr<Collider2D>& collider, int level, int width, int height) {
    auto aabb = collider->GetBounds();
    auto mortons = GetMortonNodesFromAABB(aabb, level, width, height);
    for (auto morton : mortons) {
        quadtree[morton].push_back(collider);
    }
}

const std::unordered_map<uint32_t, std::vector<std::shared_ptr<Collider2D>>>& LinearQuadtree::GetQuadtree() const {
    return quadtree;
}