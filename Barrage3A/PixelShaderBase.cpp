/*
    ◆ PixelShaderBase.cpp

    クラス名        : PixelShaderBase クラス
    作成日          : 2025/07/27 (日) 14:14:41
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "PixelShaderBase.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "RendererManager.h"

// コンストラクタ 
PixelShaderBase::PixelShaderBase() : AppBase("PixelShaderBase"){

}
PixelShaderBase::PixelShaderBase(std::shared_ptr<GameObject> owner) : AppBase("PixelShaderBase", owner) {

}

PixelShaderBase::~PixelShaderBase() {
    if(shader != -1) {
        DeleteShader(shader); // シェーダーの削除
        shader = -1; // シェーダーIDを無効化
	}
    if(scene != -1) {
        DeleteGraph(scene); // シーンの削除
        scene = -1; // シーンIDを無効化
	}
}

// AppBase Event.
void PixelShaderBase::Awake() {
	RendererManager::GetInstance().AddRenderer(shared_from_this());
}

void PixelShaderBase::Start() {
    LoadShader();
    InitializeShader();
	auto size = Window::GetInstance().GetMaxVector2D();
    scene = MakeScreen((int)size.x, (int)size.y, true);

    for (int i = 0; i < 4; i++)
    {
        vertex[i].pos = VGet((i % 2) * size.x, (i / 2) * size.y, 0);
        vertex[i].rhw = 1.0f;
        vertex[i].dif = GetColorU8(255, 255, 255, 255);
        vertex[i].spc = GetColorU8(0, 0, 0, 0);
        vertex[i].u = vertex[i].su = (float)(i % 2);
        vertex[i].v = vertex[i].sv = (float)(i / 2);
    }
}

void PixelShaderBase::OnDestroy() {
    RendererManager::GetInstance().Remove(shared_from_this());
}

bool PixelShaderBase::IsDraw() {
    return gameObject->IsActive() && enabled && (shader != -1);
}

void PixelShaderBase::Draw() {
    if (shader == -1) return; // シェーダーがロードされていない場合は描画しない

    auto size = Window::GetInstance().GetMaxVector2D();
    // 現在描かれてる画像を取得.
    GetDrawScreenGraph(0, 0, (int)size.x, (int)size.y, scene);

    // シェーダーをセット.
    SetUsePixelShader(shader);
    OnUpdateShader();

    DrawPrimitive2DToShader(vertex, 4, DX_PRIMTYPE_TRIANGLESTRIP);

    SetUsePixelShader(-1);
}