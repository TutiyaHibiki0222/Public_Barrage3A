#pragma once

#include <string>
namespace AnimatorDef {
    // アニメーターのパラメータタイプを列挙するenum.
    enum class AnimatorParameterType {
        Float,      // 浮動小数点数タイプ。数値での比較や計算に使用。
        Int,        // 整数タイプ。数値での比較や条件に使用。
        Bool,       // 真偽値タイプ。条件をチェックするために使用。
        Trigger,    // トリガータイプ。主にアニメーションの開始を示すために使用。  
    };

    // アニメーターのパラメータを保持する構造体
    struct AnimatorParameter {
        AnimatorParameterType type; // パラメータのタイプ（Trigger, Bool, Float）
        // パラメータの値を保持するためのunion。1つのタイプにのみ対応する。
        union {
            bool boolValue;     // 真偽値用
            float floatValue;   // 浮動小数点数用
            int intValue;
        };
        // トリガーの場合、このフラグがtrueに設定される。
        // トリガーは一度のみ発生し、その後はリセットされる。
        bool triggered = false;
    };
}
