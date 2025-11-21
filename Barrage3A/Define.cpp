#include "Define.h"

const int Define::WIN_W = 970;        //ウィンドウサイズ横 変更
const int Define::WIN_H = 720;        //ウィンドウサイズ縦 変更

const int Define::OUT_W = Define::WIN_W - (334 - (39));
const int Define::OUT_H = Define::WIN_H - ( 34 - 31);

const int Define::IN_W = Define::OUT_W - 50;
const int Define::IN_H = Define::OUT_H - 31;

const int Define::CENTER_X = (Define::OUT_W / 2);  //動ける範囲の中心X（OUT_Wに合わせて）
const int Define::CENTER_Y = (Define::OUT_H / 2);  //動ける範囲の中心Y（OUT_Hに合わせて）

const int Define::IN_X = ((Define::OUT_W - Define::IN_W) / 2);  //内枠のX
const int Define::IN_Y = ((Define::OUT_H - Define::IN_H) / 2);  //内枠のY

const float Define::PI = 3.141592654f;    //円周率
