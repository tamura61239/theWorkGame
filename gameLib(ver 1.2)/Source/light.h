#pragma once
#include"vector.h"
#include<wrl.h>
#include <d3d11.h>

struct SpotLight
{
	float index;
	float range;//光の届く範囲
	float type; //有効か無効か
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
	float range;//光の届く範囲
	float type; //有効か無効か
	float dumy;
	VECTOR4F position;
	VECTOR4F color;
};
static const int POINTMAX = 32;
static const int SPOTMAX = 32;

class Light
{
public:
	//バッファ生成関数
	void CreateLightBuffer(ID3D11Device* device);
	//setter
	void SetLightDirection(const VECTOR4F& lightDirection) { mDefLight.mLightDirection = lightDirection; }
	void SetLightColor(const VECTOR4F& lightColor) { mDefLight.mLightColor = lightColor; }
	void SetAmbientColor(const VECTOR4F& ambientColor) { mDefLight.mAmbientColor = ambientColor; }
	void SetPointLight(int index, VECTOR3F position, VECTOR3F color, float range);
	void SetSpotLight(int index, VECTOR3F position, VECTOR3F color, VECTOR3F dir, float range, float nearArea, float farArea);
	void ConstanceLightBufferSetShader(ID3D11DeviceContext* context);
	//getter
	const VECTOR4F& GetLightDirection() { return mDefLight.mLightDirection; }
	const VECTOR4F& GetLightColor() { return mDefLight.mLightColor; }
	const VECTOR4F& GetAmbientColor() { return mDefLight.mAmbientColor; }
	PointLight* GetPointLight() { return mPointLight; }
	SpotLight* GetSpotLight() { return mSpotLight; }

	//インスタンス関数
	static Light& GetInctance()
	{
		static Light light;
		return light;
	}
	//更新
	void Editor();
private:
	Light();
	struct CbLight
	{
		PointLight pointLight[POINTMAX];
		SpotLight spotLight[SPOTMAX];
	};
	struct CbDefLight
	{
		VECTOR4F mLightColor;
		VECTOR4F mLightDirection;
		VECTOR4F mAmbientColor;
		VECTOR4F mEyePosition;
		VECTOR3F mSkyColor;
		float dummy1;
		VECTOR3F mGroundColor;
		float dummy2;
	};

	PointLight mPointLight[POINTMAX];
	SpotLight mSpotLight[SPOTMAX];
	CbDefLight mDefLight;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbLight;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbDefLight;
};
#define pLight (Light::GetInctance())