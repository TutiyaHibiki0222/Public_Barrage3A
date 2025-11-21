#pragma once

#include <vector>
#include <list>
#include <memory>
#include <algorithm>
#include "CCell.hpp"
#include "TreeData.hpp"
#include "MortonOrder.hpp"
#include "CollisionList.hpp"
#define CLINER4TREEMANAGER_MAXLEVEL		(9)

template<typename Type>
class LinearQuadTreeSpace {
private:
    int width;
    int height;
    int currentLevel;

    std::vector<std::vector<Type>> data;
public:
    LinearQuadTreeSpace(int width, int height, int maxLevel)
        : width(width)
        , height(height)
        , currentLevel(0)
    {
        data.resize(1);
        while (currentLevel < maxLevel) {
            Expand();
        }
    }

    void Clear() {
        for (auto& cell : data) {
            cell.clear();
        }
    }

    template<typename T>
    void AddObject(T actor) {
        // グローバル座標上のバウンディングボックスを取得
        const auto collider = actor->GetBounds();
        int left    = (int) collider.x;
        int top     = (int) collider.y;
        int right   = (int)(collider.x + collider.width);
        int bottom  = (int)(collider.y + collider.height);

        int leftTopMorton       = Calc2DMortonNumber(left , top);
        int rightBottomMorton   = Calc2DMortonNumber(right, bottom);

        if (leftTopMorton == -1 && rightBottomMorton == -1) {
            AddNode(actor, 0, 0);
            return;
        }

        if (leftTopMorton == rightBottomMorton) {
            AddNode(actor, currentLevel, leftTopMorton);
            return;
        }

        int level = CalcLevel(leftTopMorton, rightBottomMorton);
        int largerMorton = Mathf::Max(leftTopMorton, rightBottomMorton);
        int cellNumber = CalcCell(largerMorton, level);
        AddNode(actor, level, cellNumber);
    }

    bool HasCell(int index) const {
        return (index >= 0 && index < (int)data.size() && !data[index].empty());
    }

    // ActorPtr は常に「Actor* 型」で使う想定です
    const std::vector<Type>& GetCell(int index) const {
        return data[index];
    }

    int TotalCells() const {
        return (int)data.size();
    }

private:

    void Expand() {
        int nextLevel = currentLevel + 1;
        int totalNodes = ((int)std::pow(4, nextLevel + 1) - 1) / 3;
        if ((int)data.size() < totalNodes) {
            data.resize(totalNodes);
        }
        currentLevel = nextLevel;
    }
    template<typename T>
    void AddNode(T actor, int level, int indexInLevel) {
        int linearIndex = MortonOrder::OffsetByLevel(level) + indexInLevel;
        if (linearIndex >= (int)data.size()) {
            data.resize(linearIndex + 1);
        }
        // 親ノードは空 vector を保持しているだけで OK
        int parent = linearIndex;
        while (parent > 0) {
            parent = (parent - 1) / 4;
        }
        data[linearIndex].push_back(actor);
    }

    int Calc2DMortonNumber(int x, int y) const {
        if (x < 0 || y < 0 || x > width || y > height) {
            return -1;
        }
        int cellsPerRow = 1 << currentLevel;
        int cellWidth   = width / cellsPerRow + 1;
        int cellHeight  = height / cellsPerRow + 1;

        int xCell = Mathf::Min(x / cellWidth, cellsPerRow - 1);
        int yCell = Mathf::Min(y / cellHeight, cellsPerRow - 1);

        int sx = SeparateBit32(xCell);
        int sy = SeparateBit32(yCell);
        return (sx | (sy << 1));
    }

    int CalcLevel(int morton1, int morton2) const {
        int xorM = morton1 ^ morton2;
        int attachedLevel = currentLevel;
        int level = currentLevel - 1;
        for (int i = 0; level >= 0; ++i, --level) {
            int flag = (xorM >> (i * 2)) & 0x3;
            if (flag > 0) {
                attachedLevel = level;
            }
        }
        return attachedLevel;
    }

    int CalcCell(int morton, int level) const {
        int shift = (currentLevel - level) * 2;
        return morton >> shift;
    }

    int SeparateBit32(int n) const {
        uint32_t x = (uint32_t)n;
        x = (x | (x << 8)) & 0x00FF00FFu;
        x = (x | (x << 4)) & 0x0F0F0F0Fu;
        x = (x | (x << 2)) & 0x33333333u;
        x = (x | (x << 1)) & 0x55555555u;
        return (int)x;
    }
};