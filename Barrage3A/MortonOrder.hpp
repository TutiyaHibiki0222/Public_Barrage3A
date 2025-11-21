/*
    MortonOrder.hpp

    ì¬“ú         : 2025/05/09
    ÅI•ÏX“ú     : 2025/05/09
*/
#pragma once
#include <cstdint>


class MortonOrder {
public:

    static uint32_t XorWithTopLeft(uint16_t x, uint16_t y, uint16_t topLeftX = 0, uint16_t topLeftY = 0) {
        uint32_t current = Encode(x, y);
        uint32_t topLeft = Encode(topLeftX, topLeftY);
        return current ^ topLeft;
    }

    static uint32_t XOR(uint32_t a, uint32_t b) {
        return a ^ b;
    }

    static uint32_t Encode(uint16_t x, uint16_t y) {
        return (Part1By1(x) | (Part1By1(y) << 1));
    }

    static uint16_t DecodeX(uint32_t code) {
        return Compact1By1(code);
    }

    static uint16_t DecodeY(uint32_t code) {
        return Compact1By1(code >> 1);
    }

    static uint32_t OffsetByLevel(uint32_t level) {
        return (Pow4(level) - 1) / 3;
    }

    static uint32_t ToLinearIndex(uint32_t level, uint32_t indexInLevel) {
        return OffsetByLevel(level) + indexInLevel;
    }


public:

    static uint32_t Part1By1(uint16_t n) {
        uint32_t x = n;
        x = (x | (x << 8)) & 0x00FF00FF;
        x = (x | (x << 4)) & 0x0F0F0F0F;
        x = (x | (x << 2)) & 0x33333333;
        x = (x | (x << 1)) & 0x55555555;
        return x;
    }

    static uint16_t Compact1By1(uint32_t n) {
        n &= 0x55555555;
        n = (n | (n >> 1)) & 0x33333333;
        n = (n | (n >> 2)) & 0x0F0F0F0F;
        n = (n | (n >> 4)) & 0x00FF00FF;
        n = (n | (n >> 8)) & 0x0000FFFF;
        return static_cast<uint16_t>(n);
    }

    static constexpr uint32_t Pow4(uint32_t n) {
        return 1u << (2 * n); 
    }
};

