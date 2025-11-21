#pragma once

// LibrarySettings.h
class LibrarySettings {
public:
	// スティックのデッドゾーン（0.0〜1.0）
	inline static float StickDeadZone = 0.15f; // ← デフォルト15%
	// 最大パッド接続数（変更可能にする場合は Config 経由でもOK）
	inline static constexpr size_t MaxPadCount = 1;
};
