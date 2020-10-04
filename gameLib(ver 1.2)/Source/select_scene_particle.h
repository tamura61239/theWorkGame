#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"drow_shader.h"
#include<memory>

class SelectSceneParticle
{
public:
	SelectSceneParticle(ID3D11Device* device);
	void ImGuiUpdate();
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbStartBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mNumberBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mNumberUAV;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mRenderSetCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mClearCSShader;
	std::unique_ptr<DrowShader>mShader;
	struct Particle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR3F angleMovement;
		VECTOR4F color;
		VECTOR3F velocity;
		float speed;
		float life;
		float sinAngle;
		float sinAngleMovement;
		float sinLength;
		VECTOR3F centerPosition;
	};
	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	struct CbStart
	{
		VECTOR3F angleMovement;
		float speed;
		VECTOR4F color;
		float startIndex;
		VECTOR3F eye;
		float range;
		VECTOR3F scope;
	};
	struct Cb
	{
		VECTOR3F defVelocity;
		float elapsdTime;
		VECTOR3F endPosition;
		float dummy2;
	};
	CbStart mCbStart;
	Cb mCb;
	float particleSize;
	float newIndex;
};