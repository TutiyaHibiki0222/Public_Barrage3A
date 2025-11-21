//#pragma once
//#include "../AppBase.h"
//#include "../GameObject.h"
//#include"../headers.h"
//#include "../Debug.hpp"
//
//using namespace GameEngine;
//
//class CharacterBase :public AppBase
//{
//public:
//    CharacterBase() :AppBase(""),hp(0), speed(0),isShot(false) {}
//    CharacterBase(std::shared_ptr<GameObject> _obj) : AppBase("", _obj), hp(0),  speed(0), isShot(false) {}
//
//protected:
//    int hp;
//    float speed;
//    bool isShot;
//
//protected:
//    std::shared_ptr<AppBase> Clone() const override {
//        return std::make_shared<CharacterBase>(*this);
//    }
//    virtual void Move() {};
//    virtual void Shot() {};
//};
//
