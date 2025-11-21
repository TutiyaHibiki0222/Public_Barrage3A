#include "Stage3.h"
#include "Texture2DShortcut.hpp"
#include "Dx3DCamera.h"
#include "Define.h"
#include "GameObject.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"
constexpr int START_N = -1;
constexpr int END_N = 7;

const static float  WIDX = 40.0f;    // X方向の幅（サイズ）
const static float  WIDY = 40.0f;    // Y方向の幅（サイズ）
const static float  WIDZ = 40.0f;    // Z方向の幅（サイズ）
const static float  SCRSPD = 0.1f;   // スクロール速度（Z方向にポリゴンが動く速さ）
const static float  SPD = 0.01f;     // 回転速度などに使う少しの速度
const static float csDefRotaX = (-Define::PI / 5.f); // 描画時にX軸回転させる角度（-36度）
const static VECTOR vMOVE = { 0,5,-50 }; // 描画時に加算するオフセット位置（Y上方向5、Z後方-50）

// 各面の頂点のローカル座標4点（Surface::DATANUM=4面分のデータ）
const static VECTOR gvPos[4][4] = {
    {{0,WIDY,0},{0,WIDY,WIDZ},{0,0,WIDZ},{0,0,0}},             // 左側面（YZ平面）
    {{0,0,0},{0,0,WIDZ},{WIDX,0,WIDZ},{WIDX,0,0}},             // 下側面（XZ平面）
    {{WIDX,WIDY,0},{WIDX,WIDY,WIDZ},{WIDX,0,WIDZ},{WIDX,0,0}}, // 右側面（YZ平面）
    {{0,WIDY,0},{0,WIDY,WIDZ},{WIDX,WIDY,WIDZ},{WIDX,WIDY,0}}, // 上側面（XZ平面）
};

// 各面ごとの法線（正規化済み）
const VECTOR normals[4] = {
    VGet(1, 0, 0),
    VGet(0, 1, 0),
    VGet(-1, 0, 0),
    VGet(0, -1, 0)
};

// インデックスは常に同じ（2三角形分）
const WORD indices[6] = { 0,1,2, 2,3,0 };

void SetupStageUnitGeometry(std::shared_ptr<StageUnit>& unit, int type, int z, int texHandle)
{
    std::vector<VERTEX3D> vertices(4);

    for (int i = 0; i < 4; ++i) {
        VECTOR pos = gvPos[type][i];
        pos.x -= WIDX / 2.0f;  // 中心化
        pos.y -= WIDY / 2.0f;

        //pos.z += WIDZ * z; // Z方向に連続配置

        vertices[i].pos = pos;
        vertices[i].norm = normals[type];
        vertices[i].dif = GetColorU8(255, 255, 255, 255);
        vertices[i].spc = GetColorU8(0, 0, 0, 0);
        // UV割り当て（左上から時計回り）
        vertices[i].u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        vertices[i].v = (i >= 2) ? 1.0f : 0.0f;
        vertices[i].su = vertices[i].sv = 0.0f;
    }

    std::vector<WORD> idxVec(indices, indices + 6);

    unit->SetVertices(vertices);
    unit->SetIndices(idxVec);
    unit->SetTextureHandle(texHandle);
    unit->SetPosition(VGet(0, 0, WIDZ * z));
    unit->SetVelocity(VGet(0, 0, -SCRSPD));
    unit->SetDrawRotateX(csDefRotaX);
}

void SetupStageUnitForSphereSegment(std::shared_ptr<StageUnit>& unit,
    float centerX, float centerY, float centerZ,
    float size, int texHandle)
{
    // ここでは単純な正方形ポリゴンを作る（球の表面の小区画に見立てる）
    std::vector<VERTEX3D> vertices(4);

    // 四角の4頂点、単位はローカル座標（球の表面に沿うように調整必要）
    vertices[0].pos = VGet(-size / 2, size / 2, 0);
    vertices[1].pos = VGet(size / 2, size / 2, 0);
    vertices[2].pos = VGet(size / 2, -size / 2, 0);
    vertices[3].pos = VGet(-size / 2, -size / 2, 0);

    // 法線は外向き（簡易的にZ軸方向）
    for (auto& v : vertices) {
        v.norm = VGet(0, 0, 1);
        v.dif = GetColorU8(255, 255, 255, 255);
        v.spc = GetColorU8(0, 0, 0, 0);
    }

    // UVは四角テクスチャ全体を割り当てる
    vertices[0].u = 0.0f; vertices[0].v = 0.0f;
    vertices[1].u = 1.0f; vertices[1].v = 0.0f;
    vertices[2].u = 1.0f; vertices[2].v = 1.0f;
    vertices[3].u = 0.0f; vertices[3].v = 1.0f;

    std::vector<WORD> idxVec = { 0,1,2, 2,3,0 };

    unit->SetVertices(vertices);
    unit->SetIndices(idxVec);
    unit->SetTextureHandle(texHandle);

    // ローカル座標で配置
    unit->SetPosition(VGet(centerX, centerY, centerZ));
    unit->SetScale(VGet(1, 1, 1));
}

void SetupCubeUnit(std::shared_ptr<StageUnit>& unit, VECTOR center, VECTOR size, int texHandle)
{
    std::vector<VERTEX3D> vertices(24);
    std::vector<WORD> indices(36);

    float hx = size.x / 2.0f;
    float hy = size.y / 2.0f;
    float hz = size.z / 2.0f;

    // 各面のローカル頂点（順序：前,後,左,右,上,下）
    VECTOR cubeVerts[8] = {
        VGet(-hx, -hy, -hz), VGet(hx, -hy, -hz),  // 0,1 : 下前左右
        VGet(hx,  hy, -hz), VGet(-hx,  hy, -hz),  // 2,3
        VGet(-hx, -hy,  hz), VGet(hx, -hy,  hz),  // 4,5 : 上後左右
        VGet(hx,  hy,  hz), VGet(-hx,  hy,  hz),  // 6,7
    };

    // 面の構成（頂点インデックス4つと法線）
    struct Face {
        int idx[4];
        VECTOR normal;
    };

    Face faces[6] = {
        {{0,1,2,3}, VGet(0,0,-1)}, // 前
        {{5,4,7,6}, VGet(0,0,1)},  // 後
        {{4,0,3,7}, VGet(-1,0,0)}, // 左
        {{1,5,6,2}, VGet(1,0,0)},  // 右
        {{3,2,6,7}, VGet(0,1,0)},  // 上
        {{4,5,1,0}, VGet(0,-1,0)}  // 下
    };

    for (int f = 0; f < 6; ++f) {
        for (int i = 0; i < 4; ++i) {
            VERTEX3D& v = vertices[f * 4 + i];
            v.pos = VAdd(cubeVerts[faces[f].idx[i]], center);
            v.norm = faces[f].normal;
            v.dif = GetColorU8(255, 255, 255, 255);
            v.spc = GetColorU8(0, 0, 0, 0);
            // UV（左上から時計回り）
            v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
            v.v = (i >= 2) ? 1.0f : 0.0f;
        }
        // インデックス追加（2三角形）
        int base = f * 4;
        indices[f * 6 + 0] = base + 0;
        indices[f * 6 + 1] = base + 1;
        indices[f * 6 + 2] = base + 2;
        indices[f * 6 + 3] = base + 2;
        indices[f * 6 + 4] = base + 3;
        indices[f * 6 + 5] = base + 0;
    }

    unit->SetVertices(vertices);
    unit->SetIndices(indices);
    unit->SetTextureHandle(texHandle);
    unit->SetPosition(center);
}

Stage3::~Stage3() {
	Sounds["#!stage1"];
}

void Stage3::Init()
{
	Sounds["#?stage1"];
    // テクスチャハンドルを配列で管理
    int texHandles[4] = {
        GameEngine::Tex["Stages/3D/Stage01:fusuma"]->spriteData,
        GameEngine::Tex["Stages/3D/Stage01:floor"]->spriteData,
        GameEngine::Tex["Stages/3D/Stage01:fusuma"]->spriteData,
        0
    };

    _units.clear();

    for (int t = 0; t < 4; ++t) {
        for (int z = START_N; z < END_N; ++z) {
            auto unit = std::make_shared<StageUnit>();
            SetupStageUnitGeometry(unit, t, z, texHandles[t]);
            _units.push_back(unit);
        }
    }

    Dx3DCamera::Instance().SetFogEnable(TRUE);
    Dx3DCamera::Instance().SetFogColor(0, 0, 0);
    Dx3DCamera::Instance().SetFogRange(100.0f, 250.0f);

    MATERIALPARAM Material{};
    Material.Diffuse = GetColorF(0, 0, 0, 1);
    Material.Specular = GetColorF(0, 0, 0, 0);
    Material.Ambient = GetColorF(1, 1, 1, 1);
    Material.Emissive = GetColorF(1, 1, 1, 0);
    Material.Power = 20.0f;

    SetMaterialParam(Material);
    SetMaterialUseVertSpcColor(FALSE);
    SetMaterialUseVertDifColor(FALSE);

    frame = 0;

	// 音
	{
		// 読み込み.
		Sounds["#?stage1"];
		// もしBGMを最初に鳴らしたいならここで再生開始
		auto musicController = GameObject::Instantiate("MusicController")->AddAppBase<MusicController>();
		if (musicController) {
			auto bgmClip = Sounds["bgm_stage1"];
			musicController->SetMusicClip(bgmClip);
			musicController->SetLoopSong(true);
			musicController->SetMusicVolume(0.8f);
		}
	}
}

void Stage3::Update()
{
    for (auto& unit : _units) {
        unit->Update(1.0f);  // deltaTime = 1.0f固定
        VECTOR pos = unit->GetPosition();

        if (pos.z < WIDZ * (START_N - 1)) {
            pos.z += WIDZ * (END_N - START_N);
            unit->SetPosition(pos);
        }
    }
    frame++;
}

void Stage3::Draw()
{

    SetDrawMode(DX_DRAWMODE_BILINEAR);

    VECTOR offset = VGet(0, 0, 0);

    float rotateY = sinf(frame * 0.001f) * 1.5f;
    float rotateX = cosf(frame * 0.0015f) * 0.5f;

    MATRIX rotY = MGetRotY(rotateY);
    MATRIX rotX = MGetRotX(rotateX);
    MATRIX rot  = MMult(rotX, rotY);

    VECTOR baseUp = VGet(0, 1, 0);
    VECTOR baseTarget = VGet(0, 0, 0);

    VECTOR rotatedUp = VTransform(baseUp, rot);
    VECTOR rotatedTarget = VTransform(baseTarget, rot);

    Dx3DCamera::Instance().SetNearFar(1.0f, 10000.f);
    Dx3DCamera::Instance().SetScreenCenter(Define::OUT_W / 2.f, Define::OUT_H / 2.f);
    Dx3DCamera::Instance().SetCamera(VGet(0, 0, -95),rotatedTarget, rotatedUp);
    Dx3DCamera::Instance().Apply();

    for (auto& unit : _units) {
        unit->Draw(offset);
    }
    SetDrawMode(DX_DRAWMODE_NEAREST);
}
