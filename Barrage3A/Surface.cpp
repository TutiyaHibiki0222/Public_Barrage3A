#include "Surface.h"
#include "Define.h"
#include "Mathf.h"

const static float  WIDX = 40.0f;    // X方向の幅（サイズ）
const static float  WIDY = 40.0f;    // Y方向の幅（サイズ）
const static float  WIDZ = 40.0f;    // Z方向の幅（サイズ）
const static float  SCRSPD = 0.1f;   // スクロール速度（Z方向にポリゴンが動く速さ）
const static float  SPACE = 5;       // （未使用？）スペースの値
const static float  SPD = 0.01f;     // 回転速度などに使う少しの速度
const static float csDefRotaX = (-Define::PI / 5.f); // 描画時にX軸回転させる角度（-36度）
const static VECTOR vMOVE = { 0,5,-50 }; // 描画時に加算するオフセット位置（Y上方向5、Z後方-50）

// 各面の頂点のローカル座標4点（Surface::DATANUM=4面分のデータ）
const static VECTOR gvPos[Surface::DATANUM][4] = {
	{{0,WIDY,0},{0,WIDY,WIDZ},{0,0,WIDZ},{0,0,0}},             // 左側面（YZ平面）
	{{0,0,0},{0,0,WIDZ},{WIDX,0,WIDZ},{WIDX,0,0}},             // 下側面（XZ平面）
	{{WIDX,WIDY,0},{WIDX,WIDY,WIDZ},{WIDX,0,WIDZ},{WIDX,0,0}}, // 右側面（YZ平面）
	{{0,WIDY,0},{0,WIDY,WIDZ},{WIDX,WIDY,WIDZ},{WIDX,WIDY,0}}, // 上側面（XZ平面）
};

Surface::Surface(int z, int sHdl, unsigned uType)
{
	VECTOR vPos[DATANUM][4] = {};
	// gvPos のX,Y座標を中央寄せ（-幅/2）に調整し、vPosにコピー
	for (int i = 0; i < DATANUM; i++) {
		for (int j = 0; j < 4; j++) {
			vPos[i][j].x = gvPos[i][j].x - WIDX / 2; // X座標を中央寄せ
			vPos[i][j].y = gvPos[i][j].y - WIDY / 2; // Y座標を中央寄せ
			// Z座標は後で面ごとに計算
		}
	}
	unsigned Tp = uType;    // 面の種類（0~3）
	_handle = sHdl;         // 使用するテクスチャハンドル
	_angle = VGet(-SPD, 0.0f, -SPD); // 回転速度（未使用かもしれません）

	// 4頂点の位置・法線・色・UVを設定
	// Z座標はz * WIDZでオフセットして連続面を表現

	// 頂点0（左上）
	_vertex[0].pos = VGet(vPos[Tp][0].x, vPos[Tp][0].y, vPos[Tp][0].z + WIDZ * z);
	_vertex[0].norm = VGet(1.0f, 0.0f, 0.0f); // 法線（面の向きに合わせて調整必要）
	_vertex[0].dif = GetColorU8(255, 255, 255, 255); // 白色
	_vertex[0].spc = GetColorU8(0, 0, 0, 0);         // スペキュラ無し
	_vertex[0].u = 0.0f;                             // テクスチャUV左上
	_vertex[0].v = 0.0f;
	_vertex[0].su = 0.0f;                            // サブUV（未使用）
	_vertex[0].sv = 0.0f;

	// 頂点1（右上）
	_vertex[1].pos = VGet(vPos[Tp][1].x, vPos[Tp][1].y, vPos[Tp][1].z + WIDZ * (z + 1));
	_vertex[1].norm = VGet(0.0f, 1.0f, 0.0f);
	_vertex[1].dif = GetColorU8(255, 255, 255, 255);
	_vertex[1].spc = GetColorU8(0, 0, 0, 0);
	_vertex[1].u = 1.0f;                             // UV右上
	_vertex[1].v = 0.0f;
	_vertex[1].su = 0.0f;
	_vertex[1].sv = 0.0f;

	// 頂点2（右下）
	_vertex[2].pos = VGet(vPos[Tp][2].x, vPos[Tp][2].y, vPos[Tp][2].z + WIDZ * (z + 1));
	_vertex[2].norm = VGet(-1.0f, 0.0f, 0.0f);
	_vertex[2].dif = GetColorU8(255, 255, 255, 255);
	_vertex[2].spc = GetColorU8(0, 0, 0, 0);
	_vertex[2].u = 1.0f;                             // UV右下
	_vertex[2].v = 1.0f;
	_vertex[2].su = 0.0f;
	_vertex[2].sv = 0.0f;

	// 頂点3（左下）
	_vertex[3].pos = VGet(vPos[Tp][3].x, vPos[Tp][3].y, vPos[Tp][3].z + WIDZ * z);
	_vertex[3].norm = VGet(0.0f, -1.0f, 0.0f);
	_vertex[3].dif = GetColorU8(255, 255, 255, 255);
	_vertex[3].spc = GetColorU8(0, 0, 0, 0);
	_vertex[3].u = 0.0f;                             // UV左下
	_vertex[3].v = 1.0f;
	_vertex[3].su = 0.0f;
	_vertex[3].sv = 0.0f;

	// インデックス設定（二つの三角形で四角ポリゴン）
	_index[0] = 0;
	_index[1] = 1;
	_index[2] = 2;
	_index[3] = 2;
	_index[4] = 3;
	_index[5] = 0;

	for (int i = 0; i < 4; i++) {
		_initialVertex[i] = _vertex[i];
	}
}

bool Surface::Update()
{
	_scrollAccum += SCRSPD;  // 累積移動量を加算

	// 初期頂点のZ座標から累積移動量を引くことでズレを防止
	for (int i = 0; i < 4; i++) {
		_vertex[i].pos.z = _initialVertex[i].pos.z - _scrollAccum;
	}
	return true;
}

void Surface::Draw(VECTOR offset)
{
	// X軸で回転する行列を作成（ステージに奥行き感を付けるため）
	MATRIX	 Matrix = MGetRotX(csDefRotaX);

	// 頂点配列をコピーし、描画用に変換を加える
	VERTEX3D tmpVertex[4];
	memcpy(tmpVertex, _vertex, sizeof(_vertex));

	for (int i = 0; i < 4; i++) {
		tmpVertex[i].pos = VTransform(tmpVertex[i].pos, Matrix); // X軸回転
		tmpVertex[i].pos = VAdd(tmpVertex[i].pos, vMOVE);        // 全体オフセット（高さ・奥行き調整）
		tmpVertex[i].pos = VAdd(tmpVertex[i].pos, offset);       // 呼び出し元からの追加オフセット
	}

	// テクスチャ付きポリゴンを描画（Zバッファ有効）
	DrawPolygonIndexed3D(tmpVertex, 4, _index, 2, _handle, TRUE);
}

float Surface::GetZ()
{
	// Z座標取得（スクロール判定用）
	return _vertex[0].pos.z;
}

float Surface::GetZWid()
{
	// ポリゴンのZ方向幅（40）
	return WIDZ;
}

void Surface::AddZ(float fZ)
{
	_scrollAccum -= fZ;  // AddZの加算はZ座標にプラスなので累積では引く
}

