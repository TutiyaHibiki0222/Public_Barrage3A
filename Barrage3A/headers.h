#pragma once
/*
	ヘッダーファイルの開く順番を定義する。
*/
#include "Project.h"        // プロジェクト全体で使用される定義.
#include "Property.hpp"
#include "Mathf.h"          
#include "Reflection.h"		// 反射やメタ情報に関するクラス.
#include "Vector.h"         // 座標系統のh.
#include "JsonUtility.h"    // JSONの操作に関するユーティリティ.
#include "PlayerPrefs.h"    // プレイヤー設定など.
#include "GameEngine.h"     // ゲームエンジンに関連する機能.
#include "Random.h"         // ランダム系統の変数.
#include "Sound.h"			// サウンド処理.
#include "InputSystem.h"    // 入力システム.
#include "SceneManager.h"	// sceneの処理.