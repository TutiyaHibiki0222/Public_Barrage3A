#pragma once

#include "ResourceManager.h"
#include "Transform2D.h"
#include "Animator.h"
#include "AnimatorOverride.h"
#include "Texture2DShortcut.hpp"

using namespace AnimatorDef;

void CreatePlayerAnimator()
{
    const float timeIdle = 1.0f;
    const float timeDirection = 0.06f;
    const float timeLeftRight = 0.5f;


    // プレイヤーのAnimation基盤
    {
        // プレイヤーのAnimation基盤
        auto animator = std::make_shared<Animator>();
        animator->SetSpeed(1.0f);

        // ---- パラメータ定義 ----
        animator->AddParameter("isLeft", AnimatorDef::AnimatorParameterType::Bool);
        animator->AddParameter("isRight", AnimatorDef::AnimatorParameterType::Bool);

        // ---- ステート定義 ----
        AnimatorDef::AnimatorState idleState{ "Idle", nullptr, {},
            []() { std::cout << "[Enter] Idle\n"; },
            []() { std::cout << "[Exit] Idle\n"; } };

        AnimatorDef::AnimatorState moveLeftStart{ "MoveLeftStart", nullptr, {},
            []() { std::cout << "[Enter] MoveLeftStart\n"; },
            []() { std::cout << "[Exit] MoveLeftStart\n"; } };

        AnimatorDef::AnimatorState moveLeft{ "MoveLeft", nullptr, {},
            []() { std::cout << "[Enter] MoveLeft\n"; },
            []() { std::cout << "[Exit] MoveLeft\n"; } };

        AnimatorDef::AnimatorState moveRightStart{ "MoveRightStart", nullptr, {},
            []() { std::cout << "[Enter] MoveRightStart\n"; },
            []() { std::cout << "[Exit] MoveRightStart\n"; } };

        AnimatorDef::AnimatorState moveRight{ "MoveRight", nullptr, {},
            []() { std::cout << "[Enter] MoveRight\n"; },
            []() { std::cout << "[Exit] MoveRight\n"; } };

        // ---- 戻り演出ステート ----
        AnimatorDef::AnimatorState moveLeftEnd{ "MoveLeftEnd", nullptr, {},
            []() { std::cout << "[Enter] MoveLeftEnd\n"; },
            []() { std::cout << "[Exit] MoveLeftEnd\n"; } };

        AnimatorDef::AnimatorState moveRightEnd{ "MoveRightEnd", nullptr, {},
            []() { std::cout << "[Enter] MoveRightEnd\n"; },
            []() { std::cout << "[Exit] MoveRightEnd\n"; } };

        // ---- 遷移定義 ----
        // Idle → 左演出
        AnimatorDef::AnimatorTransition toLeft;
        toLeft.toStateName = "MoveLeftStart";
        toLeft.AddBoolCondition("isLeft", true);

        // Idle → 右演出
        AnimatorDef::AnimatorTransition toRight;
        toRight.toStateName = "MoveRightStart";
        toRight.AddBoolCondition("isRight", true);

        // 左演出 → 左移動（自動）
        AnimatorDef::AnimatorTransition moveLeftStartToLeft;
        moveLeftStartToLeft.toStateName = "MoveLeft";
        moveLeftStartToLeft.hasExitTime = true;

        // 右演出 → 右移動（自動）
        AnimatorDef::AnimatorTransition moveRightStartToRight;
        moveRightStartToRight.toStateName = "MoveRight";
        moveRightStartToRight.hasExitTime = true;

        // 左移動 → 左演出終了（キー離し）
        AnimatorDef::AnimatorTransition leftToEnd;
        leftToEnd.toStateName = "MoveLeftEnd";
        leftToEnd.AddBoolCondition("isLeft", false);

        // 右移動 → 右演出終了（キー離し）
        AnimatorDef::AnimatorTransition rightToEnd;
        rightToEnd.toStateName = "MoveRightEnd";
        rightToEnd.AddBoolCondition("isRight", false);

        // 左演出終了 → Idle（自動）
        AnimatorDef::AnimatorTransition leftEndToIdle;
        leftEndToIdle.toStateName = "Idle";
        leftEndToIdle.hasExitTime = true;

        // 右演出終了 → Idle（自動）
        AnimatorDef::AnimatorTransition rightEndToIdle;
        rightEndToIdle.toStateName = "Idle";
        rightEndToIdle.hasExitTime = true;


        // ---- 遷移追加 ----
        idleState.transitions.push_back(toLeft);
        idleState.transitions.push_back(toRight);

        moveLeftStart.transitions.push_back(moveLeftStartToLeft);
        moveRightStart.transitions.push_back(moveRightStartToRight);

        moveLeft.transitions.push_back(leftToEnd);
        moveRight.transitions.push_back(rightToEnd);

        moveLeftEnd.transitions.push_back(leftEndToIdle);
        moveRightEnd.transitions.push_back(rightEndToIdle);

        // ---- ステート登録 ----
        animator->AddState(idleState);
        animator->AddState(moveLeftStart);
        animator->AddState(moveLeft);
        animator->AddState(moveRightStart);
        animator->AddState(moveRight);
        animator->AddState(moveLeftEnd);
        animator->AddState(moveRightEnd);

        // ---- リソース登録 ----
        Resource::Set("PlayerAnimator", animator);
    }

    // アニメーションデータプリセット(プレイヤー1 霊夢)
    {
        auto overrideClips = std::make_shared<AnimatorOverride>();

        auto p = Texture2DManager::GetInstance().GetTexture2D("Player", "霊夢");
        // 通常.
        auto idleClip = std::make_shared<AnimationClip>();
        idleClip->duration = timeIdle;
        idleClip->frames = (p->GetTexturesByPrefixRange("player01", 0, 7)); 
        idleClip->loop = true;
        overrideClips->SetClip("Idle", idleClip);

        // ←演出
        auto moveLeftStartClip = std::make_shared<AnimationClip>();
        moveLeftStartClip->duration = timeDirection;
        moveLeftStartClip->frames = (p->GetTexturesByPrefixRange("player01", 9, 11));
        moveLeftStartClip->loop = false;
        overrideClips->SetClip("MoveLeftStart", moveLeftStartClip);
        // ←移動
        auto moveLeftClip = std::make_shared<AnimationClip>();
        moveLeftClip->duration = timeLeftRight;
        moveLeftClip->frames = (p->GetTexturesByPrefixRange("player01", 12, 15));
        moveLeftClip->loop = true;
        overrideClips->SetClip("MoveLeft", moveLeftClip);
        // ←終了
        auto moveLeftEndClip = std::make_shared<AnimationClip>();
        moveLeftEndClip->duration = timeDirection;
        moveLeftEndClip->frames = (p->GetTexturesByPrefixRange("player01", 11, 9));
        moveLeftEndClip->loop = false;
        overrideClips->SetClip("MoveLeftEnd", moveLeftEndClip);

        // →演出
        auto moveRightStartClip = std::make_shared<AnimationClip>();
        moveRightStartClip->duration = timeDirection;
        moveRightStartClip->frames = (p->GetTexturesByPrefixRange("player01", 17, 19));
        moveRightStartClip->loop = false;
        overrideClips->SetClip("MoveRightStart", moveRightStartClip);
        // →演出
        auto moveRightClip = std::make_shared<AnimationClip>();
        moveRightClip->duration = timeLeftRight;
        moveRightClip->frames = (p->GetTexturesByPrefixRange("player01", 20, 23));
        moveRightClip->loop = true;
        overrideClips->SetClip("MoveRight", moveRightClip);
        // →終了
        auto moveRightEndClip = std::make_shared<AnimationClip>();
        moveRightEndClip->duration = timeDirection;
        moveRightEndClip->frames = (p->GetTexturesByPrefixRange("player01", 19, 17));
        moveRightEndClip->loop = false;
        overrideClips->SetClip("MoveRightEnd", moveRightEndClip);

        Resource::Set("ReimuAnimationData", overrideClips);
    }

    // アニメーションデータプリセット(プレイヤー2 魔理沙)
    {
        auto overrideClips = std::make_shared<AnimatorOverride>();

        auto p = Texture2DManager::GetInstance().GetTexture2D("Player", "魔理沙");

        // 通常.
        auto idleClip = std::make_shared<AnimationClip>();
        idleClip->duration = timeIdle;
        idleClip->frames = (p->GetTexturesByPrefixRange("player02", 0, 7));
        idleClip->loop = true;
        overrideClips->SetClip("Idle", idleClip);

        // ←演出
        auto moveLeftStartClip = std::make_shared<AnimationClip>();
        moveLeftStartClip->duration = timeDirection;
        moveLeftStartClip->frames = (p->GetTexturesByPrefixRange("player02", 9, 11));
        moveLeftStartClip->loop = false;
        overrideClips->SetClip("MoveLeftStart", moveLeftStartClip);
        // ←移動
        auto moveLeftClip = std::make_shared<AnimationClip>();
        moveLeftClip->duration = timeLeftRight;
        moveLeftClip->frames = (p->GetTexturesByPrefixRange("player02", 12, 15));
        moveLeftClip->loop = true;
        overrideClips->SetClip("MoveLeft", moveLeftClip);
        // ←終了
        auto moveLeftEndClip = std::make_shared<AnimationClip>();
        moveLeftEndClip->duration = timeDirection;
        moveLeftEndClip->frames = (p->GetTexturesByPrefixRange("player02", 11, 9));
        moveLeftEndClip->loop = false;
        overrideClips->SetClip("MoveLeftEnd", moveLeftEndClip);

        // →演出
        auto moveRightStartClip = std::make_shared<AnimationClip>();
        moveRightStartClip->duration = timeDirection;
        moveRightStartClip->frames = (p->GetTexturesByPrefixRange("player02", 17, 19));
        moveRightStartClip->loop = false;
        overrideClips->SetClip("MoveRightStart", moveRightStartClip);
        // →演出
        auto moveRightClip = std::make_shared<AnimationClip>();
        moveRightClip->duration = timeLeftRight;
        moveRightClip->frames = (p->GetTexturesByPrefixRange("player02", 20, 23));
        moveRightClip->loop = true;
        overrideClips->SetClip("MoveRight", moveRightClip);
        // →終了
        auto moveRightEndClip = std::make_shared<AnimationClip>();
        moveRightEndClip->duration = timeDirection;
        moveRightEndClip->frames = (p->GetTexturesByPrefixRange("player02", 19, 17));
        moveRightEndClip->loop = false;
        overrideClips->SetClip("MoveRightEnd", moveRightEndClip);

        Resource::Set("MarisaAnimationData", overrideClips);
    }
}

void CreateBulletAnimator()
{
	float timeNormal = 1.0f / 60.0f;
	float timeHit = 15.0f / 60.0f;
    // 弾Animator
    {
        auto animator = std::make_shared<Animator>();
        animator->SetSpeed(1.0f);

        // ---- パラメータ定義 ----
        // ヒットしたかどうかで切り替え
        animator->AddParameter("isHit", AnimatorDef::AnimatorParameterType::Bool);

        // ---- ステート定義 ----
        AnimatorDef::AnimatorState normalState{
            "Normal", nullptr, {},
            []() { std::cout << "[Enter] Normal\n"; },
            []() { std::cout << "[Exit] Normal\n"; }
        };

        AnimatorDef::AnimatorState hitState{
            "Hit", nullptr, {},
            []() { std::cout << "[Enter] Hit\n"; },
            []() { std::cout << "[Exit] Hit\n"; }
        };

        // ---- 遷移定義 ----
        AnimatorDef::AnimatorTransition normalToHit;
        normalToHit.toStateName = "Hit";
        normalToHit.AddBoolCondition("isHit", true);

        AnimatorDef::PropertyTween fadeOut;
        fadeOut.type = AnimatorDef::PropertyTween::Type::Alpha;
        fadeOut.startValue = 1.0f;
        fadeOut.endValue = 0.0f;
        fadeOut.duration = timeHit * 1.75f;
        fadeOut.onUpdate = [](float t, std::shared_ptr<SpriteRenderer> sp) {
            if (sp) sp->GetColor().a = t;
            };
		hitState.AddTween(fadeOut);

        // ---- 遷移追加 ----
        normalState.transitions.push_back(normalToHit);

        // ---- ステート登録 ----
        animator->AddState(normalState);
        animator->AddState(hitState);

        // ---- リソース登録 ----
        Resource::Set("b00_Animator", animator);
    }

    // アニメーションデータプリセット
    {
        auto overrideClips = std::make_shared<AnimatorOverride>();
        auto p = Tex("Player/Bullet/霊夢");
        if(p == nullptr)
        {
            std::cout << "Bullet Base Texture not found!" << std::endl;
            return;
        }
        // 通常弾
        auto normalClip = std::make_shared<AnimationClip>();
        normalClip->duration = timeNormal;
        normalClip->frames = p->GetTexturesByPrefixRange("bl_00", 0, 1);
        normalClip->loop = true;
        overrideClips->SetClip("Normal", normalClip);

        // ヒット弾
        auto hitClip = std::make_shared<AnimationClip>();
        hitClip->duration = timeHit * 1.3f;
        hitClip->frames = p->GetTexturesByPrefixRange("bl_00_bullet_hit", 1, 4);
        hitClip->loop = false;

        overrideClips->SetClip("Hit", hitClip);

        Resource::Set("b00_AnimationData", overrideClips);
    }

    // 弾Animator
    {
        auto animator = std::make_shared<Animator>();
        animator->SetSpeed(1.0f);

        // ---- パラメータ定義 ----
        animator->AddParameter("isHit", AnimatorDef::AnimatorParameterType::Bool);

        // ---- ステート定義 ----
        AnimatorDef::AnimatorState normalState{
            "Normal", nullptr, {},
            []() { std::cout << "[Enter] Normal\n"; },
            []() { std::cout << "[Exit] Normal\n"; }
        };

        AnimatorDef::AnimatorState hitState{
            "Hit", nullptr, {},
            []() { std::cout << "[Enter] Hit\n"; },
            []() { std::cout << "[Exit] Hit\n"; }
        };

        // ---- 遷移定義 ----
        AnimatorDef::AnimatorTransition normalToHit;
        normalToHit.toStateName = "Hit";
        normalToHit.AddBoolCondition("isHit", true);

        // ---- ヒット用Tween ----
        // 透明度
        AnimatorDef::PropertyTween fadeOut;
        fadeOut.type = AnimatorDef::PropertyTween::Type::Alpha;
        fadeOut.startValue = 0.8f;
        fadeOut.endValue = 0.0f;
        fadeOut.duration = timeHit * 1.5f;
        fadeOut.onUpdate = [](float t, std::shared_ptr<SpriteRenderer> sp) {
            if (sp) sp->GetColor().a = t;
            };
        hitState.AddTween(fadeOut);

        // スケール（大きく→小さく）
        AnimatorDef::PropertyTween scaleTween;
        scaleTween.type = AnimatorDef::PropertyTween::Type::Scale;
        scaleTween.startValue = 1.0f;     // 通常
        scaleTween.endValue = 0.0f;     // 小さく
        scaleTween.duration = timeHit * 1.5f;
        const float peakScale = 3.0f;
        // 途中で最大サイズ（例：1.5）に到達させる
        scaleTween.onUpdate = [=](float t, std::shared_ptr<SpriteRenderer> sp) {
            if (!sp) return;

            float s = 1.0f;

            if (t < 0.5f) {
                // 前半：1.0 → 2.0
                float k = t / 0.5f;
                s = Mathf::Lerp(1.0f, peakScale, k);
            }
            else {
                // 後半：1.5 → 0.0f
                float k = (t - 0.5f) / 0.5f;
                s = Mathf::Lerp(peakScale, 0.0f, k);
            }

            sp->GetGameObject()->transform->scale = { s, s };
            };

        hitState.AddTween(scaleTween);

        // 回転（ランダム方向）
        AnimatorDef::PropertyTween rotTween;
        rotTween.type = AnimatorDef::PropertyTween::Type::Rotation;
        rotTween.startValue = 0.0f;
        rotTween.duration = timeHit * 1.5f;
        rotTween.onUpdate = [](float t, std::shared_ptr<SpriteRenderer> sp) {
            if (sp) sp->SetRotation(t);
            };
        hitState.AddTween(rotTween);

        // ---- 遷移追加 ----
        normalState.transitions.push_back(normalToHit);

        // ---- ステート登録 ----
        animator->AddState(normalState);
        animator->AddState(hitState);

        // ---- リソース登録 ----
        Resource::Set("b01_Animator", animator);
    }

    // アニメーションデータプリセット
    {
        auto overrideClips = std::make_shared<AnimatorOverride>();
        auto p = Tex("Player/Bullet/霊夢");
        if (!p) {
            std::cout << "Bullet Base Texture not found!" << std::endl;
            return;
        }

        // 通常弾
        auto normalClip = std::make_shared<AnimationClip>();
        normalClip->duration = timeNormal;
        normalClip->frames = p->GetTexturesByPrefixRange("bl", 0, 0); // 1フレーム
        normalClip->loop = true;
        overrideClips->SetClip("Normal", normalClip);

        // ヒット弾（bl_0~bl_4）
        auto hitClip = std::make_shared<AnimationClip>();
        hitClip->duration = timeHit;
        hitClip->frames = p->GetTexturesByPrefixRange("bl", 0, 4);
        hitClip->loop = false;
        overrideClips->SetClip("Hit", hitClip);

        Resource::Set("b01_AnimationData", overrideClips);
    }

    {
        auto animator = std::make_shared<Animator>();
        animator->SetSpeed(1.0f);

        // ---- パラメータ定義 ----
        // ヒットしたかどうかで切り替え
        animator->AddParameter("isHit", AnimatorDef::AnimatorParameterType::Bool);

        // ---- ステート定義 ----
        AnimatorDef::AnimatorState normalState{
            "Normal", nullptr, {},
            []() { std::cout << "[Enter] Normal\n"; },
            []() { std::cout << "[Exit] Normal\n"; }
        };

        AnimatorDef::AnimatorState hitState{
            "Hit", nullptr, {},
            []() { std::cout << "[Enter] Hit\n"; },
            []() { std::cout << "[Exit] Hit\n"; }
        };

        // ---- 遷移定義 ----
        AnimatorDef::AnimatorTransition normalToHit;
        normalToHit.toStateName = "Hit";
        normalToHit.AddBoolCondition("isHit", true);

        AnimatorDef::PropertyTween fadeOut;
        fadeOut.type = AnimatorDef::PropertyTween::Type::Alpha;
        fadeOut.startValue = 0.6f;
        fadeOut.endValue = 0.0f;
        fadeOut.duration = timeHit * 1.75f;
        fadeOut.onUpdate = [](float t, std::shared_ptr<SpriteRenderer> sp) {
            if (sp) sp->GetColor().a = t;
            };
        hitState.AddTween(fadeOut);

        AnimatorDef::PropertyTween rotTween;
        rotTween.type = AnimatorDef::PropertyTween::Type::Rotation;
        rotTween.startValue = 0.0f;
        rotTween.duration = timeHit * 1.5f;
        rotTween.onUpdate = [](float t, std::shared_ptr<SpriteRenderer> sp) {
            if (sp) sp->SetRotation(t);
        };
        hitState.AddTween(rotTween);

        // ---- 遷移追加 ----
        normalState.transitions.push_back(normalToHit);

        // ---- ステート登録 ----
        animator->AddState(normalState);
        animator->AddState(hitState);

        // ---- リソース登録 ----
        Resource::Set("b02_Animator", animator);
    }

}
