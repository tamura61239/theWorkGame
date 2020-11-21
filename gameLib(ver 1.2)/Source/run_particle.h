#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"model.h"
#include"drow_shader.h"
#include"static_mesh.h"

#define RUNPARTICLE_TYPE 0

class RunParticles
{
public:
	RunParticles(ID3D11Device* device);
	void ImGuiUpdate();
	void SetBoneData(Model* model);
	void SetPlayerData(const VECTOR3F&velocity,const bool playFlag);
	void Update(ID3D11DeviceContext* context, float elapsd_time);
	void Render(ID3D11DeviceContext* context);
private:
#if (RUNPARTICLE_TYPE==0)
	struct CbeBone
	{
		VECTOR4F boneWorld[32];
		float boneNumber;
		VECTOR3F dummmy;
	};
	struct CbCreateData
	{
		VECTOR3F velocity;
		float maxLife;
		VECTOR4F color;
		float scale;
		float mStartNumber;
		float speed;
		float maxY;
	};
#elif (RUNPARTICLE_TYPE==1)
public:
	void SetMeshData(Model* model, ID3D11Device* device);
private:
	struct Vertex
	{
		VECTOR3F	position;
		VECTOR3F	normal;
		VECTOR4F	bone_weight;
		UVECTOR4	bone_index;
	};
	struct CbeBone
	{
		FLOAT4X4 boneTransform[32];
	};
	struct CbCreateData
	{
		VECTOR3F velocity;
		float maxLife;
		VECTOR4F color;
		float scale;
		float mStartNumber;
		float speed;
		float meshSize;
	};
	int mMeshSize;
	float mCreateTimeCount;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mMeshSRV;
#endif
	struct CbUpdate
	{
		float elapsdTime;
		VECTOR3F dummy2;
	};
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F velocity;
		float life;
		VECTOR4F color;
		float scele;
		float lifeAmount;
	};
	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	struct EditorData
	{
		float maxLife;
		VECTOR4F color;
		float scale;
		float mCreateSize;
		float speed;

	};
	int mRenderSize;
	bool mPlayFlag;
	bool mTestFlag;
	VECTOR3F mTestVelocity;
	float mNewIndex;
	//Mesh周り
	std::vector<CbeBone>mCbBones;
	CbeBone mCbBone;
	//パーティクルバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreateBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBoneBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbUpdateBuffer;
	//Shader
	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mUpdateShader;
	CbCreateData mCbCreateData;
	EditorData mEditorData;
	//ファイル操作関数
	void Load();
	void Save();
};