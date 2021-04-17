#pragma once
#include"static_obj.h"

//�X�e�[�W�I�u�W�F�N�g�̕K�v�f�[�^
struct StageData
{
	VECTOR3F mPosition;
	VECTOR3F mAngle;
	VECTOR3F mScale;
	int mColorType;
	int mObjType;
};
//�X�e�[�W�I�u�W�F�N�g�N���X
class StageObj :public StaticObj
{
public:
	StageObj(std::shared_ptr<StaticMesh>mesh);
	StageObj(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType = SHADER_TYPE::USEALLY, bool pathOrganize = false, int organizeType = 0);
	//setter
	void SetStageData(const StageData& data)
	{
		static float s = gameObjScale / 10.0f;
		mData = data;
		mPosition = data.mPosition * s;
		mScale = data.mScale * s;
		mAngle = data.mAngle;
	}
	void SetColorType(const int type) { mData.mColorType = type; }
	//getter
	const StageData& GetStageData() { return mData; }
	void SetBeforeWorld(const FLOAT4X4& w) { beforeWorld = w; }
	const FLOAT4X4& GetBeforeWorld() { return beforeWorld; }
private:
	//�I�u�W�F�N�g�̃f�[�^
	StageData mData;
	//�O�̃t���[���̃��[���h��ԍs��
	FLOAT4X4 beforeWorld;
};