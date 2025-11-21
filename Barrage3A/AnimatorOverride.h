#pragma once

#include "AnimationClip.h"

#include <memory>
#include <unordered_map>
#include <string>
namespace AnimatorDef {

	class AnimatorOverride {
	private:
		std::unordered_map < std::string, std::shared_ptr< AnimationClip>> dataMap;
	public:
		AnimatorOverride() = default;

        // 指定ステートに Clip をセット（Override）
        void SetClip(const std::string& stateName, std::shared_ptr<AnimationClip> clip) {
            dataMap[stateName] = clip;
        }

        // 指定ステートの Clip を取得
        std::shared_ptr<AnimationClip> GetClip(const std::string& stateName) const {
            auto it = dataMap.find(stateName);
            if (it != dataMap.end()) return it->second;
            return nullptr;
        }

        // 全 Clip を取得（必要に応じて）
        const std::unordered_map<std::string, std::shared_ptr<AnimationClip>>& GetAllClips() const {
            return dataMap;
        }

        // Override が存在するかチェック
        bool HasClip(const std::string& stateName) const {
            return dataMap.find(stateName) != dataMap.end();
        }
	};
}