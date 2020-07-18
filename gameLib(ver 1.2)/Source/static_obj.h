#pragma once
#include"static_mesh.h"
#include"obj3d.h"

class StaticObj:public Obj3D
{
public:
	StaticObj(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType = SHADER_TYPE::USEALLY, bool pathOrganize = false, int organizeType = 0);
	int RayPick(
		const VECTOR3F& startPosition,//レイを飛ばす開始座標
		const VECTOR3F& endPosition,//レイを飛ばす終了座標
		VECTOR3F* outPosition,//レイが当たった座標
		VECTOR3F* outNormal,//レイが当たった面の法線
		float* outLength//レイが当たった面までの距離
	);
	StaticMesh* GetMesh() { return mMesh.get(); }
private:
	std::unique_ptr<StaticMesh>mMesh;
};