#pragma once

#include <DxLib.h>

class Surface {

public:
    enum eSurface {
        WIND,
        FLOR,
        DOOR,
        LOOF,
        DATANUM,
    };

    Surface(int z, int sHdl, unsigned uType);
    bool Update();
    void Draw(VECTOR offset);

    float GetZ();
    float GetZWid();
    void AddZ(float fZ);

private:
    VERTEX3D    _vertex[4];
    WORD        _index[6];
    int         _handle;
    VECTOR      _angle;

    VERTEX3D _initialVertex[4];  // 初期頂点のコピー
    float _scrollAccum = 0.0f;  // 累積スクロール量
};
