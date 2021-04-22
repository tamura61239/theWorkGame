#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "model_data.h"
#include"shader_type.h"
//アニメーションするモデルのバッファデータクラス	
class ModelResource
{
public:
	//コンストラクタ
	ModelResource(ID3D11Device* device, std::unique_ptr<ModelData> data,SHADER_TYPE shaderType= SHADER_TYPE::USEALLY);
	~ModelResource() {}
	//マテリアル
	struct Material
	{
		VECTOR4F color = { 0.8f, 0.8f, 0.8f, 1.0f };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
	};
	//マテリアルごとにメッシュを区切る
	struct Subset
	{
		u_int		startIndex = 0;
		u_int		indexCount = 0;
		Material* diffuse;
		Material* normal;
		Material* bump;
	};
	//メッシュ
	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;
		std::vector<Subset>						subsets;

		int										nodeIndex;
		std::vector<int>						nodeIndices;
		std::vector<DirectX::XMFLOAT4X4*>		inverseTransforms;
	};
	//getter
	const std::vector<Mesh>& GetMeshes() const { return mMeshes; }
	const std::vector<ModelData::Node>& GetNodes() const { return mData->nodes; }
	const std::vector<ModelData::Animation>& GetAnimations() const { return mData->animations; }
	const std::vector<ModelData::Mesh>& GetMeshDatas() const { return mData->meshes; }
	SHADER_TYPE GetShaderType() const{ return mShaderType; }
private:
	//モデルデータ
	std::unique_ptr<ModelData>	mData;
	std::vector<Material>	mDiffuses;
	std::vector<Material>	mNormals;
	std::vector<Material>	mBumps;
	std::vector<Mesh>		mMeshes;
	//シェーダーの種類
	SHADER_TYPE mShaderType;
};
