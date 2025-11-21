/*
    ◆ ParticleSystem.h

    クラス名        : ParticleSystem クラス
    作成日          : 2025/06/28 (土) 04:20:22
    最終変更日      : 2025/06/28 (土) 20:25:05
    作成者          : 𡈽屋 響
    概要            : エフェクト用のパーティクルシステム.
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "IDraw.h"
#include "Text.h"
#include "Filter.hpp"
#include "Texture2D.h"
#include <vector>
#include <memory>
#include <cstdlib>
#include <cmath>

using namespace GameEngine;

enum class SpawnParticleShape {
    Circle,     // 円.
    Square,     // 四角.
    Box,
    Line,       // 横一直線.
};

enum class VelocityDirectionMode {
    Random360,      // 0〜360度ランダム（今の状態）
    Cone,           // 指定角度を中心に円錐状に広がる
    Fixed,          // 固定方向のみ
    Line,           // ある線上の方向
    CustomFunction  // ユーザー定義関数で決定
};

// パーティクルデータ(実体)
struct Particle {
    Vector2D pos;           // 2Dスクリーン座標
    Vector2D vc;            // 2D速度ベクトル
    float size;             // 基本スケール
    float sizeCurrent;      // 現在のスケール（Updateで変化）
	float targetSize;	    // 目標スケール
    Color color;            // RGBAカラー (0～255)
    Color startColor;       // 初回のカラー;
    Vector3D rot;           // 回転角度 (度): x,y は疑似3Dチルト, z はスピン.
    Vector3D rotSpeed;      // 各軸の回転速度 (度/秒)
    float life;             // 残り寿命 (秒)
    float totalLife;        // 初期寿命 (秒)
    bool active;            // アクティブ状態フラグ
    float angleDirection;
};

using ParticleFunc = std::function<void(Particle&, Vector2D,Vector2D)>;
using ParticleDrawFunc = std::function<void(const Particle&, const Sprite&)>;

#define PARTICLE_FUNC [](Particle& p, Vector2D pos, Vector2D scale)

template <typename F>
ParticleFunc MakeParticleFunc(F&& f) {
    return std::forward<F>(f);
}

struct ParticleSettings {
    int maxParticles = 1000;                    // 最大パーティクル数
    float spawnRadiusMin = 0.0f;                // 生成範囲の最小半径（内側の空洞半径）
    float spawnRadiusMax = 0.0f;                // 生成範囲の最大半径（外側の範囲）
    float emissionRate = 100.0f;                // 毎秒生成するパーティクル数
    float emissionInterval = 0.01f;             // パーティクル生成間隔（秒単位）
    float emissionDuration = -1.0f;             // エミッション持続時間（<0なら無限）
    int burstCount = 0;                         // "＞0"なら開始時にこの数だけ一度に生成
    float lifetimeMin = 1.0f;                   // パーティクル寿命最小値
    float lifetimeMax = 2.0f;                   // パーティクル寿命最大値
    float speedMin = 50.0f;                     // 初速最小値
    float speedMax = 150.0f;                    // 初速最大値
    float sizeMin = 0.5f;                       // サイズ最小値
    float sizeMax = 1.5f;                       // サイズ最大値
    float endSizeMin = 2.0f;                    // 寿命終端時のサイズ最小
    float endSizeMax = 4.0f;                    // 寿命終端時のサイズ最大

    Vector3D startRotationMin = { 0, 0, 0 };    // 開始回転最小値 (度)
    Vector3D startRotationMax = { 0, 0, 0 };    // 開始回転最大値 (度)
    Vector3D rotationSpeedMin = { 0, 0, 0 };    // 回転速度最小値（度/秒）
    Vector3D rotationSpeedMax = { 0, 0, 0 };    // 回転速度最大値（度/秒）

    Color   startColor    = { 255,255,255,255 };// Default.
    bool    useRandomStartColor = false;        // true なら次の範囲内でランダムに startColor を決定
    Color   startColorMin = {   0,  0,  0,  0 };// ランダム色の最小 RGBA
    Color   startColorMax = { 255,255,255,255 };// ランダム色の最大 RGBA
    SpawnParticleShape spawnShape = SpawnParticleShape::Circle; // 生成する範囲の形.
    Vector2D spawnBoxMin = { 0, 0 }; // Boxの左上隅など
    Vector2D spawnBoxMax = { 0, 0 }; // Boxの右下隅など
    bool alignToVelocity = false;               // 
    VelocityDirectionMode velocityMode = VelocityDirectionMode::Random360;
    float velocityDirectionDeg  = 90.0f;       // 度で指定（例：下方向）
    float velocitySpreadDeg     = 45.0f;       // 広がり（度）
    std::function<Vector2D()> customVelocityFunc = nullptr; // CustomFunction用
    ParticleFunc customUpdate = nullptr;        // nullptrならデフォルト更新処理
};

// ParticleSystem class and AppBase class
class ParticleSystem : public AppBase, public IRendererDraw, public std::enable_shared_from_this<ParticleSystem>{
private:    // 非公開.
	ParticleSettings settings;
    std::vector<Particle> particles;
    std::shared_ptr<Sprite> sprite;
    SortingLayer sortingLayer;
	RenderMode   renderMode = RenderMode::WorldSpace; // 描画モード.
    float emitCounter   = 0;
    float elapsedTime   = 0;
    bool  burstDone     = 0;
	bool  isEndDestroy  = false; // Destroy() で破棄するかどうか.
	bool  isStop        = false; // 停止状態かどうか.

    ParticleDrawFunc customDraw = nullptr; // nullptrならデフォルト描画
public:     // 公開.

    // コンストラクタ.
    ParticleSystem();
    ParticleSystem(std::shared_ptr<GameObject>);

	// 設定を取得・設定 (レイヤー).
    SortingLayer& GetSortingLayer() { return sortingLayer; } 
	ParticleSettings& GetParticleSettings() { return settings; }
	void SetParticleSettings(const ParticleSettings& _settings) { settings = _settings; }
	void SetRenderMode(RenderMode mode) { renderMode = mode; }
    void SetSprite(const std::shared_ptr<Sprite>& sp) { sprite = sp; }
	void SetEndDestroy(bool end) { isEndDestroy = end; } // Destroy() で破棄するかどうか.
	void Stop()     { isStop = true; }  // 停止状態にする.
	void Resume()   { isStop = false; } // 停止状態を解除.
	void Restart();                     // 再起動 (停止状態解除 + エミッション再開).

    void SetCustomDraw(ParticleDrawFunc func) { customDraw = std::move(func); }

    // AppBase 関数.
    void Awake()        override;
	void Start()        override;
    void Update()       override;
    void OnDestroy()    override;
	
    // IRendererDraw 関数.
	bool IsDraw()   override;
	void Draw()     override;
    int  GetSortingOrder() const override;
    RenderMode GetRenderMode() const override;

private:
    // 作成.
    void EmitParticle();
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<ParticleSystem>(*this);
    }
};