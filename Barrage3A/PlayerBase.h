/*
    ◆ PlayerBase.h

    クラス名        : PlayerBase クラス
    作成日          : 2025/11/14 (金) 21:38:23
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "TouhouBase.h"
#include "WeakAccessor.hpp"
#include "Animator.h"
#include "SpriteRenderer.h"
#include "MagatamaController.h"

// PlayerBase class and AppBase class
class PlayerBase : public TouhouBase {
protected:
    enum PlayerState {
        NORMAL,     // 通常状態
        INVINCIBLE, // 無敵状態（復活直後など）
        HIT,        // 被弾状態（ダメージを受けている）
        BOMBING     // 戻る（未使用？）
    };
    PlayerState state = NORMAL;
    Vector2DInt size{};
    float speed             = 300.0f;
    float invincibleTime    = 2.0f;
    float invincibleTimer   = 0.0f;
    bool isRespawning = false;
    int prevCount           = -1;
    int mgtmCount           = 0;            
    int shotCount           = 0;     
    int lifeCount           = 6;

	System::WeakAccessor<Animator> animator;
	System::WeakAccessor<SpriteRenderer> sprite;
    std::shared_ptr<MagatamaController> magatama;
public:
    void Awake() override;
    void Start() override;
protected: 
    // コンストラクタ.
    PlayerBase();

    void Tick()  override;

    void Move();
    void Shot();
    void ShotBom();

	virtual void Initialize()   {}
	virtual void Shoot()        {}
    virtual void OptionShot(const bool isFocused, const std::vector<Vector2D>& poss) {}
	virtual void Bomb()         {}
    virtual void OnHit()        {}

    void OnCollisionEnter(GameObject* other) override;

    virtual void UpdateNormal();
    virtual void UpdateInvincible();
    virtual void UpdateHit();
private:
    bool IsFocus() const;
    void ClampToScreen(Vector2D& pos, const Vector2DInt& size, const Vector2D& scale);
    void HandleDeath();
    void UpdateInvincibleColor(float time);
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<PlayerBase>(*this);
    }
};