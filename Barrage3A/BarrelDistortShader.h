#pragma once
#include "PixelShaderBase.h"

class BarrelDistort : public PixelShaderBase {
private:


public:
	BarrelDistort() : PixelShaderBase() {}

	void LoadShader() override {
		
		if(shader == -1) {
			shader = LoadPixelShader("Resources/Shader/BarrelDistort.pso");
		}
		
		if (shader == -1) {
			std::cerr << "Failed to load BarrelDistort shader." << std::endl;
		}
	}

	void InitializeShader() override {
		sortingLayer.layer = -7;
	}

	void OnUpdateShader() override {
		auto size = Window::GetInstance().GetMaxVector2D();
		auto pos = transform->GetWorldPosition();
		SetUseTextureToShader(0, scene);
		SetPSConstSF(0, 2.0f);						//0なのでc0
		SetPSConstSF(1, 2.75f);						//0なのでc1
		SetPSConstSF(2, pos.x / size.x);	//0なのでc2
		SetPSConstSF(3, pos.y / size.y);	//0なのでc3
		SetPSConstSF(4, 2.0f);						// HeatStrength（0〜1ぐらい）
		SetPSConstSF(5, Time.time);
		SetPSConstSF(6, size.y / size.x); // aspectRatioをc6にセット
	}
protected:
	std::shared_ptr<AppBase> Clone() const override {
		return std::make_shared<BarrelDistort>(*this);
	};
};