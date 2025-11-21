#pragma once
#include "Header/BulletPatternFactory.h"
#include "Header/EnemyBase.h"

class Enemy04 : public EnemyBase {
private:
	enum class State {
		ENTER,   // ‰æ–Ê‚É~‚è‚Ä‚­‚é
		ATTACK,  // UŒ‚’†
		EXIT     // ‘Şêiã¸j
	};

	State state = State::ENTER;
	Vector2D targetPos;
	int shotTimer = 0;
	int shotCount = 0;
	const int maxShots = 3;
	const float enterSpeed = 150.0f;
	const float exitSpeed = 200.0f;

public:
	Enemy04() : EnemyBase() {}
	Enemy04(std::shared_ptr<GameObject> _obj) : EnemyBase(_obj) {}

	void Init() override {
		hp = 999;           // “|‚³‚ê‚¸‚É©“®‘Şê‚·‚é“G‚È‚Ì‚Å‘å‚«‚ß
		hpMax = 999;
		isShot = true;
		speed = 0;
		targetPos = Vector2D(transform->position.x, 200); // ~‰ºæYÀ•W
	}

	void Pattern() override {

		switch (state) 
		{
		case State::ENTER:
			vel = Vector2D(0, 1); // ‰º‚ÉˆÚ“®
			if (transform->position.y >= targetPos.y) {
				transform->position.y = targetPos.y;
				vel = Vector2D(0, 0); // ’â~
				state = State::ATTACK;
			}
			break;

		case State::ATTACK:
			isShot = true;
			break;

		case State::EXIT:
			vel = Vector2D(0, -1); // ã‚ÉˆÚ“®
			if (transform->position.y < -HEIGHT / 2 - 50) {
				Destroy(gameObject);
			}
			break;
		}
	}

	void Shot() override {
		shotTimer++;
		if (shotTimer >= 40) {
			shotTimer = 0;
			BulletPatternFactory::FireAimed(transform->position, 250.0f);
			shotCount++;
			if (shotCount >= maxShots) {
				state = State::EXIT;
			}
		}
		isShot = false;
	}

protected:
	std::shared_ptr<AppBase> Clone() const override {
		return std::make_shared<Enemy04>(*this);
	}
};
