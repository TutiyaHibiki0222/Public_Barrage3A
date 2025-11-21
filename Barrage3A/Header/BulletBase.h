#pragma once
#include "../AppBase.h"
#include "../GameObject.h"
#include "../headers.h"
#include "../Debug.hpp"
#include "../GameEngine.h"
#include "../EnemyManager.h"
// ------------------------------------------------------
// ◆ Moveインターフェース
// ------------------------------------------------------
class BulletMove {
public:
	virtual ~BulletMove() = default;
	virtual void Move(std::shared_ptr<Transform2D> _transform) = 0;
};

// ------------------------------------------------------
// ◆ Bullet（見た目と基本移動）
// ------------------------------------------------------
class Bullet : public AppBase {
protected:
	float speed = 0.0f;

public:
	Bullet() : AppBase("Bullet") {}
	Bullet(std::shared_ptr<GameObject> _obj) : AppBase("Bullet", _obj) {}

	void Awake() override {
		//gameObject->AddAppBase<CircleCollider>()->SetRadius(5);
		//Destroy(2); // 2秒後に自動破棄
	}

	void SetVelocity(float _speed) {
		speed = _speed;
	}

	void Update() override {
		// 角度（rotation）から方向ベクトルを算出
		float angleDeg = transform->rotation;
		float angleRad = angleDeg * Mathf::Deg2Rad; // ラジアン変換

		Vector2D direction = Vector2D(std::cos(angleRad), std::sin(angleRad));

		// 移動
		transform->position += direction * speed * Time.deltaTime;

		// 画面外チェック
		Vector2D pos = transform->position;
		Vector2D wid = Window::GetInstance().GetMaxVector2D() / 2;

		if (pos.x < -wid.x || pos.x > wid.x ||
			pos.y < -wid.y || pos.y > wid.y) {
			Destroy();
		}
	}
	// 初めて当たっている瞬間のイベント.
	void OnCollisionEnter(GameObject* other) override {

		Destroy();

	}
protected:
	std::shared_ptr<AppBase> Clone() const override {
		return std::make_shared<Bullet>(*this);
	}
};


// ------------------------------------------------------
// ◆ BulletManager
// ------------------------------------------------------
class BulletControl : public Bullet {
private:
	std::vector<std::shared_ptr<BulletMove>> moves;

public:
	BulletControl() : Bullet() {}
	BulletControl(std::shared_ptr<GameObject> _obj) : Bullet(_obj) {}

	void AddMove(std::shared_ptr<BulletMove> move) {
		moves.push_back(move);
	}

	void SetMove(std::shared_ptr<BulletMove> move) {
		moves.clear();
		moves.push_back(move);
	}

	void Update() override {
		// 通常の移動（直進など）
		Bullet::Update();

		// Moveによる補正（WaveやHomingなど）
		for (auto& move : moves) {
			if (move) move->Move(transform);
		}
	}

protected:
	std::shared_ptr<AppBase> Clone() const override {
		return std::make_shared<BulletControl>(*this);
	}
};

// ------------------------------------------------------//
// ◆ Move実装											 //
// ------------------------------------------------------//
class StraightMove : public BulletMove {
private:
	float spX, spY;
	Vector2D speed;

public:
	StraightMove(float _spX, float _spY) : spX(_spX), spY(_spY) {
		speed = { spX ,spY };
	}
	StraightMove(const Vector2D& vec): speed(vec) {}

	void Move(std::shared_ptr<Transform2D> _tr) override {
		_tr->position += speed * Time.deltaTime;
	}
};

// ------------------------------------------------------
// ◆ 螺旋弾
// ------------------------------------------------------
class FrontSpreadMove : public BulletMove {
private:
	Vector2D direction;
	float speed;
	float angle;

public:
	FrontSpreadMove(float _angleDeg, float _speed)
		: angle(_angleDeg), speed(_speed)
	{
		float angleRad = _angleDeg * (Mathf::PI / 180.0f);
		direction = { cosf(angleRad), sinf(angleRad) };
	}

	void Move(std::shared_ptr<Transform2D> _tr) override {
		_tr->position += direction * speed * Time.deltaTime;
	}
};

// ------------------------------------------------------
// ◆ 遅延弾
// ------------------------------------------------------s
class DelayMove : public BulletMove {
private:
	float waitTime;
	float currentTime = 0;
	std::shared_ptr<BulletMove> next;

public:
	DelayMove(float _wait, std::shared_ptr<BulletMove> _next)
		: waitTime(_wait), next(_next) {}

	void Move(std::shared_ptr<Transform2D> tr) override {
		currentTime += Time.deltaTime;
		if (currentTime > waitTime) {
			next->Move(tr);
		}
	}
};

