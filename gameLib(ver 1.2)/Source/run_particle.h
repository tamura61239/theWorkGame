#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"player_ai.h"
#include"drow_shader.h"
#include"static_mesh.h"
#include"constant_buffer.h"


class RunParticles
{
public:
	RunParticles(ID3D11Device* device, std::shared_ptr<PlayerAI>player);
	void Editor();
	void Update(ID3D11DeviceContext* context, float elapsd_time);
	void Render(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context,DrowShader*shader);
private: 
	//定数バッファ
	struct CbBone
	{
		FLOAT4X4 boneTransForm[128];
	};
	struct CbCreate
	{
		float life;
		float speed;
		UINT color;
		int indexCount;
		int startIndex;
		VECTOR3F dummy;
	};
	struct CbUpdate
	{
		float elapsdTime;
		VECTOR3F dummy;
	};
	std::unique_ptr<ConstantBuffer<CbBone>>mCbBoneBuffer;
	std::unique_ptr<ConstantBuffer<CbCreate>>mCbCreateBuffer;
	std::unique_ptr<ConstantBuffer<CbUpdate>>mCbUpdateBuffer;
	//パーティクルバッファ
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F velocity;
		UINT color;
		float scale;
		float life;
		float lifeAmoust;
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
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleIndexBuffer[2];
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleIndexUAV[2];
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mDeleteIndexUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleCountBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleCountUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	//メッシュデータ
	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mVertexBuffer;
		int mMwshSize;
	};
	std::vector<Mesh>mMeshs;
	//シェーダー
	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mStartCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	//SRV
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;

	struct EditorData
	{
		float mColor[4];
		float life;
		float speed;
		float mCreateTime;
		int mCreateCount;
		UINT textureType;
	};
	EditorData mEditorData;
	std::weak_ptr<PlayerAI>mPlayer;
	int mMaxParticle;
	float mTimer;
	//パラメーター
	int mIndexNum;
	UINT mRenderCount;
	bool mTestFlag;
};