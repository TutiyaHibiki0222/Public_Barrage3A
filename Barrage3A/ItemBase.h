/*
    ◆ ItemBase.h

    クラス名        : ItemBase クラス
    作成日          : 2025/07/18 (金) 19:19:47
    最終変更日      : 
    作成者          : 
    概要            : フィールド上に出現する各種アイテムのベースクラス。
                    : 落下挙動・アイテムタイプによる効果などの共通処理を行う。
*/
#pragma once
#include "headers.h"
#include "AppBase.h"
#include "SpriteRenderer.h"

enum class ItemType {
    Score,              // 得点アイテム.
	ScoreBig,           // 大得点アイテム.
    Power,              // 霊力.
	PowerBig,           // 大霊力.
    LifePiece,          // 残基 (欠片).
    LifeFull,           // 残基.
    BombPiece,          // スペルカード (欠片).
    BombFull,           // スペルカード.
    PointExtend,        // エクステンド点.
    Custom,             // ユーザー定義のCustomアイテム.
};

// ItemBase class and AppBase class
class ItemBase : public AppBase , public std::enable_shared_from_this<ItemBase>
{
private:    // 非公開.
    ItemType itemType   = ItemType::Score;
    Vector2D velocity   = Vector2D(0, 0);
    float gravity       = -0.03f;
    float maxFallSpeed  = -3.0f;
    float collectSpeed  =  10.0f;
    float spinTimer = 0.0f;           // 残りの回転時間
    float initialSpinSpeed = 25.0f;    // 最初の回転速度（度/フレーム）
    bool isAutoCollect  = false; // 自動吸収中かどうか
    bool hasRisen = false; // 上昇を経験したかどうか
    std::shared_ptr<SpriteRenderer> sprite;
public: // 公開.

    // コンストラクタ.
    ItemBase();
    ItemBase(std::shared_ptr<GameObject>);

    // AppBase 関数.
    void Awake()        override;
    void Start()        override;
    void Update()       override;
    void OnDestroy()    override;
    virtual void OnItemEvent();
    // 自動回収 強制.
    void AutoCollect();    
    void SetType(ItemType type);
    void SetSpinDuration(float time); // 回転する秒数を設定
protected:
    virtual std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<ItemBase>(*this);
    };
};