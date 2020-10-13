#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>


class RunParticles
{
public:
	RunParticles(ID3D11Device* device);
	void ImGuiUpdate();
	void SetPlayerData(const VECTOR3F& velocity, bool groundFlag, const VECTOR3F& position);
	void Update(ID3D11DeviceContext* context, float elapsd_time);
	void Render(ID3D11DeviceContext* context);
	struct Particle
	{
		VECTOR4F position = VECTOR4F(0, 0, 0, 0);
		VECTOR3F scale = VECTOR3F(0, 0, 0);
		VECTOR3F angle = VECTOR3F(0, 0, 0);
		VECTOR4F color = VECTOR4F(0, 0, 0, 0);
		VECTOR3F velocity = VECTOR3F(0, 0, 0);
		VECTOR3F accel = VECTOR3F(0, 0, 0);
		float life = 0;
	};
private:
	struct Cb
	{
		VECTOR3F angleMovement;
		float elapsdTime;
		float startIndex;
		float indexSize;
		float createFlag;
		float dummy;
	};
	struct CbStart
	{
		VECTOR4F playerPosition;
		VECTOR4F color;
		VECTOR3F playerVelocity;
		float maxLife;
		float rand1;
		float rand2;
		float totalRand;
		float moveType;

	};
	struct PlayerData
	{
		VECTOR3F mVelocity;
		VECTOR3F mPosition;
		bool mGroundFlag;
	};
	//バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbStartBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRandBuffer;
	//UAV
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	//SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mRandSRV;
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>mGSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
	Cb mCb;
	CbStart mCbStart;
	PlayerData mPlayerData;
	void SetRandBufferData(std::vector<VECTOR2F>&data);
	bool flag;
	void Load();
	void Save();
};