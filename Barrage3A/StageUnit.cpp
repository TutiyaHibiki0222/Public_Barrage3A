#include "StageUnit.h"
#include <cstring> // memcpy
#include <cmath>

StageUnit::StageUnit() {}

StageUnit::~StageUnit() {}

void StageUnit::SetPosition(const VECTOR& pos) {
    _position = pos;
    _dirty = true;
}

void StageUnit::SetRotation(const VECTOR& rot) {
    _rotation = rot;
    _dirty = true;
}

void StageUnit::SetScale(const VECTOR& scale) {
    _scale = scale;
    _dirty = true;
}

void StageUnit::SetVelocity(const VECTOR& vel) {
    _velocity = vel;
}

VECTOR StageUnit::GetVelocity() const {
    return _velocity;
}

void StageUnit::SetAngularVelocity(const VECTOR& angVel) {
    _angularVelocity = angVel;
}

void StageUnit::SetVertices(const std::vector<VERTEX3D>& vertices) {
    _vertices = vertices;
    _dirty = true;
}

void StageUnit::SetIndices(const std::vector<WORD>& indices) {
    _indices = indices;
}

void StageUnit::SetTextureHandle(int handle) {
    _textureHandle = handle;
}

int StageUnit::GetTextureHandle() const {
    return _textureHandle;
}

void StageUnit::Update(float deltaTime) {
    // 位置・回転の更新
    _position.x += _velocity.x * deltaTime;
    _position.y += _velocity.y * deltaTime;
    _position.z += _velocity.z * deltaTime;

    _rotation.x += _angularVelocity.x * deltaTime;
    _rotation.y += _angularVelocity.y * deltaTime;
    _rotation.z += _angularVelocity.z * deltaTime;

    _dirty = true;
}

void StageUnit::UpdateTransformedVertices() const {
    //if (!_dirty) return;
    _dirty = false;

    if (_vertices.empty()) {
        _transformedVertices.clear();
        return;
    }

    // 変換済み頂点を確保
    _transformedVertices.resize(_vertices.size());

    // 変換行列を作成（スケール→回転X→回転Y→回転Z→平行移動）
    MATRIX matScale = MGetScale(_scale);
    MATRIX matRotX = MGetRotX(_rotation.x);
    MATRIX matRotY = MGetRotY(_rotation.y);
    MATRIX matRotZ = MGetRotZ(_rotation.z);

    MATRIX matRot = MMult(matRotZ, MMult(matRotY, matRotX));
    MATRIX mat = MMult(matScale, matRot);

    // 平行移動を加える行列
    mat.m[3][0] = _position.x;
    mat.m[3][1] = _position.y;
    mat.m[3][2] = _position.z;

    for (size_t i = 0; i < _vertices.size(); i++) {
        // 位置を変換
        _transformedVertices[i] = _vertices[i];
        _transformedVertices[i].pos = VTransform(_vertices[i].pos, mat);
        _transformedVertices[i].norm = VTransform(_vertices[i].norm, matRot);
    }
}

void StageUnit::Draw(VECTOR offset) const {
    if (_textureHandle < 0 || _vertices.empty() || _indices.empty()) return;

    UpdateTransformedVertices();

    // 3軸回転行列を作成（Z→Y→Xの順で回転）
    MATRIX rotX = MGetRotX(_drawRotateX);
    MATRIX rotY = MGetRotY(_drawRotateY);  // Xのままだったので修正
    MATRIX rotZ = MGetRotZ(_drawRotateZ);
    MATRIX rotationMatrix = MMult(rotZ, MMult(rotY, rotX));

    for (auto& v : _transformedVertices) {
        VECTOR pos = VTransform(v.pos, rotationMatrix); // 3軸回転まとめて適用
        pos = VAdd(pos, vMove);                          // vMoveを加算
        pos = VAdd(pos, offset);                         // offsetを加算
        v.pos = pos;
    }

    SetDrawBright(color.R255(), color.G255(), color.B255());
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, color.A255());

    DrawPolygonIndexed3D(_transformedVertices.data(), (int)_transformedVertices.size(),
        _indices.data(), (int)(_indices.size() / 3), _textureHandle, TRUE);

    SetDrawBright(255, 255, 255);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void StageUnit::DrawWithMatrix(const MATRIX& worldMat) const {
    if (_textureHandle < 0 || _vertices.empty() || _indices.empty()) return;

    // 頂点変換用バッファを用意
    std::vector<VERTEX3D> transformedVerts(_vertices.size());

    // 頂点をワールド変換行列で変換
    for (size_t i = 0; i < _vertices.size(); ++i) {
        transformedVerts[i] = _vertices[i]; // コピー

        // 位置を変換
        transformedVerts[i].pos = VTransform(_vertices[i].pos, worldMat);
    }

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, color.A255());
    SetDrawBright(color.R255(), color.G255(), color.B255());

    // 描画
    DrawPolygonIndexed3D(
        transformedVerts.data(),
        (int)transformedVerts.size(),
        _indices.data(),
        (int)(_indices.size() / 3),
        _textureHandle,
        TRUE
    );

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    SetDrawBright(255, 255, 255);
}

void StageUnit::SetVerticesAsBox(float width, float height, COLOR_U8 cu8)
{
    _vertices.clear();
    _indices.clear();

    float hw = width * 0.5f;
    float hh = height * 0.5f;

    // 頂点4つを作成
    _vertices.resize(4);

    // 左上
    _vertices[0].pos = VGet(-hw, hh, 0);
    _vertices[0].norm = VGet(0, 0, -1);
    _vertices[0].dif = cu8;
    _vertices[0].spc = GetColorU8(0, 0, 0, 0);
    _vertices[0].u = 0.0f;
    _vertices[0].v = 0.0f;
    _vertices[0].su = 0.0f;
    _vertices[0].sv = 0.0f;

    // 右上
    _vertices[1].pos = VGet(hw, hh, 0);
    _vertices[1].norm = VGet(0, 0, -1);
    _vertices[1].dif = cu8;
    _vertices[1].spc = GetColorU8(0, 0, 0, 0);
    _vertices[1].u = 1.0f;
    _vertices[1].v = 0.0f;
    _vertices[1].su = 0.0f;
    _vertices[1].sv = 0.0f;

    // 左下
    _vertices[2].pos = VGet(-hw, -hh, 0);
    _vertices[2].norm = VGet(0, 0, -1);
    _vertices[2].dif = cu8;
    _vertices[2].spc = GetColorU8(0, 0, 0, 0);
    _vertices[2].u = 0.0f;
    _vertices[2].v = 1.0f;
    _vertices[2].su = 0.0f;
    _vertices[2].sv = 0.0f;

    // 右下
    _vertices[3].pos = VGet(hw, -hh, 0);
    _vertices[3].norm = VGet(0, 0, -1);
    _vertices[3].dif = cu8;
    _vertices[3].spc = GetColorU8(0, 0, 0, 0);
    _vertices[3].u = 1.0f;
    _vertices[3].v = 1.0f;
    _vertices[3].su = 0.0f;
    _vertices[3].sv = 0.0f;

    // 三角形インデックス（2枚で四角形）
    _indices = { 0, 1, 2, 2, 1, 3 };
}
