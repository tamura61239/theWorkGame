#include "static_obj.h"

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
/****************************メッシュを生成*************************/

StaticObj::StaticObj(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType, bool pathOrganize, int organizeType):Obj3D()
{
	mMesh = std::make_shared<StaticMesh>(device, fileName, shaderType, pathOrganize, organizeType);
}
/*************************メッシュを取得****************************/

StaticObj::StaticObj(std::shared_ptr<StaticMesh> mesh)
{
	mMesh = mesh;
}
/*****************************************************/
//　　　　　　　　　　レイピック関数
/*****************************************************/

int StaticObj::RayPick(const VECTOR3F& startPosition, const VECTOR3F& endPosition, VECTOR3F* outPosition, VECTOR3F* outNormal, float* outLength)
{
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&GetWorld());
	DirectX::XMMATRIX invers_W;
	invers_W = DirectX::XMMatrixInverse(NULL, W);
	//オブジェクト空間でのレイに変換
	DirectX::XMVECTOR startposition = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR endposition = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR loacal_start = DirectX::XMVector3TransformCoord(startposition, invers_W);
	DirectX::XMVECTOR loacal_end = DirectX::XMVector3TransformCoord(endposition, invers_W);
	//レイピック
	float outDistance;
	VECTOR3F start, end;
	DirectX::XMStoreFloat3(&start, loacal_start);
	DirectX::XMStoreFloat3(&end, loacal_end);
	int set = mMesh->RayPick(start, end, outPosition, outNormal, &outDistance);
	if (set != -1)
	{
		//オブジェクト空間からワールド空間へ変換
		DirectX::XMVECTOR out_p = DirectX::XMLoadFloat3(outPosition);
		DirectX::XMVECTOR out_n = DirectX::XMLoadFloat3(outNormal);

		DirectX::XMVECTOR world_position = DirectX::XMVector3TransformCoord(out_p, W);
		DirectX::XMVECTOR world_normal = DirectX::XMVector3TransformNormal(out_n, W);

		DirectX::XMStoreFloat3(outPosition, world_position);
		DirectX::XMStoreFloat3(outNormal, world_normal);

		*outLength = outDistance; // 距離を入れ忘れてたよ～

		return set;
	}
	return set;
}

