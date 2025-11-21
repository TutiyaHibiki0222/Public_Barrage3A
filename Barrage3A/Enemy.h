/*
    ◆ Enemy.cpp

    クラス名        : Enemy クラス
    作成日          : 2025/07/17 (木) 9:41:58
    最終変更日      :
    作成者          :
    概要            : クラスの概要をここに記述してください
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "Transform2D.h"
#include "ItemBase.h"
#include "EnemyScript.h"
#include "ShotScript.h"
#include "Animator.h"
#include "SpriteRenderer.h"
class EnemyScript;  // 前方宣言

struct EnemyStatus {
    int hp = 100;
};

struct DropItemInfo {
    ItemType type;
    int quantity;
};

// Enemy class and AppBase class
class Enemy : public AppBase , public std::enable_shared_from_this<Enemy>
{
private:
    EnemyStatus status;
    std::vector<DropItemInfo> dropItems;
    Vector2D acceleration               = Vector2D(0, 0);
    Vector2D velocity                   = Vector2D(0,0);
    float rotation                      = 0.f;
    std::shared_ptr<EnemyScript> script     = nullptr;
    std::shared_ptr<ShotScript> shotScript = nullptr;
    std::string customUpdateFunctionID;
	std::shared_ptr<Animator> animator = nullptr; // アニメーション用
	std::shared_ptr<SpriteRenderer> spriteRenderer = nullptr; // スプライトレンダラー
    Vector2D previousPosition;

    int damageFlashTimer = 0;           // フレーム単位（例：5フレーム点滅）
    const int maxDamageFlashFrame = 10;  // 何フレーム暗くするか
public:
    Enemy();
    Enemy(std::shared_ptr<GameObject> owner);

    void Awake()        override;
	void Start()        override;
    void Update()       override;
    void OnDestroy()    override;

    float GetSpeed() const {
        return velocity.Length();
    }

    void SetSpeed(float speed) {
        float currentSpeed = velocity.Length();
        if (currentSpeed > 0.0001f) {
            velocity = velocity.GetNormalize() * speed;
        }
        else {
            velocity = Vector2D(speed, 0.f);
        }
    }

    void SetAcceleration(const Vector2D& a) {
        acceleration = a;
    }

    void SetCustomUpdateFunction(const std::string& funcId) {
        customUpdateFunctionID = funcId;
    }

    const std::string& GetCustomUpdateFunction() const {
        return customUpdateFunctionID;
    }

    Vector2D GetPosition() const;
    void SetPosition(const Vector2D&);
    Vector2D GetVelocity() const;
    void SetVelocity(const Vector2D&);
    float GetRotation() const;
    void SetRotation(float);

    void Damage(int _damage);

    // スクリプト連携API
    void SetScript(std::shared_ptr<EnemyScript> s) { script = s; }
    void SetHp(int hp);
    void SetHitboxEnable(bool enabled);
    void SetAngle(float);
    void Die();
    void AddDropItem(ItemType type, int quantity);
    const std::vector<DropItemInfo>& GetDropItems() const;
    void ClearDropItems();

    void Accelerate(const Vector2D& a);
    void Rotate(float deltaAngle);
    void FireShot(const std::string& shotScriptID);
    void SetType(int templateId, int colorId);
    void EmitEffect(int effectId);
    void SetOffset(int slot, const Vector2D& offset);
    float GetAngle() const;
    void OnCollisionEnter(GameObject* other) override;
private:
    bool IsOutsideScreen(const Vector2D& pos);
    void DropItem();
protected:
    std::shared_ptr<AppBase> Clone() const override;

};
