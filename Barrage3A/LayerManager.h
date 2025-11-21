/*
    LayerManager.h

    class
        - LayerManager 

    作成日         : 2025/05/03
    最終変更日     : 2025/05/03
*/
#pragma once

#include "Layer.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace GameEngine {

    class LayerManager {
    private:
        std::unordered_map<std::string, Layer> nameToLayer;
        std::unordered_map<Layer, std::string> layerToName;
        // コライダー マスク.
        uint32_t collisionMatrix[static_cast<int>(Layer::Count)] = {};

        LayerManager() {
            for (int i = 0; i < static_cast<int>(Layer::Count); ++i) {
                collisionMatrix[i] = UINT32_MAX;
            }
        };
        ~LayerManager() = default;
    public:
        static LayerManager& GetInstance() {
            static LayerManager instance;
            return instance;
        }

        void AllCollisioneMatrix(bool canCollide) {
            uint32_t mask = canCollide ? UINT32_MAX : 0;
            for (int i = 0; i < static_cast<int>(Layer::Count); ++i) {
                collisionMatrix[i] = mask;
            }
		}

        bool LoadLayers(const std::string& path);
        bool LoadCollisionMatrix(const std::string& filepath);

        bool CanCollide(Layer a, Layer b) const {
            return (collisionMatrix[static_cast<int>(a)] & ToMask(b)) != 0;
        }

        void SetCollisionMask(Layer layer, uint32_t mask) {
            collisionMatrix[static_cast<int>(layer)] = mask;
        }

        void SetCollisionMask(Layer layer, Layer target, bool canCollide) {
            uint32_t mask = GetCollisionMask(layer);
            if (canCollide) {
                mask |= ToMask(target);  // 衝突許可
            }
            else {
                mask &= ~ToMask(target); // 衝突拒否
            }
            SetCollisionMask(layer, mask);
        }

        void SetCollisionMaskBothWays(Layer layer, Layer target, bool canCollide) {
            SetCollisionMask(layer, target ,canCollide);
            SetCollisionMask(target, layer, canCollide);
        }

        void AllowCollision(Layer from, Layer to) {
            collisionMatrix[static_cast<int>(from)] |= ToMask(to);
        }

        void DenyCollision(Layer from, Layer to) {
            collisionMatrix[static_cast<int>(from)] &= ~ToMask(to);
        }

        void AddCollision(Layer layer, Layer target) {
            collisionMatrix[static_cast<int>(layer)] |= ToMask(target);
        }

        void RemoveCollision(Layer layer, Layer target) {
            collisionMatrix[static_cast<int>(layer)] &= ~ToMask(target);
        }

        uint32_t GetCollisionMask(Layer layer) const {
            return collisionMatrix[static_cast<int>(layer)];
        }


        Layer GetLayerByName(const std::string& name) const;
        const std::string& GetLayerName(Layer layer) const;
    };

}