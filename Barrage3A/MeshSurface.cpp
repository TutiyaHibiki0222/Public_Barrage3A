#include "MeshSurface.h"
#include "Mathf.h"

MeshSurface::MeshSurface()
    : _position{ 0,0,0 }
    , _rotation{ 0,0,0 }
    , _scale{ 1,1,1 }
    , _textureHandle(-1)
    , _visible(true)
{
    _localMatrix = MGetIdent();
}

void MeshSurface::SetVertices(const std::vector<VERTEX3D>& verts, const std::vector<u16>& indices) {
    _vertices = verts;
    _indices = indices;
}

void MeshSurface::SetTexture(int texHandle) {
    _textureHandle = texHandle;
}

void MeshSurface::SetPosition(const VECTOR& pos) {
    _position = pos;
    UpdateTransform();
}

void MeshSurface::SetRotation(const VECTOR& rot) {
    _rotation = rot;
    UpdateTransform();
}

void MeshSurface::SetScale(const VECTOR& scale) {
    _scale = scale;
    UpdateTransform();
}

void MeshSurface::SetVisible(bool visible) {
    _visible = visible;
}

bool MeshSurface::Update() {
    // 移動制御は外部のStage側で行う
    return _visible;
}

void MeshSurface::UpdateTransform() {
    MATRIX s = MGetScale(_scale);
    MATRIX rx = MGetRotX(_rotation.x);
    MATRIX ry = MGetRotY(_rotation.y);
    MATRIX rz = MGetRotZ(_rotation.z);
    MATRIX t = MGetTranslate(_position);
    _localMatrix = MMult(MMult(MMult(s, rz), ry), rx);
    _localMatrix = MMult(_localMatrix, t);
}

void MeshSurface::Draw() {
    DrawWithOffset(VGet(0, 0, 0));
}

void MeshSurface::DrawWithOffset(const VECTOR& offset) {
    if (!_visible || _textureHandle < 0) return;

    std::vector<VERTEX3D> transformedVerts = _vertices;
    for (auto& v : transformedVerts) {
        v.pos = VTransform(v.pos, _localMatrix); // ローカル変換
        v.pos = VAdd(v.pos, offset);             // unitのワールド位置オフセット
        v.pos = VAdd(v.pos, VGet(0, 5, -50));     // 全体オフセット（高さ・奥行き調整）
    }

    DrawPolygonIndexed3D(
        transformedVerts.data(), (int)transformedVerts.size(),
        _indices.data(), (int)(_indices.size() / 3),
        _textureHandle, TRUE
    );
}
