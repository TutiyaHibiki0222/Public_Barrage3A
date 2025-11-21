#pragma once
#include <vector>
#include <DxLib.h>
#include "IDraw.h"

class StageUnit {

private:
    std::vector<VERTEX3D> _vertices;        // 元頂点
    std::vector<WORD> _indices;              // インデックス

    int _textureHandle = -1;

    VECTOR _position = VGet(0, 0, 0);
    VECTOR _rotation = VGet(0, 0, 0);          // ラジアン単位（X,Y,Z）
    VECTOR _scale = VGet(1, 1, 1);

    VECTOR _velocity = VGet(0, 0, 0);          // 1秒あたりの位置変化
    VECTOR _angularVelocity = VGet(0, 0, 0);   // 1秒あたりの角度変化（ラジアン）
    VECTOR vMove = VGet(0, 5, -50);

    float _drawRotateX = 0.0f;                  // 描画時X軸回転角度（ラジアン）
    float _drawRotateY = 0.0f;                  // 描画時X軸回転角度（ラジアン）
    float _drawRotateZ = 0.0f;                  // 描画時X軸回転角度（ラジアン）

    Color color = Color(255,255,255,255);

    mutable std::vector<VERTEX3D> _transformedVertices;  // 描画用変換済み頂点
    mutable bool _dirty = true;   // 変換済み頂点が古いか
public:
    StageUnit();
    ~StageUnit();

    void SetPosition(const VECTOR& pos);
    void SetRotation(const VECTOR& rot);
    void SetScale(const VECTOR& scale);

    VECTOR GetPosition() const { return _position; }
    VECTOR GetRotation() const { return _rotation; }
    VECTOR GetScale() const { return _scale; }

    void SetVelocity(const VECTOR& vel);
    VECTOR GetVelocity() const;
    void SetAngularVelocity(const VECTOR& angVel);

    void Update(float deltaTime);
    void Draw(VECTOR offset = VGet(0, 0, 0)) const;
    void DrawWithMatrix(const MATRIX& worldMat) const;

    void SetVertices(const std::vector<VERTEX3D>& vertices);
    void SetIndices(const std::vector<WORD>& indices);
    void SetTextureHandle(int handle);
    int  GetTextureHandle() const;


    void SetDrawRotateX(float angle) { _drawRotateX = angle; }
    float GetDrawRotateX() const { return _drawRotateX; }
    void SetDrawRotateY(float angle) { _drawRotateY = angle; }
    float GetDrawRotateY() const { return _drawRotateY; }
    void SetDrawRotateZ(float angle) { _drawRotateZ = angle; }
    float GetDrawRotateZ() const { return _drawRotateZ; }

    void SetDrawMove(const VECTOR& move) { vMove = move; }
    VECTOR GetDrawMove() const { return vMove; }

    void SetVerticesAsBox(float width, float height, COLOR_U8 cu8 = GetColorU8(255, 255, 255, 255));

    void SetColor(const Color& _color) { color = _color; }
private:
    void UpdateTransformedVertices() const;
};
