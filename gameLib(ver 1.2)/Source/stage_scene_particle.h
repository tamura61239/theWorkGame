#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"drow_shader.h"
#include"constant_buffer.h"
#include<memory>
#include"cs_buffer.h"

class StageSceneParticle
{
public:
	//�R���X�g���N�^
	StageSceneParticle(ID3D11Device* device);
	//�G�f�B�^
	void Editor();
	//�X�V
	void Update(ID3D11DeviceContext* context, float elapsdTime);
	//�`��
	void Render(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context,DrowShader*shader);
private:
	//�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSEndShader;
	std::vector<std::unique_ptr<DrowShader>>mShader;
	//�o�b�t�@
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mParticleCount;
	std::unique_ptr<CSBuffer>mParticleIndices[2];
	std::unique_ptr<CSBuffer>mParticleDeleteIndex;
	std::unique_ptr<CSBuffer>mParticleRender;
	//SRV
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
	//�p�[�e�B�N���̃f�[�^
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
	//�萔�o�b�t�@
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
	//�G�f�B�^�f�[�^
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
	EditorData mEditorData;
	//�`��Ɏg��index�z��ԍ�
	int mIndexCount;
	//������
	float mCreateCount;
	//�ő吔
	int mMaxCount;
	//�`�悷�鐔
	UINT mRenderCount;

};