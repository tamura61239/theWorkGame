#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"drow_shader.h"
#include<memory>

class StageSceneParticle
{
public:
	StageSceneParticle(ID3D11Device* device);
	void ImGuiUpdate();
	void Update(ID3D11DeviceContext* context, float elapsdTime);
	void Render(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context,DrowShader*shader);
	void RenderVelocity(ID3D11DeviceContext* context);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRandBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbStartBuffer;
	struct CbStart
	{
		float startIndex;
		VECTOR3F startPosition;
		float maxLife;
		VECTOR3F createRange;
	};
	struct Cb
	{
		float elapsdTime;
		VECTOR3F angleMovement;
	};
	struct Particle
	{
		VECTOR4F position = VECTOR4F(0, 0, 0, 0);
		float life = 0;
		VECTOR3F scale = VECTOR3F(0, 0, 0);
		VECTOR4F color = VECTOR4F(0, 0, 0, 0);
		VECTOR3F velocity = VECTOR3F(0, 0, 0);
		float maxA = 0;
		VECTOR3F angle = VECTOR3F(0, 0, 0);
	};
	struct SaveData
	{
		VECTOR3F startPosition;
		float maxLife;
		VECTOR3F createRange;
		VECTOR3F angleMovement;
		float oneTimeIndexSize;
	};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mRandSRV;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreateShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>mGSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
	std::unique_ptr<DrowShader>mVelocityShader;
	void LoadRandData(std::vector<VECTOR3F>& data);
	void Load();
	void Save();
	int particleSize;
	Cb mCb;
	CbStart mCbStart;
	int oneTimeIndexSize;
	int oneframeIndex;
	float newStartIndex;
};