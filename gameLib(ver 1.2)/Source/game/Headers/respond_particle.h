#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"player_manager.h"
#include"drow_shader.h"
#include"constant_buffer.h"
#include"cs_buffer.h"


class RespondParticle
{
public:
	//コンストラクタ
	RespondParticle(ID3D11Device* device, PlayerManager*player);
	RespondParticle(const RespondParticle& respond) {};
	//エディタ
	void Editor();
	//更新
	void Update(ID3D11DeviceContext* context, float elapsd_time);
	//描画
	void Render(ID3D11DeviceContext* context);
private:
	void Create(ID3D11DeviceContext* context);
	void Move(ID3D11DeviceContext* context, float elapsd_time);
	//定数バッファ
	struct CbBone
	{
		FLOAT4X4 boneTransForm[128];
	};
	struct CbCreate
	{
		UINT color;
		float expansionTime;
		float respondTime;
		float expansionspeed;
		VECTOR3F respondPosition;
		float centerY;
		float scale;
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
		VECTOR3F start;
		VECTOR3F end;
		float respondTime;
		float timer;
		float expansionTime;
		VECTOR3F velocity;
		VECTOR4F color;
		float scale;
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
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mParticleRender;
	std::unique_ptr<CSBuffer>mParticleIndexs[2];
	std::unique_ptr<CSBuffer>mParticleDeleteIndex;
	std::unique_ptr<CSBuffer>mParticleCount;
	//メッシュデータ
	struct Mesh
	{
		std::unique_ptr<CSBuffer>mIndex;
		std::unique_ptr<CSBuffer>mVertex;
		CbBone inverse;
		int mMeshSize;
	};
	std::vector<Mesh>mMeshs;
	//シェーダー
	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mStartCSShader;

	//SRV
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
	//エディタデータ
	struct EditorData
	{
		float mColor[4];
		float expansionSpeed;
		float respondTime;
		float expansionTime;
		float scale;
		float centerY;
	};
	EditorData mEditorData;
	//プレイヤー情報
	PlayerManager* mPlayer;
	//描画する数
	UINT mRenderCount;
	//パラメーター
	int mIndexNum;
	//パーティクルの最大数
	int mMaxParticle;
	//
	bool mCreateFlag;
	float mTimer;
};
