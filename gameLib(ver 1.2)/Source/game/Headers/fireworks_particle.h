#pragma once
#include"drow_shader.h"
#include<memory>
#include"vector.h"
#include<vector>
#include"cs_buffer.h"
#include"constant_buffer.h"
class FireworksParticle
{
public:
	//�R���X�g���N�^
	FireworksParticle(ID3D11Device* device);
	FireworksParticle(const FireworksParticle&fireworks){}
	//�G�f�B�^
	void Editor();
	//�X�V
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	//�`��
	void Render(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context,DrowShader*shader);
	//�G�~�b�^�[�̐���
	void CreateEmitor(const int ranking)
	{
		//�����L���O�ɍ��킹�Đ�������G�~�b�^�[�̐��𑝂₷
		mCreateFlag = true;
		mEmitorTimer = 0;
		mEmitors.resize(mStartEmitorData.size());
		mCreateCount.resize(mStartEmitorData.size());
		memset(&mCreateCount[0], 0, sizeof(int) * mCreateCount.size());
		mIndex = 0;
		mState = 0;
		mNowPlayRanking = ranking;
		SetStartList((5 - mNowPlayRanking) * mEditorData.mOneRankEmitorCount+ mEditorData.mOneRankEmitorCount/2);
	}
	//�G�~�b�^�[�����
	void ClearEmitor()
	{
		mCreateFlag = false;
		mEmitorTimer = 0;
		mEmitors.clear();
		mCreateCount.clear();
		mIndex = 0;
		mState = 0;
	}
private:
	/****************************�ϐ�*******************************/
	//�V�F�[�_�[
	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSClearShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCountShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreate1Shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreate2Shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	//�p�[�e�B�N���f�[�^
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
	struct ParticleCount
	{
		UINT aliveParticleCount;
		UINT aliveNewParticleCount;
		UINT deActiveParticleCount;
		UINT dummy;
	};

	//�p�[�e�B�N���o�b�t�@
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mParticleCount;
	//�`��p�o�b�t�@
	std::unique_ptr<CSBuffer>mParticleRender;
	std::unique_ptr<CSBuffer>mParticleIndexs[2];
	std::unique_ptr<CSBuffer>mParticleDeleteIndex;

	//�����p�f�[�^
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
	//�萔�o�b�t�@
	struct CbCreate
	{
		CreateData createData[30];
		int startIndex;
		VECTOR3F dummy2;

	};
	struct CbUpdate
	{
		float elapsdime;
		VECTOR3F dummy;
	};
	std::unique_ptr<ConstantBuffer<CbCreate>>mCbCreate;
	std::unique_ptr<ConstantBuffer<CbUpdate>>mCbUpdate;
	//SRV
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
	//�G�~�b�^�[�f�[�^
	struct EmitorData
	{
		EmitorData() :position(0, 0, 0), velocity(0, 0, 0), type(0), maxLife(0), speed(0), emitorStartTime(0) {}

		VECTOR3F position;
		VECTOR3F velocity;
		float maxLife;
		float speed;
		float emitorStartTime;
		int type;
	};
	struct Emitor
	{
		Emitor() :position(0, 0, 0), velocity(0, 0, 0), type(-1), life(0) {}
		VECTOR3F position;
		VECTOR3F velocity;
		int type;
		float life;
	};
	//�G�f�B�^�̃f�[�^
	struct EditorData
	{
		float mMaxEmitorTime;
		float mStartMaxTime;
		int mOneRankEmitorCount;
		UINT textureType;
	};
	//�G�f�B�^�p�����[�^�[
	std::vector<EmitorData>mEmitorData;
	std::vector<EmitorData>mStartEmitorData;
	std::vector<FireworksData>mFireworkDatas;
	EditorData mEditorData;
	//�Z�[�u���Ȃ��ϐ�
	bool mParameterEditFlag;
	bool mCreateFlag;
	int mDefRanking;
	int mDefStartState;
	//�`��p�f�[�^
	UINT mRenderCount;
	int mIndexNum;
	//updataParameter
	int mMaxParticle;
	float mEmitorTimer;
	int mIndex;
	int mNowPlayRanking;
	int mState;
	int mFireworksCount;
	std::vector<Emitor>mEmitors;
	std::vector<int>mCreateCount;
	std::vector<int>mStartCreateNumberList;
	/****************************�֐�********************************/
	void Load();
	void Save();
	void Clearount(ID3D11DeviceContext* context);
	void StartFireworksEmitorUpdate(float elapsdTime, CbCreate* smokeConstant, CbCreate* fireworksConstant, int& emitorCount, int& fireworksCount);
	void LoopFireworksEmitorUpdate(float elapsdTime, CbCreate* smokeConstant, CbCreate* fireworksConstant, int& emitorCount, int& fireworksCount);
	void SetStartList(const int size);

};