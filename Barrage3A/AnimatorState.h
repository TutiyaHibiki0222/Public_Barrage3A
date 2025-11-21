#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include "AnimationClip.h"
#include "AnimatorCondition.h"
#include "SpriteRenderer.h"

namespace AnimatorDef {

    struct PropertyTween {
        enum class Type { Alpha, Scale, Rotation, Position };
        Type type;
        float startValue;
        float endValue;
        float duration;
        std::function<void(float,std::shared_ptr<SpriteRenderer>)> onUpdate; // Tween中の値を反映する
    };

    // アニメーターの状態を表現する構造体.
    struct AnimatorState {
        std::string name;                           // 状態の名前。例: "Idle", "Run", "Jump" など。
        std::shared_ptr<AnimationClip> clip;        // アニメーションクリップ。この状態で使用されるアニメーションを保持。
        std::vector<AnimatorTransition> transitions;// この状態に遷移するための条件と遷移先を記述した遷移リスト。
        
        // 状態に入ったときに呼び出される関数。初期化処理などを行うために使用。
        std::function<void()> onEnter = nullptr;
        // 状態から出るときに呼び出される関数。終了処理などを行うために使用。
        std::function<void()> onExit = nullptr;

        std::vector<PropertyTween> tweens;

        void SetOnEnter(std::function<void()> enterFunc) {
            onEnter = enterFunc;
        }

        // 状態から出るときに呼ばれる関数を設定
        void SetOnExit(std::function<void()> exitFunc) {
            onExit = exitFunc;
        }

        // 遷移を追加するメソッド
        void AddTransition(const AnimatorTransition& transition) {
            transitions.push_back(transition);
        }

        // 遷移を追加するメソッド (条件を指定して遷移を作成)
        void AddTransition(const std::string& toStateName, const std::string& conditionName, bool conditionValue) {
            AnimatorTransition transition;
            transition.toStateName = toStateName;
            transition.AddBoolCondition(conditionName, conditionValue);
            transitions.push_back(transition);
        }

        void AddTransition(const std::string& toStateName, const std::string& conditionName, float expectedValue, float epsilon = 0.01f) {
            AnimatorTransition transition;
            transition.toStateName = toStateName;
            transition.AddFloatCondition(conditionName, expectedValue, epsilon);
            transitions.push_back(transition);
        }

        void AddTween(const PropertyTween& t) { tweens.push_back(t); }
    };
}