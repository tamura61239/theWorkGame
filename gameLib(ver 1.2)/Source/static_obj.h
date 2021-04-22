#pragma once
#include"static_mesh.h"
#include"obj3d.h"

static const float gameObjScale = 2;
//モデルのオブジェクトデータ
class StaticObj:public Obj3D
{
public:
	//コンストラクタ
	StaticObj(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType = SHADER_TYPE::USEALLY, bool pathOrganize = false, int organizeType = 0);
	StaticObj(std::shared_ptr<StaticMesh>mesh);
	//レイピック
	int RayPick(
		const VECTOR3F& startPosition,//レイを飛ばす開始座標
		const VECTOR3F& endPosition,//レイを飛ばす終了座標
		VECTOR3F* outPosition,//レイが当たった座標
		VECTOR3F* outNormal,//レイが当たった面の法線
		float* outLength//レイが当たった面までの距離
	);
	//getter
	StaticMesh* GetMesh() { return mMesh.get(); }
	
private:
	//モデルデータ
	std::shared_ptr<StaticMesh>mMesh;
};