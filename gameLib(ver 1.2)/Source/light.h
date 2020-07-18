#pragma once
#include"vector.h"
#include<wrl.h>
#include <d3d11.h>

struct SpotLight
{
	float index;
	float range;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type; //—LŒø‚©–³Œø‚©
	float nearArea;
	float farArea;
	float dumy0;
	float dumy1;
	float dumy2;
	VECTOR4F position;
	VECTOR4F color;
	VECTOR4F dir;
};

struct PointLight {
	float index;
	float range;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type; //—LŒø‚©–³Œø‚©
	float dumy;
	VECTOR4F position;
	VECTOR4F color;
};
static const int POINTMAX = 32;
static const int SPOTMAX = 32;

class Light
{
public:
	Light();
	void CreateLightBuffer(ID3D11Device* device);
	//setter
	void SetLightDirection(const VECTOR4F& lightDirection) { mLightDirection = lightDirection; }
	void SetLightColor(const VECTOR4F& lightColor) { mLightColor = lightColor; }
	void SetAmbientColor(const VECTOR4F& ambientColor) { mAmbientColor = ambientColor; }
	void SetPointLight(int index, VECTOR3F position, VECTOR3F color, float range);
	void SetSpotLight(int index, VECTOR3F position, VECTOR3F color, VECTOR3F dir, float range, float near, float far);
	void ConstanceLightBufferSetShader(ID3D11DeviceContext* context);
	//getter
	const VECTOR4F& GetLightDirection() { return mLightDirection; }
	const VECTOR4F& GetLightColor() { return mLightColor; }
	const VECTOR4F& GetAmbientColor() { return mAmbientColor; }
	PointLight* GetPointLight() { return pointLight; }
	SpotLight* GetSpotLight() { return spotLight; }

	static Light& GetInctance()
	{
		static Light light;
		return light;
	}
	struct CbLight
	{
		PointLight pointLight[POINTMAX];
		SpotLight spotLight[SPOTMAX];
	};
private:
	VECTOR4F mLightDirection;
	VECTOR4F mLightColor;
	VECTOR4F mAmbientColor;
	PointLight pointLight[POINTMAX];
	SpotLight spotLight[SPOTMAX];
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbLight;
};
#define pLight (Light::GetInctance())