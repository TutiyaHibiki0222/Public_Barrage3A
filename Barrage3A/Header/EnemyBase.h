#pragma once
#include "../AppBase.h"
#include "../GameObject.h"
#include"../headers.h"
#include "../Debug.hpp"
class EnemyBase : public AppBase , public std::enable_shared_from_this<EnemyBase>
{
protected:
	bool isShot;
	bool isStete;
	float speed;
	double muki;
	int score;
	int count;
	int  hp;
	int  hpMax;
	Vector2D vel;
protected:
	EnemyBase()
		: AppBase("EnemyBase")
		, isShot(false)
		, isStete(false)
		, score(0)
		, count(0)
		, speed(0)
		, hp(0)
		, hpMax(0)
		, muki(0)
		, vel()
	{};
	EnemyBase(std::shared_ptr<GameObject> _obj) 
		: AppBase("EnemyBase",_obj)
		, isShot(false)
		, isStete(false)
		, score(0)
		, count(0)
		, speed(0)
		, hp(0)
		, hpMax(0)
		, muki(0)
		, vel()
	{};

public:
	void Awake() override;

	void Start() override {
		Init();
	}

	void Update() override {

		Pattern();

		if (!isStete)	Move();

		if (isShot)		Shot();

		Vector2D pos = transform->position;
		if (Mathf::Abs(pos.x) > WIDTH * 0.5f || Mathf::Abs(pos.y) > HEIGHT * 0.5f) {
			Destroy();
		}


		count++;
	}

	void OnDestroy() override;

	void SetSpeed(float _spd) {
		speed = _spd;
	}

	void Damage(int d) {
		hp -= d;
		if (d > 0) {
			Destroy();
		}
	}
protected:
	virtual void Init() = 0;
	virtual void Pattern() = 0;
	virtual void Move() { transform->position += vel * speed * Time.deltaTime; };
	virtual void Shot() = 0;
};



class Enemy01 : public EnemyBase {
public:
	Enemy01() : EnemyBase() {}
	Enemy01(std::shared_ptr<GameObject> _obj) : EnemyBase(_obj) {}

	void Awake()override {
		gameObject->AddAppBase<BoxCollider>()->SetSize({ 10,10 });
	}

	void Init() override {
		//speed = 100;
		//muki  = 90;
		hp = hpMax = 100;
	}

	void Pattern() override {
		
	}

	void Shot() override {

	}
protected:
	std::shared_ptr<AppBase> Clone() const override {
		return std::make_shared<Enemy01>(*this);
	}
};

