/*
    ◆ ReimuHakurei.cpp

    クラス名        : ReimuHakurei クラス
    作成日          : 2025/11/14 (金) 23:00:52
    最終変更日      : 
    作成者          : 
    概要            : クラスの概要をここに記述してください
*/
#include "ReimuHakurei.h"
#include "GameObject.h"
#include "GameObjectMgr.h"
#include "PlayerBullet.h"
#include "MusicController.h"
#include "AudioResourceShortcut.hpp"
#include "EnemyHoming.h"
#include "ResourceManager.h"
#include "AnimatorOverride.h"
// コンストラクタ 
ReimuHakurei::ReimuHakurei() {

}

ReimuHakurei::ReimuHakurei(std::shared_ptr<GameObject> owner) {

}

// AppBase Event.
void ReimuHakurei::Initialize() {
    magatama->Init(4, Sprite::MakeLoadImage("Resources/Images/Player/dot_bool_red2.png"), transform->position);
}

void ReimuHakurei::Shoot() {
	if (auto t = Tex["Player/Bullet/霊夢:bl_00_0"])
	{
		int center = 2 / 2;
		for (int i = 0; i < 2; ++i) {
			auto bullet = GameObject::Instantiate("PlayerBullet");
			bullet->transform->rotation = 90;
			bullet->SetTag("PlayerBullet");
			bullet->SetLayer(Layer::PlayerBullet);
			auto b = bullet->AddAppBase<PlayerBullet>();
			b->SetSpeed(14);
			b->SetDamage(4);
			// offset を左右対称に
			float offsetX = (i - center) * 20.0f;

			// 偶数個のときは中心が左右の中間になるため微調整
			if (2 % 2 == 0) offsetX += 20.0f / 2.0f;
			bullet->transform->position = transform->position + Vector2D(offsetX, 15);
			auto sp = bullet->AddAppBase<SpriteRenderer>();
			sp->SetSprite(t);
			sp->SetLayer(-2);
			sp->GetColor().a = 0.6f;

			bullet->AddAppBase<BoxCollider>()->SetSize({ (float)t->width,(float)t->height });

			// Animator挿入.
			auto a = Resource::GetPtrClone<Animator>("b00_Animator");
			if (a) {
				bullet->AddAppBase(a);
				auto d = Resource::GetPtrClone<AnimatorDef::AnimatorOverride>("b00_AnimationData");
				if (d) {
					a->SetOverrideController(d);
				}
			}
		}

		auto ms = MusicController::GetInstance();
		if (ms)
		{
			auto se = ms->OneShotAudio(Sounds["player_shot"]->Clone());
			if (se) {
				float playAreaLeft = -WIDTH / 2.0f + 64.0f;
				float playAreaRight = WIDTH / 2.0f - 350.0f;
				float playAreaCenter = (playAreaLeft + playAreaRight) / 2.0f;
				float playAreaWidth = playAreaRight - playAreaLeft;
				float relativeX = transform->position.x - playAreaCenter;
				float pan = relativeX / (playAreaWidth / 2.0f);
				se->SetStereoPan(pan);
			}
		}

	}
	shotCount = 4;
}

void ReimuHakurei::OptionShot(const bool isFocused, const std::vector<Vector2D>& poss) {
	if (isFocused) {
		for (const auto& pos : poss) {
			if (auto t = Tex["Player/Bullet/霊夢:bl_02"]) {
				int center = 2 / 2;
				for (int i = 0; i < 2; ++i) {
					auto bullet = GameObject::Instantiate("PlayerBullet");
					bullet->transform->rotation = 90;
					bullet->SetTag("PlayerBullet");
					bullet->SetLayer(Layer::PlayerBullet);
					auto b = bullet->AddAppBase<PlayerBullet>();
					b->SetSpeed(14);
					b->SetDamage(4);
					// offset を左右対称に
					float offsetX = (i - center) * 7.5f;

					// 偶数個のときは中心が左右の中間になるため微調整
					if (2 % 2 == 0) offsetX += 7.5f / 2.0f;
					bullet->transform->position = pos + Vector2D(offsetX, 0);
					auto sp = bullet->AddAppBase<SpriteRenderer>();
					sp->SetSprite(t);
					sp->SetLayer(-2);
					sp->GetSortingLayer().orderInLayer = 10;
					sp->GetColor().a = 0.6f;

					bullet->AddAppBase<BoxCollider>()->SetSize({ (float)t->width,(float)t->height });
				
					auto a = Resource::GetPtrClone<Animator>("b02_Animator");
					if (a) {
						bullet->AddAppBase(a);

						// 弾の SpriteRenderer
						float currentRot = 0.0f;
						currentRot = transform->rotation;  // 現在の角度取得

						// ランダム方向（+1 or -1）
						float dir = (Random.Range(0, 1) == 0 ? 1.0f : -1.0f);

						// ★ 最大 ±75° 回転
						float deltaRot = 10.0f * dir;
						float targetRot = currentRot + deltaRot;

						// Hit ステートの回転Tweenを書き換え
						if (auto hitState = a->GetStatePtr("Hit")) {
							for (auto& t : hitState->tweens) {
								if (t.type == AnimatorDef::PropertyTween::Type::Rotation) {
									t.startValue = targetRot;   // 現在角度から
									t.endValue = targetRot;      // ±60°
								}
							}
						}
					}
				}
			}
		}
		mgtmCount = 4;
	}
	else {
		int count = (int)poss.size();
		if (count == 0) return;

		const float baseAngle = 90.0f;
		const float maxSpread = 45.0f;
		float spacing = 35.0f;

		// countが1なら間隔は0、それ以外は間隔を調整
		if (count > 1) {
			float totalSpread = spacing * (count - 1);
			if (totalSpread > maxSpread) {
				spacing = maxSpread / (count - 1);
			}
		}

		float startAngle = baseAngle - spacing * (count - 1) * 0.5f;

		for (int i = 0; i < count; ++i) {
			auto pos = poss[i];
			float angle = startAngle + i * spacing;

			if (auto t = Tex["Player/Bullet/霊夢:bl_0"]) {
				auto bulletObj = GameObject::Instantiate("bulelt");
				bulletObj->transform->position = pos;
				bulletObj->transform->rotation = angle;

				bulletObj->SetLayer(Layer::PlayerBullet);
				bulletObj->SetTag("PlayerBullet");

				auto b = bulletObj->AddAppBase<PlayerBullet>();
				b->SetSpeed(6);
				b->SetDamage(2);
				b->SetBulletController<EnemyHoming>(b->GetSpeed());
				b->onHitLambda = [](PlayerBullet* b) {
					b->SetSpeed(b->GetSpeed() * 0.1f);
					if (b->animator) {
						b->animator->SetBool("isHit", true);
						if (auto s = b->animator->GetOwnerRenderer()) {
							s->SetLayer(6);
							s->GetColor().a = 1.0f;
						}
						b->Destroy(0.5f);
					}
					else {
						b->Destroy();
					}
				};
				bulletObj->AddAppBase<CircleCollider>()->SetRadius((float)(t->width / 2));

				auto sp = bulletObj->AddAppBase<SpriteRenderer>();
				sp->SetSprite(t);
				sp->SetLayer(-2);
				sp->GetColor().a = 0.6f;
				sp->SetAfterImageEnabled(true);
				sp->SetAfterImageSettings(0.01f, 0.1f, 10);			

				auto a = Resource::GetPtrClone<Animator>("b01_Animator");
				if (a) {
					bulletObj->AddAppBase(a);

					auto d = Resource::GetPtrClone<AnimatorDef::AnimatorOverride>("b01_AnimationData");
					if (d) {
						a->SetOverrideController(d);
					}

					// 弾の SpriteRenderer
					float currentRot = 0.0f;
					currentRot = transform->rotation;  // 現在の角度取得

					// ランダム方向（+1 or -1）
					float dir = (Random.Range(0, 1) == 0 ? 1.0f : -1.0f);

					// ★ 最大 ±75° 回転
					float deltaRot = 75.0f * dir;
					float targetRot = currentRot + deltaRot;

					// Hit ステートの回転Tweenを書き換え
					if (auto hitState = a->GetStatePtr("Hit")) {
						for (auto& t : hitState->tweens) {
							if (t.type == AnimatorDef::PropertyTween::Type::Rotation) {
								t.startValue = currentRot;   // 現在角度から
								t.endValue = targetRot;      // ±60°
							}
						}
					}
				}
			}
		}
		mgtmCount = 6;
	}

}

void ReimuHakurei::Bomb() {

}

void ReimuHakurei::OnHit() {

}