#pragma once
#include "Header/BulletPatternFactory.h"
#include "Header/EnemyBase.h"

class Enemy02 : public EnemyBase {
public:
	Enemy02() : EnemyBase() {}
	Enemy02(std::shared_ptr<GameObject> _obj) : EnemyBase(_obj) {}

	void Init() override {
		// 初期化（座標、HP、速度など）;
		hp = hpMax = 10;
		isShot = true;
		isStete = false; // falseで通常移動
		if (vel == 0) vel = Vector2D(0, -10); // 方向未設定ならデフォルトで下方向
		gameObject->AddAppBase<BoxCollider>()->SetSize({ 10,10 });
	}
	void Pattern() override {
		// ある時間ごとに弾を撃つ（例：60フレーム毎）
		if (count % 60 == 0) {
			isStete = false;
			isShot = true;
		}
	}

	void Shot() override {
		// nWAY弾 + ランダムばら撒き
		Vector2D origin = transform->position;
		BulletPatternFactory::FireNWay(origin, -90.0f, 15.0f, 5, 200.0f);
		BulletPatternFactory::FireSpread(origin, (int)(Random.Value() * 10));
		isShot = false;
		isStete = true;
	}

protected:
	std::shared_ptr<AppBase> Clone() const override {
		return std::make_shared<Enemy02>(*this);
	}
};
