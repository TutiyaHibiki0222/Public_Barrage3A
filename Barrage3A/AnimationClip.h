#pragma once

#include "Texture2D.h"
using namespace GameEngine;

namespace AnimatorDef {

    // アニメーションクリップを表すクラス
    class AnimationClip {
    public:
        // アニメーションの総時間（秒単位）.
        float duration = 1.0f;
        // アニメーションのフレームを保持するベクター.
        std::vector<std::shared_ptr<Sprite>> frames;
        // アニメーションがループするかどうか.
        bool loop = true;
    public:
        // フレーム数
        size_t GetFrameCount() const {
            return frames.size();
        }

        // 時間から現在のフレームを取得
        std::shared_ptr<Sprite> GetFrameAtTime(float time) const {
            if (frames.empty()) return nullptr;

            float frameDuration = duration / frames.size();
            size_t index = static_cast<size_t>(time / frameDuration);

            if (!loop) {
                if (index >= frames.size()) {
                    return frames.back();  // 非ループ時は最後のフレーム固定
                }
                return frames[index];
            }

            // ループ時のみmod演算
            index = index % frames.size();
            return frames[index];
        }

    };

}