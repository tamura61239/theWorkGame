#include "stage_obj.h"

/**********************コンストラクタ*************************/
StageObj::StageObj(std::shared_ptr<StaticMesh> mesh):StaticObj(mesh)
{
	mData.mPosition = VECTOR3F(0, 0, 0);
	mData.mScale = VECTOR3F(0, 0, 0);
	mData.mAngle = VECTOR3F(0, 0, 0);
	mData.mObjType = 0;
	mData.mColorType = 0;
}
StageObj::StageObj(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType, bool pathOrganize, int organizeType):StaticObj(device,fileName,shaderType, pathOrganize, organizeType)
{
	mData.mPosition = VECTOR3F(0, 0, 0);
	mData.mScale = VECTOR3F(0, 0, 0);
	mData.mAngle = VECTOR3F(0, 0, 0);
	mData.mObjType = 0;
	mData.mColorType = 0;
}

