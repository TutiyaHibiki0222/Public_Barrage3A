/*
    LayerMask.hpp

    class
        - LayerMask

    ì¬“ú         : 2025/04/28
    ÅI•ÏX“ú     : 2025/04/28
*/
#pragma once

#include "Layer.h"

namespace GameEngine {

    class LayerMask {
    private:
        uint32_t mask = 0;

    public:
        LayerMask() = default;
        explicit LayerMask(uint32_t _mask) : mask(_mask) {}

        void SetMask(uint32_t _mask) { mask = _mask; }
        uint32_t GetMask() const { return mask; }

        bool Contains(Layer layer) const {
            return (mask & ToMask(layer)) != 0;
        }

        void Add(Layer layer) {
            mask |= ToMask(layer);
        }

        void Remove(Layer layer) {
            mask &= ~ToMask(layer);
        }

        static LayerMask Everything() { return LayerMask(~0u); }
        static LayerMask Nothing() { return LayerMask(0); }
    };

}