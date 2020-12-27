#pragma once
#include"camera.h"
#include<memory>
#include<string>

class LightView
{
public:
	LightView(ID3D11Device* device,std::string fileName="");
	void Update(const VECTOR3F&targetPosition,ID3D11DeviceContext*context);
	void ImGuiUpdate();
	Camera* GetLightCamera() {
		return mLightCamera.get();
	}
private:
	void Load(std::string fileName);
	void Save(std::string fileName);
	struct LightViewData
	{
		float lightEyeLength = 750.f;
		float viewportMagnification = 3.5f;
		float localFocus[3] = { 0,0,0 };
	};
	std::unique_ptr<Camera>mLightCamera;
	char mFileName[256];
	LightViewData mData;
};