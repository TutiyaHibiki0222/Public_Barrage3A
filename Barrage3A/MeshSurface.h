#pragma once
#include <vector>
#include <DxLib.h>
#include <cstdint>

using u16 = uint16_t;

class MeshSurface {
public:
    MeshSurface();

    void SetVertices(const std::vector<VERTEX3D>& verts, const std::vector<u16>& indices);
    void SetTexture(int texHandle);

    void SetPosition(const VECTOR& pos);
    void SetRotation(const VECTOR& rot);
    void SetScale(const VECTOR& scale);

    void SetVisible(bool visible);

    bool Update();

    void Draw();
    void DrawWithOffset(const VECTOR& offset);

private:
    void UpdateTransform();

    std::vector<VERTEX3D> _vertices;
    std::vector<u16> _indices;

    VECTOR _position;
    VECTOR _rotation;
    VECTOR _scale;

    MATRIX _localMatrix;

    int _textureHandle;
    bool _visible;
};

