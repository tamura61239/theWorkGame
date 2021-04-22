#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"player_ai.h"
#include"drow_shader.h"
#include"static_mesh.h"
#include"constant_buffer.h"
#include"cs_buffer.h"


class RunParticles
{
public:
	//�R���X�g���N�^
	RunParticles(ID3D11Device* device, std::shared_ptr<PlayerAI>player);
	//�G�f�B�^
	void Editor();
	//�X�V
	void Update(ID3D11DeviceContext* context, float elapsd_time);
	//�`��
	void Render(ID3D11DeviceContext* context);
private: 
	//�萔�o�b�t�@
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
	//�p�[�e�B�N���o�b�t�@
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
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mParticleRender;
	std::unique_ptr<CSBuffer>mParticleIndexs[2];
	std::unique_ptr<CSBuffer>mParticleDeleteIndex;
	std::unique_ptr<CSBuffer>mParticleCount;
	//���b�V���f�[�^
	struct Mesh
	{
		std::unique_ptr<CSBuffer>mIndex;
		std::unique_ptr<CSBuffer>mVertex;
		int mMwshSize;
	};
	std::vector<Mesh>mMeshs;
	//�V�F�[�_�[
	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mStartCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	//SRV
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
	//�G�f�B�^�f�[�^
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
	//�v���C���[���
	std::weak_ptr<PlayerAI>mPlayer;
	//�p�[�e�B�N���̍ő吔
	int mMaxParticle;
	//����
	float mTimer;
	//�p�����[�^�[
	int mIndexNum;
	//�`�悷�鐔
	UINT mRenderCount;
	bool mTestFlag;
};