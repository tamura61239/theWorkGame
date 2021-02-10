#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"drow_shader.h"
#include"constant_buffer.h"
#include<memory>

class SelectSceneParticle
{
public:
	SelectSceneParticle(ID3D11Device* device);
	void ImGuiUpdate();
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
private:
	//バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleCountBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleIndexBuffer[2];
	//srv
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	//uav
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleCountUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleIndexUAV[2];
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mDeleteIndexUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSEndShader;
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
	struct ParticleCount
	{
		UINT aliveParticleCount;
		UINT aliveNewParticleCount;
		UINT deActiveParticleCount;
		UINT dummy;
	};

	struct CbCreate
	{
		VECTOR3F angleMovement;
		float speed;
		VECTOR4F color;
		float startIndex;
		VECTOR3F eye;
		float range;
		VECTOR3F scope;
	};
	struct CbUpdate
	{
		VECTOR3F defVelocity;
		float elapsdTime;
		VECTOR3F endPosition;
		float dummy2;
	};
	std::unique_ptr<ConstantBuffer<CbCreate>> mCbCreate;
	std::unique_ptr<ConstantBuffer<CbUpdate>> mCbUpdate;
	float newIndex;
	int mIndexCount;
	float mMaxParticle;
	UINT mRenderCount;
};