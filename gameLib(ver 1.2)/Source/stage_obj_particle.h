#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"stage_obj.h"

class StageObjParticle
{
public:
	StageObjParticle(ID3D11Device* device);
	void CreateBuffer(ID3D11Device* device, std::vector<std::shared_ptr<StageObj>>stages);
	void Update(ID3D11DeviceContext* context, float elapsd_time, const int colorState);
	void Render(ID3D11DeviceContext* context);
private:
	//�o�b�t�@(UnorderedAccessView�֌W)
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mStageObjs;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRands;
	//�o�b�t�@(�萔)
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbStartData;
	//�R���s���[�g�V�F�[�_�[�̃r���[
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mStageObjsSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mRandsSRV;

	//stageObj�̃o�b�t�@�f�[�^
	struct Obj
	{
		VECTOR3F max;
		VECTOR3F min;
	};
	//particle�̃o�b�t�@�f�[�^
	struct Particle
	{
		VECTOR4F position = VECTOR4F(0, 0, 0, 0);
		float life = 0;
		VECTOR3F scale = VECTOR3F(0, 0, 0);
		VECTOR4F color = VECTOR4F(1, 0, 0, 0);
		VECTOR3F velocity = VECTOR3F(0, 0, 0);
		float colorType = -1;
		VECTOR3F angle = VECTOR3F(0, 0, 0);
	};
	//�萔�o�b�t�@�̃f�[�^
	struct CbBuffer
	{
		float maxLife;
		float elapsdTime;
		float nowColorType;
		float maxSize;
		VECTOR3F angleMovement;
		float dummy2;
	};
	struct CbStartData
	{
		float startIndex;
		float indexSize;
		float redNumber;
		float changeColorFlag;
		VECTOR4F redColor;
		VECTOR4F blueColor;
	};
	//�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>mGSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
	int redObjSize;
	int blueObjSize;
	int particleSize;
	CbBuffer mCb;
	CbStartData mStartData;
	void SetRandBufferData(std::vector<VECTOR3F>& data);
	void LoadCbData();
	void SaveCbData();
};