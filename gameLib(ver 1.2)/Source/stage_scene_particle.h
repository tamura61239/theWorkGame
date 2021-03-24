#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"drow_shader.h"
#include"constant_buffer.h"
#include<memory>

class StageSceneParticle
{
public:
	StageSceneParticle(ID3D11Device* device);
	void ImGuiUpdate();
	void Update(ID3D11DeviceContext* context, float elapsdTime);
	void Render(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context,DrowShader*shader);
private:
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSEndShader;
	std::vector<std::unique_ptr<DrowShader>>mShader;
	//UAV
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleCountUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleIndexUAV[2];
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleDeleteIndexUAV;
	//バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleCountBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleIndexBuffer[2];
	//SRV
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;

	struct Particle
	{
		VECTOR3F position;
		VECTOR3F accel;
		VECTOR3F velocity;
		float life;
		VECTOR4F color;
		VECTOR3F scale;
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
		int startIndex;
		VECTOR3F createCentralPosition;
		float randX;
		VECTOR3F createArea;
		VECTOR4F color;
		float maxLife;
		float scale;
		VECTOR2F colorRatio;
		VECTOR4F color2;
	};
	struct CbUpdate
	{
		float elapsdTime;
		VECTOR3F windDirection;
		float maxSpeed;
		VECTOR3F dummy2;

	};
	std::unique_ptr<ConstantBuffer<CbCreate>>mCbCreate;
	std::unique_ptr<ConstantBuffer<CbUpdate>>mCbUpdate;
	struct EditorData
	{
		float randX;
		VECTOR3F createCentralPosition;
		VECTOR3F createArea;
		VECTOR3F windDirection;
		float oneSecondCreateNumber;
		VECTOR4F color;
		float maxLife;
		float scale;
		float maxSpeed;
		VECTOR2F colorRatio;
		VECTOR4F color2;
		int shaderType;
		UINT textureType;
	};
	int mIndexCount;
	float mCreateCount;
	int mMaxCount;
	UINT mRenderCount;
	EditorData mEditorData;
};