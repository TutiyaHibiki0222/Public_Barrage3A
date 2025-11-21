#pragma once
#include "headers.h"
#include "AppBase.h"
#include "AnimatorState.h"
#include "AnimatorParameter.h"
#include "AnimatorOverride.h"
#include "SpriteRenderer.h"

// Animator class and AppBase class
class Animator : public AppBase {
private:    // 非公開.
    std::unordered_map<std::string, AnimatorDef::AnimatorState> states;
    std::unordered_map<std::string, AnimatorDef::AnimatorParameter> parameters;
    std::weak_ptr<SpriteRenderer> ownerRenderer;
    std::string currentStateName;
    float time  = 0.0f;
    float speed = 1.0f;

    bool isUpdateTimeScale;       // Scaleに影響受けるか.
    bool isPlaying = true;        // Update() の再生可否.
    bool isPaused = false;        // Pause状態.
    std::string defaultStateName; // Reset() で戻る初期ステート.
public:     // 公開.

    // コンストラクタ.
    Animator();
    Animator(std::shared_ptr<GameObject>);

    void AddState(const AnimatorDef::AnimatorState& state);
    void AddParameter(const std::string& name, AnimatorDef::AnimatorParameterType type);

    void SetTrigger(const std::string& name);
    void SetBool(const std::string& name, bool value);
    void SetFloat(const std::string& name, float value);
    void SetInt(const std::string& name, int value);
    void SetSpeed(float s);
    void SetIsUpdateTimeScale(bool);
    float GetSpeed() const;

    // 名前でステートを取得（変更可能）
    AnimatorDef::AnimatorState* GetStatePtr(const std::string& name) {
        auto it = states.find(name);
        if (it != states.end()) return &it->second;
        return nullptr;
    }

    // 名前でステートを取得（const）
    const AnimatorDef::AnimatorState* GetStatePtr(const std::string& name) const {
        auto it = states.find(name);
        if (it != states.end()) return &it->second;
        return nullptr;
    }

    void SetOverrideController(std::shared_ptr<AnimatorDef::AnimatorOverride> ov);

    void Play(const std::string& stateName);        // 即座にステート遷移
    void Stop();                                    // アニメーション停止
    void Pause();                                   // 再生一時停止
    void Resume();                                  // 再開
    void Reset();                                   // 初期状態に戻す

    float GetTime() const;                          // 現在の再生位置
    void SetTime(float t);                          // 指定位置から再生
    const std::string& GetCurrentStateName() const; // 現在ステート名取得
    bool IsPlaying() const;                         // 再生中かどうか

    // AppBase 関数.
    void Start()    override;
    void Update()   override;

	std::shared_ptr<SpriteRenderer> GetOwnerRenderer() const { return ownerRenderer.lock(); }
protected:
    std::shared_ptr<AppBase> Clone() const override {
        return std::make_shared<Animator>(*this);
    }
};