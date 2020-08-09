#pragma once
#include"static_obj.h"

struct StageData
{
	VECTOR3F mPosition;
	VECTOR3F mAngle;
	VECTOR3F mScale;
	int mColorType;
	int mObjType;
};
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
		if (data.mColorType == 1)
		{
			mColor = VECTOR4F(0, 0, 1, 0.3f);
		}
		else
		{
			mColor = VECTOR4F(1, 0, 0, 1);
		}
	}
	//getter
	const StageData& GetStageData() { return mData; }
private:
	StageData mData;
};