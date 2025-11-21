#pragma once

#include <string>
#include <functional>

#include "AnimatorParameter.h"

namespace AnimatorDef {

    struct AnimatorCondition {
        // 条件を評価するパラメータの名前.
        std::string parameterName;   

        // 条件を評価するためのラムダ式.
        std::function<bool(const AnimatorParameter&)> condition;

        // デフォルトコンストラクタ
        AnimatorCondition() {}

        // bool 値に対する条件コンストラクタ
        AnimatorCondition(const std::string& paramName, bool expectedValue)
            : parameterName(paramName), condition([expectedValue](const AnimatorParameter& param) {
            return param.boolValue == expectedValue;
                }) {}

        // 任意の関数で条件を与えるコンストラクタ
        AnimatorCondition(const std::string& paramName, std::function<bool(const AnimatorParameter&)> cond)
            : parameterName(paramName), condition(cond) {}

        // float 値に対する条件コンストラクタ
        AnimatorCondition(const std::string& paramName, float expectedValue, float epsilon = 0.01f)
            : parameterName(paramName), condition([expectedValue, epsilon](const AnimatorParameter& param) {
            return std::abs(param.floatValue - expectedValue) < epsilon;
                }) {}

        // Trigger 値に対する条件コンストラクタ
        AnimatorCondition(const std::string& paramName) : parameterName(paramName),
            condition([](const AnimatorParameter& param) {
            return param.triggered;
                }) {}

        // int 値に対する条件コンストラクタ
        AnimatorCondition(const std::string& paramName, int expectedValue)
            : parameterName(paramName),
            condition([expectedValue](const AnimatorParameter& param) {
            return param.intValue >= expectedValue;
                }) {}

        // float 値に対する Less または Greater 条件 (誤差も考慮)
        AnimatorCondition(const std::string& paramName, float expectedValue, bool isLess, float epsilon = 0.01f)
            : parameterName(paramName), condition([expectedValue, isLess, epsilon](const AnimatorParameter& param) {
            return isLess ? (param.floatValue < expectedValue - epsilon) : (param.floatValue > expectedValue + epsilon);
                }) {}

        // int 値に対する Less または Greater 条件
        AnimatorCondition(const std::string& paramName, int expectedValue, bool isLess)
            : parameterName(paramName), condition([expectedValue, isLess](const AnimatorParameter& param) {
            return isLess ? (param.intValue < expectedValue) : (param.intValue > expectedValue);
                }) {}
    };

    // アニメーターの状態遷移を定義する構造体
    struct AnimatorTransition {
        // 遷移先のステート名.
        std::string toStateName;
        // 遷移条件のリスト.
        std::vector<AnimatorCondition> conditions; 
        // この遷移が終了条件を持つかどうかを示すフラグ.
        bool hasExitTime = false;

        // bool型の条件追加
        void AddBoolCondition(const std::string& paramName, bool expectedValue) {
            conditions.emplace_back(paramName, expectedValue);
        }

        // float型の条件追加
        void AddFloatCondition(const std::string& paramName, float expectedValue, float epsilon = 0.01f) {
            conditions.emplace_back(paramName, expectedValue, epsilon);
        }

        // Trigger型の条件追加
        void AddTriggerCondition(const std::string& paramName) {
            conditions.emplace_back(paramName);
        }

        // Less 条件追加 (例: 値が expectedValue より小さい)
        void AddLessCondition(const std::string& paramName, float expectedValue, float epsilon = 0.01f) {
            conditions.emplace_back(paramName, expectedValue, true, epsilon);
        }

        // Greater 条件追加 (例: 値が expectedValue より大きい)
        void AddGreaterCondition(const std::string& paramName, float expectedValue, float epsilon = 0.01f) {
            conditions.emplace_back(paramName, expectedValue, false, epsilon);
        }

        // int 型の Less 条件追加
        void AddIntLessCondition(const std::string& paramName, int expectedValue) {
            conditions.emplace_back(paramName, expectedValue, true);
        }

        // int 型の Greater 条件追加
        void AddIntGreaterCondition(const std::string& paramName, int expectedValue) {
            conditions.emplace_back(paramName, expectedValue, false);
        }
    };

}
