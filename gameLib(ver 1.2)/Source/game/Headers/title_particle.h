#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"drow_shader.h"
#include<memory>
#include<vector>
#include"constant_buffer.h"
#include"cs_buffer.h"


class TitleParticle
{
public:
	TitleParticle(ID3D11Device* device);
	void Editor();
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
	void SetChangeFlag(const bool changeFlag) { mSceneChange = changeFlag; }
private:
	//�p�[�e�B�N���o�b�t�@
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mParticleCount;
	//�`��p�o�b�t�@
	std::unique_ptr<CSBuffer>mParticleRender;
	std::unique_ptr<CSBuffer>mParticleIndexs[2];
	std::unique_ptr<CSBuffer>mParticleDeleteIndex;
	//�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mSceneChangeCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mRenderSetCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCountShader;
	std::unique_ptr<DrowShader>mShader;
	//srv
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
	//�p�[�e�B�N���̃f�[�^
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F scale;
		VECTOR3F velocity;
		VECTOR3F angle;
		VECTOR4F color;
		float speed;
		float life;
		float moveAngle;
		float moveAngleMovement;
		float moveAngleLength;
		float maxLife;
		VECTOR3F defVelocity;
		VECTOR3F startPosition;
		VECTOR3F defPosition;

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

	//�萔�o�b�t�@
	struct CbStart
	{
		float startIndex;
		VECTOR3F startPosition;
		float leng;
		VECTOR3F sphereRatio;
		VECTOR4F color;
		float life;
		float moveLen;
		float randSpeed;
		float defSpeed;

	};
	struct CbStart2
	{
		float randMoveLength;
		float defMoveLength;
		float randMoveAngle;
		float randScale;
		VECTOR3F defVelocity;
		float dummy;

	};
	struct Cb
	{
		float elapsdTime;
		VECTOR3F angleMovement;
	};
	std::unique_ptr<ConstantBuffer<CbStart>>mCbStart;
	std::unique_ptr<ConstantBuffer<CbStart2>>mCbStart2;
	std::unique_ptr<ConstantBuffer<Cb>>mCb;
	//�G�f�B�^�f�[�^
	struct EditorData
	{
		VECTOR3F startPosition;
		float leng;
		VECTOR3F sphereRatio;
		VECTOR4F color;
		float life;
		float moveLen;
		float randSpeed;
		float defSpeed;
		float randMoveLength;
		float defMoveLength;
		float randMoveAngle;
		float randScale;
		VECTOR3F defVelocity;
		VECTOR3F angleMovement;
		UINT textureType;
	};
	EditorData mEditorData;
	//��
	float mStartIdex;
	//������
	float particleSize;
	//�ő吔
	int mMaxParticle;
	float mNewIndex;
	//�V�[����؂�ւ��邩�ǂ���
	bool mSceneChange;
	//�`��p�f�[�^
	UINT mRenderCount;
	int mIndexNum;

};