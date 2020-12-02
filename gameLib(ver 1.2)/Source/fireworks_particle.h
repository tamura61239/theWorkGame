#pragma once
#include"drow_shader.h"
#include<memory>
#include"vector.h"
#include<vector>


class FireworksParticle
{
public:
	FireworksParticle(ID3D11Device* device);
	void ImGuiUpdate();
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
private:
	//シェーダー
	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreate1Shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreate2Shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	//描画用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreateBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreate2Buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbUpdateBuffer;

	//UAV
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	struct EmitorData
	{
		VECTOR3F position;
		VECTOR3F velocity;
		float maxTime;
		float speed;
	};
	struct Emitor
	{
		Emitor() :position(0, 0, 0), velocity(0, 0, 0),type(0),timer(0),maxTime(0) {}
		VECTOR3F position;
		VECTOR3F velocity;
		int type;
		float timer;
		float maxTime;
	};
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F velocity;
		VECTOR3F accel;
		float scale;
		float life;
		float lifeAmoust;
		float endTime;
		float endTimer;
		VECTOR4F startColor;
		VECTOR4F endColor;
	};
	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	struct FireworksData
	{
		float maxCount;
		float maxLife;
		float speed;
		float scale;
		float parsent;
		float endTimer;
		float gravity;

		float dummy;

		VECTOR4F color;
		VECTOR4F endColor;
	};
	struct CreateData
	{
		VECTOR3F position;
		float dummy2;
		VECTOR3F velocity;
		float dummy3;
		FireworksData firework;
	};

	struct CbCreate
	{
		CreateData createData[30];
	};
	struct CbUpdate
	{
		float elapsdime;
		VECTOR3F dummy;
	};
	std::vector<Emitor>mEmitors;
	EmitorData mEmitorData;
	FireworksData mFireworkData;
	bool createFlag;
	int mMaxParticle;
};