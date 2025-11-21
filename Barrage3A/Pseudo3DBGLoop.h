#pragma once
#include "AppBase.h"
#include "RendererManager.h"
#include "Time.h"
#include "DxLib.h"
#include <vector>
#include <algorithm>

// 疑似3D台形背景（DrawPolygon3D使用）
class Pseudo3DBGLoop : public AppBase, public IRendererDraw, public std::enable_shared_from_this<Pseudo3DBGLoop> {
private:
    struct Stripe {
        float z;
        int graph;
        VERTEX_3D verts[6];
    };

    std::vector<Stripe> stripes;

    const int stripeCount = 7;                // 描画する台形ストライプの数
    const float scrollSpeed = 2.0f;           // 前方向へのスクロール速度
    const float fromZ = 1000.0f;              // 奥側のZ座標（描画の開始位置）
    const float toZ = -550.0f;                // 手前側のZ座標（描画の終了位置）

    const float widthNear = 90.0f;            // 奥（消失点側）での道の幅
    const float widthFar = 300.0f;            // 手前（プレイヤー側）での道の幅

    const float screenCenterX = WIDTH / 2.0f - 100.0f; // 道の中央を画面左からオフセットして配置
    const float baseY = 1250;                 // 消失点のY座標（画面上）
    const float vanishY = 30.0f;              // 手前のY座標（画面下）

    int graphHandle = -1;

public:
    Pseudo3DBGLoop() : AppBase("Pseudo3DBGLoop") {}

    void Awake() override {
        RendererManager::GetInstance().AddRenderer(shared_from_this());
        graphHandle = LoadGraph("Resources/Images/Ground.jpg");

        for (int i = 0; i < stripeCount; ++i) {
            Stripe stripe;
            stripe.z = fromZ - ((fromZ - toZ) / stripeCount) * i;
            stripe.graph = graphHandle;
            UpdateVertices(stripe.verts, stripe.z);
            stripes.push_back(stripe);

        }
    }

    void Update() override {
        const float stripeInterval = (fromZ - toZ) / stripeCount;

        for (auto& stripe : stripes) {
            stripe.z -= scrollSpeed;

            if (stripe.z < toZ) {
                // 最も手前にあるストライプのZを基準にぴったり重ねてループ
                float maxZ = toZ;
                for (const auto& s : stripes) {
                    if (s.z > maxZ) maxZ = s.z;
                }
                stripe.z = maxZ + stripeInterval;
            }

            UpdateVertices(stripe.verts, stripe.z);
        }
    }

    void Draw() override {
        // 手前から奥へ描画（Zの降順）
        std::sort(stripes.begin(), stripes.end(), [](const Stripe& a, const Stripe& b) {
            return a.z > b.z;
            });

        for (const auto& stripe : stripes) {
            DrawPolygon3D(stripe.verts, 2, stripe.graph, TRUE);
        }
    }

    void OnDestroy() override {
        RendererManager::GetInstance().Remove(shared_from_this());
    }

     int GetSortingOrder() const override { return -9009; }
    

protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Pseudo3DBGLoop>(*this);
    }

    void UpdateVertices(VERTEX_3D* vtx, float z) {
        const float stripeInterval = (fromZ - toZ) / stripeCount;
        float zNext = z + stripeInterval;

        float t1 = (z - toZ) / (fromZ - toZ); // 奥側
        float t2 = (zNext - toZ) / (fromZ - toZ); // 手前側

        // 台形の奥と手前の幅（遠近感の表現）
        float topW = widthFar + (widthNear - widthFar) * t1;
        float bottomW = widthFar + (widthNear - widthFar) * t2;

        // 台形の奥と手前のY座標（Y方向のスクロール補正）
        float topY = vanishY + (baseY - vanishY) * t1;
        float bottomY = vanishY + (baseY - vanishY) * t2;

        // 👇 中心Xをずらす。全頂点で共通に使う
        const float centerX = screenCenterX;

        // 👇 中心からのオフセットで四辺を算出（共通中心）
        float topL = centerX - topW / 2;
        float topR = centerX + topW / 2;
        float botL = centerX - bottomW / 2;
        float botR = centerX + bottomW / 2;

        // 三角形2枚で台形を構成（反時計回り）
        vtx[0].pos = VGet(topL, topY, z);
        vtx[1].pos = VGet(topR, topY, z);
        vtx[2].pos = VGet(botL, bottomY, zNext);
        vtx[3].pos = VGet(botR, bottomY, zNext);
        vtx[4] = vtx[2]; // 再利用
        vtx[5] = vtx[1]; // 再利用

        // カラーとUV設定
        for (int i = 0; i < 6; ++i) {
            vtx[i].r = vtx[i].g = vtx[i].b = vtx[i].a = 255;
            vtx[i].u = (i % 2 == 0) ? 0.0f : 1.0f;
            vtx[i].v = (i < 2 || i == 5) ? 0.0f : 1.0f;
        }
    }
};