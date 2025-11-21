/*
    Layer.h

    enum class 
        - Layer -> <cstdint>::uint8_t

    作成日         : 2025/05/03
    最終変更日     : 2025/05/03
*/
#pragma once


#include <string>
#include <unordered_map>
#include <cstdint>

namespace GameEngine 
{

    enum class Layer : uint8_t {
        // Default.
        Default = 0,
        // その他追加するルート.
        Player,
        PlayerBullet,
        Graze,
        Enemy,
        EnemyBullet,
        Item,
        // size.
        Count
    };

    inline uint32_t ToMask(Layer layer) {
        return 1u << static_cast<uint8_t>(layer);
    }

    inline Layer ToLayer(uint8_t id) {
        return static_cast<Layer>(id);
    }

}