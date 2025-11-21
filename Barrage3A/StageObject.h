#pragma once
#include <vector>
#include <memory>
#include "StageUnit.h"
#include <DxLib.h>

class StageObject {
private:
    std::vector<std::shared_ptr<StageUnit>> _units;  // 内包するパーツ群（相対座標系）
    VECTOR _position = VGet(0, 0, 0);
    VECTOR _rotation = VGet(0, 0, 0);
    VECTOR _scale = VGet(1, 1, 1);

    mutable std::vector<VERTEX3D> _transformedVerticesCache; // キャッシュ（任意）

public:
    StageObject() = default;

    void AddUnit(std::shared_ptr<StageUnit> unit) {
        _units.push_back(unit);
    }

    // 親オブジェクトのトランスフォーム設定・取得
    void SetPosition(const VECTOR& pos) { _position = pos; }
    void SetRotation(const VECTOR& rot) { _rotation = rot; }
    void SetScale(const VECTOR& scale) { _scale = scale; }

    VECTOR GetPosition() const { return _position; }
    VECTOR GetRotation() const { return _rotation; }
    VECTOR GetScale() const { return _scale; }

    void Update(float deltaTime) {
        for (auto& unit : _units) {
            unit->Update(deltaTime);
        }
    }

    void Draw(const VECTOR& offset) const {
        // 親の変換行列を作成
        MATRIX matScale = MGetScale(_scale);
        MATRIX matRotX = MGetRotX(_rotation.x);
        MATRIX matRotY = MGetRotY(_rotation.y);
        MATRIX matRotZ = MGetRotZ(_rotation.z);
        MATRIX matRot = MMult(matRotZ, MMult(matRotY, matRotX));

        // オフセットを加えた移動
        VECTOR offsetPosition = VAdd(_position, offset);
        MATRIX matTrans = MGetTranslate(offsetPosition);

        MATRIX parentMat = MMult(matRot, matScale);
        parentMat = MMult(parentMat, matTrans);

        for (auto& unit : _units) {
            // 各Unitのローカル変換行列を作成
            VECTOR uPos = unit->GetPosition();
            VECTOR uRot = unit->GetRotation();
            VECTOR uScale = unit->GetScale();

            MATRIX uScaleMat = MGetScale(uScale);
            MATRIX uRotXMat = MGetRotX(uRot.x);
            MATRIX uRotYMat = MGetRotY(uRot.y);
            MATRIX uRotZMat = MGetRotZ(uRot.z);
            MATRIX uRotMat = MMult(uRotZMat, MMult(uRotYMat, uRotXMat));
            MATRIX uTransMat = MGetTranslate(uPos);

            MATRIX localMat = MMult(uRotMat, uScaleMat);
            localMat = MMult(localMat, uTransMat);

            // ワールド変換 = 親の変換 * ローカル変換
            MATRIX worldMat = MMult(localMat, parentMat);

            unit->DrawWithMatrix(worldMat);
        }
    }

};
