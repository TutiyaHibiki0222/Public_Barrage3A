/*
	Animator class
*/
#include "Animator.h"
#include "GameObject.h"

// コンストラクタ 
Animator::Animator() : AppBase("Animator"), isUpdateTimeScale(true){

}
Animator::Animator(std::shared_ptr<GameObject> owner) : AppBase("Animator", owner), isUpdateTimeScale(true) {

}

void Animator::AddState(const AnimatorDef::AnimatorState& state) {
    states[state.name] = state;
    if (currentStateName.empty()) {
        currentStateName = state.name;
        defaultStateName = state.name;
        if (state.onEnter) state.onEnter();
    }
}
void Animator::AddParameter(const std::string& name, AnimatorDef::AnimatorParameterType type) {
    AnimatorDef::AnimatorParameter param;
    param.type = type;
    if      (type == AnimatorDef::AnimatorParameterType::Bool ) param.boolValue  = false;
    else if (type == AnimatorDef::AnimatorParameterType::Float) param.floatValue = 0.0f;
    parameters[name] = param;
}

// --- パラメータ設定 ---
void Animator::SetTrigger(const std::string& name) {
    auto it = parameters.find(name);
    if (it != parameters.end() && it->second.type == AnimatorDef::AnimatorParameterType::Trigger) {
        it->second.triggered = true;
    }
}

void Animator::SetBool(const std::string& name, bool value) {
    auto it = parameters.find(name);
    if (it != parameters.end() && it->second.type == AnimatorDef::AnimatorParameterType::Bool) {
        it->second.boolValue = value;
    }
}

void Animator::SetFloat(const std::string& name, float value) {
    auto it = parameters.find(name);
    if (it != parameters.end() && it->second.type == AnimatorDef::AnimatorParameterType::Float) {
        it->second.floatValue = value;
    }
}

void Animator::SetInt(const std::string& name, int value) {
    auto it = parameters.find(name);
    if (it != parameters.end() && it->second.type == AnimatorDef::AnimatorParameterType::Int) {
        it->second.intValue = value;
    }
}

void Animator::SetSpeed(float s) {
    speed = s;
}

void Animator::SetIsUpdateTimeScale(bool is) {
    isUpdateTimeScale = is;
}

float Animator::GetSpeed() const {
    return speed;
}

void Animator::SetOverrideController(std::shared_ptr<AnimatorDef::AnimatorOverride> overrideController) {
    if (!overrideController) return;

    for (auto& [type,state] : states) {
        auto newClip = overrideController->GetClip(state.name);
        if (newClip) {
            state.clip = newClip; // 名前一致で Clip を置き換え
        }
    }
}

void Animator::Play(const std::string& stateName) {
    if (states.count(stateName)) {
        auto& current = states[currentStateName];
        if (current.onExit) current.onExit(); // ← 前ステートの終了処理

        currentStateName = stateName;
        time = 0.0f;
        isPlaying = true;

        auto& next = states[stateName];
        if (next.onEnter) next.onEnter(); // ← 新ステートの開始処理
    }
}

void Animator::Stop() {
    isPlaying = false;
    time = 0.0f;
}

void Animator::Pause() {
    isPaused = true;
}

void Animator::Resume() {
    isPaused = false;
}

void Animator::Reset() {
    if (!defaultStateName.empty()) {
        Play(defaultStateName);
    }
}

float Animator::GetTime() const {
    return time;
}

void Animator::SetTime(float t) {
    time = t;
}

bool Animator::IsPlaying() const {
    return isPlaying && !isPaused;
}

const std::string& Animator::GetCurrentStateName() const {
    return currentStateName;
}

// AppBase Event.
void Animator::Start() {
    ownerRenderer = gameObject->GetAppBase<SpriteRenderer>();
}


void Animator::Update() {
    if (states.empty() || !isPlaying) return;

    float delta = isUpdateTimeScale ? Time.deltaTime * speed : Time.unscaledDeltaTime * speed;
    time += delta;

    auto it = states.find(currentStateName);
    if (it == states.end()) return;

    AnimatorDef::AnimatorState& current = it->second;

    // 1. 遷移判定
    for (const AnimatorDef::AnimatorTransition& trans : current.transitions) {
        if (trans.hasExitTime && current.clip && time < current.clip->duration) continue;

        bool passed = true;
        for (const AnimatorDef::AnimatorCondition& cond : trans.conditions) {
            auto p = parameters.find(cond.parameterName);
            if (p == parameters.end() || !cond.condition(p->second)) {
                passed = false;
                break;
            }
        }

        if (passed) {
            if (current.onExit) current.onExit();
            currentStateName = trans.toStateName;
            time = 0.0f;

            auto& nextState = states[currentStateName];
            if (nextState.onEnter) nextState.onEnter();

            // Triggerリセット
            for (auto& [k, v] : parameters)
                if (v.type == AnimatorDef::AnimatorParameterType::Trigger) v.triggered = false;

            break;
        }
    }

    // 2. フレーム更新
    if (current.clip) {
        auto frame = current.clip->GetFrameAtTime(time);
        if (frame && gameObject) {
            auto re = ownerRenderer.lock();
            if (!re) {
                re = gameObject->GetAppBase<SpriteRenderer>();
                ownerRenderer = re;
            }
            if (re) re->SetSprite(frame);
        }
    }

    // 3. Tween更新（透明化や拡縮など）
    for (auto& t : current.tweens) {
        float progress = time / t.duration;
        if (progress > 1.0f) progress = 1.0f;
        float value = t.startValue + (t.endValue - t.startValue) * progress;
        if (t.onUpdate) t.onUpdate(value, ownerRenderer.lock());
    }
}
