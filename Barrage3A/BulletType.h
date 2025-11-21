#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <functional> // std::hash
#include <stdexcept>  // std::out_of_range

#include "GameObject.h"
#include "Texture2DShortcut.hpp"
#include "SpriteRenderer.h"
#include "Collider2D.h"

// 弾の種類（親ID）を表す列挙型
// 弾の形状や特性に応じて分類
enum class BulletParentID : int {
    B1,             // b1.png
    B4,             // b4.png
    BigB4,          // bigB4.png
    B5,             // b5.png
    B6,             // b6.png
    B7,             // b7.png
    Crystal,        // b8.png
    Kunai,          // b9.png
    MiniB0,         // miniB0.png
    MiniB1,         // miniB1.png
    MiniB2,         // miniB2.png
    Star,           // star.png
    BigSter,        // starBig.png
    B13,            // b13.png
    B14,            // b14.png
    B15,            // b15.png
    Bullet,         // Bullet.png
    Knife,          // knife.png (ナイフ).
    Arraw,          // arraw.png
    BigBullet,      // b0.png

    Count           // 弾の種類の数（列挙型の要素数）
};

// 弾の色を表す列挙型
// 弾の描画色や属性を管理
// 弾の色を表す列挙型（描画や属性指定に使用）
enum class BulletColor : int {
    Gray = 0,         // 灰色：中立・無彩色。背景・無属性などに。
    Red,              // 赤：基本的な攻撃色。
    VividRed,         // 鮮やかな赤：より強調された攻撃色。
    Purple,           // 紫：妖しさや特殊効果に。
    Pink,             // ピンク：柔らかい印象や女性的な表現に。
    Blue,             // 青：冷静・冷気・水属性など。
    LightBlue,        // 明るい青：爽やか・透明感のある表現に。
    Cyan,             // シアン（水色）：中間色。エネルギー系にも使える。
    SkyBlue,          // 空色：明るく穏やかな印象。
    MintGreen,        // ミントグリーン：癒し系・生命系。
    NeonGreen,        // ネオングリーン：毒・異常・未来的表現。
    LimeGreen,        // ライムグリーン：元気・スピード感。
    LemonYellow,      // レモン色：注意喚起やエネルギー表現。
    PastelYellow,     // パステル黄：柔らかく明るい印象。
    Orange,           // オレンジ：元気・炎・活発な印象。
    White,            // 白：無属性・ボス・神聖系など。
    Black,            // 黒：闇・高難度・重厚な敵など。

    Count,            // 色数カウント用（ループ処理などに使用）
    Unknown = 0xFF    // 未定義・読み込み失敗時などに使用
};


namespace std {
    template <>
    struct hash<BulletColor> {
        size_t operator()(const BulletColor& c) const noexcept {
            return std::hash<int>()(static_cast<int>(c));
        }
    };
}
// BulletParentID のハッシュ関数
namespace std {
    template<>
    struct hash<BulletParentID> {
        size_t operator()(BulletParentID id) const noexcept {
            using Underlying = std::underlying_type_t<BulletParentID>;
            return std::hash<Underlying>{}(static_cast<Underlying>(id));
        }
    };
}
struct BulletTypeData
{
    std::string imagePath = "";
    float       hitboxSize  = 0;      // 当たり判定の大きさ.
    std::unordered_map<BulletColor, int> colorToImageIndex;
};

class BulletTypeManager {
private:
    std::unordered_map<BulletParentID, BulletTypeData> bulletTypeMap;

    BulletTypeManager() {}

public:
    static BulletTypeManager& GetInstance() {
        static BulletTypeManager instance;
        return instance;
    }

    // コピー禁止
    BulletTypeManager(const BulletTypeManager&) = delete;
    BulletTypeManager& operator=(const BulletTypeManager&) = delete;

    bool HasBulletType(BulletParentID id) const {
        return bulletTypeMap.find(id) != bulletTypeMap.end();
    }

    void SetBulletType(BulletParentID id, const BulletTypeData& data) {
        bulletTypeMap[id] = data;
    }

    void LoadJson(const std::string& ptch);

    const BulletTypeData& GetBulletType(BulletParentID id) const {
        auto it = bulletTypeMap.find(id);
        if (it == bulletTypeMap.end()) {
            throw std::out_of_range("BulletParentID not found in BulletTypeManager");
        }
        return it->second;
    }

    int GetImageIndex(BulletParentID id, BulletColor color) const {
        const BulletTypeData& data = GetBulletType(id);
        auto it = data.colorToImageIndex.find(color);
        if (it == data.colorToImageIndex.end()) return -1;
        return it->second;
    }

    float GetHitboxSize(BulletParentID id) const {
        auto it = bulletTypeMap.find(id);
        if (it == bulletTypeMap.end()) return 0.0f;
        return it->second.hitboxSize;
    }

    std::shared_ptr<GameObject> CreateBullet(
        BulletParentID id, BulletColor color, 
        std::string tag = "EnemyBullet", 
        Layer layer = Layer::EnemyBullet, 
        int drawLayer = 5)
    {
        // データ取得
        const BulletTypeData& data = GetBulletType(id);

        // インデックス取得
        int index = GetImageIndex(id, color);
        if (index == -1) {
            std::cerr << "[BulletTypeManager] Missing color mapping: BulletParentID="
                << static_cast<int>(id) << ", Color=" << static_cast<int>(color) << std::endl;

            return nullptr; // 登録されていない色
        }

        // Sprite を取得
        auto texture = Tex[data.imagePath+ "_" + std::to_string(index)];
        if (!texture) return nullptr;

        // GameObject 作成
        auto bullet = GameObject::Instantiate("Bullet");
        bullet->SetTag(tag);
        bullet->SetLayer(layer);

        // Sprite
        auto renderer = bullet->AddAppBase<SpriteRenderer>();
        renderer->SetSprite(texture);
        renderer->SetRotation(-90);
		renderer->SetLayer(drawLayer);
        // コライダー追加（とりあえず CircleCollider）
        float radius = data.hitboxSize;
        if (radius > 0.0f) {
            auto collider = bullet->AddAppBase<CircleCollider>();
            collider->SetRadius(radius);
        }

        return bullet;
    }
};
