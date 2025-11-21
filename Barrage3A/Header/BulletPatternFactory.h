#pragma once
#include "BulletBase.h"
#include "../Random.h"
#include "../GameObjectMgr.h"
#include "../Prefab.h"

// 追尾弾用のMoveクラス
class HomingMove : public BulletMove {
private:
	std::shared_ptr<GameObject> player;
	float speed;

public:
	HomingMove(std::shared_ptr<GameObject> p, float s) : player(p), speed(s) {}

	void Move(std::shared_ptr<Transform2D> tr) override {
		if (!player) return;
		Vector2D dir = (player->transform->position - tr->position).GetNormalize();
		tr->position += dir * speed * Time.deltaTime;
	}
};

class BulletPatternFactory {
private:
	// プレイヤー取得
	static std::shared_ptr<GameObject> GetPlayer() {
		return Object.FindWithName("Player");
	}
public:
	// 1. nWAY弾（奇数/偶数を統合）
	static std::vector<std::shared_ptr<GameObject>> FireNWay(Vector2D origin, float centerAngle, float interval, int count, float speed) {
		std::vector<std::shared_ptr<GameObject>> objs;
		for (int i = 0; i < count; ++i) {
			float angle = centerAngle + interval * (i - (count - 1) / 2.0f);
			auto bullet = PrefabMgr.Instantiate("Bullet_Base")[0];
			bullet->transform->position = origin;
			bullet->GetAppBase<BulletControl>()->SetMove(std::make_shared<FrontSpreadMove>(angle, speed));
			objs.push_back(bullet);
		}
		return objs;
	}

	// 2. 自機狙い
	static void FireAimed(Vector2D origin, float speed) {
		auto player = GetPlayer();
		if (!player) return;
		Vector2D dir = (player->transform->position - origin).GetNormalize();
		float angle = atan2f(dir.y, dir.x) * 180.0f / Mathf::PI;

		auto bullet = PrefabMgr.Instantiate("Bullet_Base")[0];
		bullet->transform->position = origin;
		bullet->GetAppBase<BulletControl>()->SetMove(std::make_shared<FrontSpreadMove>(angle, speed));
	}
	
	// 3. 自機外し（角度を±オフセット）
	static void FireOffsetAimed(Vector2D origin, float offsetAngleDeg, float speed) {
		auto player = GetPlayer();
		if (!player) return;
		Vector2D dir = (player->transform->position - origin).GetNormalize();
		float angle = atan2f(dir.y, dir.x) * 180.0f / Mathf::PI + offsetAngleDeg;

		auto bullet = PrefabMgr.Instantiate("Bullet_Base")[0];
		bullet->transform->position = origin;
		bullet->GetAppBase<BulletControl>()->SetMove(std::make_shared<FrontSpreadMove>(angle, speed));
	}

	// 4. 自機依存（移動量や位置に応じたカスタム弾）
	static void FirePlayerDependent(Vector2D origin, float speed) {
		auto player = GetPlayer();
		if (!player) return;
		float angle = (player->transform->position.x > origin.x) ? 45.0f : 135.0f;
		auto bullet = PrefabMgr.Instantiate("Bullet_Base")[0];
		bullet->transform->position = origin;
		bullet->GetAppBase<BulletControl>()->SetMove(std::make_shared<FrontSpreadMove>(angle, speed));
	}

	// 5. 固定弾
	static void FireFixed(Vector2D origin, float angleDeg, float speed) {
		auto bullet = PrefabMgr.Instantiate("Bullet_Base")[0];
		bullet->transform->position = origin;
		bullet->GetAppBase<BulletControl>()->SetMove(std::make_shared<FrontSpreadMove>(angleDeg, speed));
	}

	// 6. ランダム弾
	static void FireRandom(Vector2D origin) {
		float angle = Random.Range(0.0f, 360.0f);
		float speed = Random.Range(100.0f, 400.0f);
		FireFixed(origin, angle, speed);
	}

	// 7. ばら撒き弾（nWAY + ランダム）
	static void FireSpread(Vector2D origin, int count) {
		for (int i = 0; i < count; ++i) {
			FireRandom(origin);
		}
	}

	// 8. 設置弾（その場に止まり、一定時間後に削除）
	static void FireStationary(Vector2D origin) {
		auto bullet = PrefabMgr.Instantiate("Bullet_Base")[0];
		bullet->transform->position = origin;
		bullet->GetAppBase<BulletControl>()->SetMove(std::make_shared<DelayMove>(2.0f, std::make_shared<StraightMove>(Vector2D(0, 0))));
	}

	// 9. 追尾弾（毎フレームPlayer方向に微調整）
	static void FireHoming(Vector2D origin, float speed) {
		auto player = GetPlayer();
		if (!player) return;
		auto bullet = PrefabMgr.Instantiate("Bullet_Base")[0];
		bullet->transform->position = origin;
		bullet->GetAppBase<BulletControl>()->SetMove(std::make_shared<HomingMove>(player, speed));
	}

	// 10. 撃ち返し弾（敵が倒されたとき用）
	static void FireReflect(Vector2D origin) {
		FireAimed(origin, 250.0f);
	}

	// 11. デフォルト弾 (中心基準で左右対称に count 発撃つ)
	static std::vector<std::shared_ptr<GameObject>> FireShot(Vector2D origin, Vector2D speed, int count = 1, float spacing = 100.0f) {
		int center = count / 2;
		std::vector<std::shared_ptr<GameObject>> objs;
		for (int i = 0; i < count; ++i) {
			auto bullet = PrefabMgr.Instantiate("Bullet_Base")[0];

			// offset を左右対称に
			float offsetX = (i - center) * spacing;

			// 偶数個のときは中心が左右の中間になるため微調整
			if (count % 2 == 0) offsetX += spacing / 2.0f;

			bullet->transform->position = origin + Vector2D(offsetX, 0);
			bullet->GetAppBase<BulletControl>()->SetMove(std::make_shared<StraightMove>(speed));

			objs.push_back(bullet);
		}
		return objs;
	}
};
