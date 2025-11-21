#pragma once
#include "Header/BulletPatternFactory.h"
#include "Header/EnemyBase.h"

class Enemy03 : public EnemyBase {
private:
	int shotInterval = 60;    // 60ƒtƒŒ[ƒ€‚²‚Æ‚ÉŒ‚‚Âi1•bŠÔŠuj
	Vector2D origin;          // ‰ŠúˆÊ’u
	float waveAmplitude = 30; // ¶‰E‚Ì—h‚ê•

public:
	Enemy03() : EnemyBase() {}
	Enemy03(std::shared_ptr<GameObject> _obj) : EnemyBase(_obj) {}

	void Init() override {
		speed = 100;
		vel = Vector2D(0, -1);  // ‰º•ûŒü‚ÉˆÚ“®
		hp = 10;
		hpMax = 10;
		isShot = true;
		origin = transform->position;
	}

	void Pattern() override {
		if (count % shotInterval == 0) {
			Shot();
		}
		// Šî–{‚Ì‰º•ûŒüˆÚ“®{¶‰E‚É—h‚ê‚é
		float offsetX = sinf(Time.time * 2.0f) * waveAmplitude;
		transform->position = origin + Vector2D(offsetX, -count * speed * Time.deltaTime);
	}

	void Shot() override {
		Vector2D firePos = transform->position + Vector2D(0, -10);
		BulletPatternFactory::FireAimed(firePos, 350.0f);
	}

protected:
	std::shared_ptr<AppBase> Clone() const override {
		return std::make_shared<Enemy03>(*this);
	}
};
