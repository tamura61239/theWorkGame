#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "model_data.h"
#include"shader_type.h"

class ModelResource
{
public:
	ModelResource(ID3D11Device* device, std::unique_ptr<ModelData> data,SHADER_TYPE shaderType= SHADER_TYPE::USEALLY);
	~ModelResource() {}

	struct Material
	{
		VECTOR4F color = { 0.8f, 0.8f, 0.8f, 1.0f };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	};

	struct Subset
	{
		u_int		start_index = 0;
		u_int		index_count = 0;
		Material* diffuse;
		Material* normal;
		Material* bump;
	};

	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	index_buffer;
		std::vector<Subset>						subsets;

		int										node_index;
		std::vector<int>						node_indices;
		std::vector<DirectX::XMFLOAT4X4*>		inverse_transforms;
	};

	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }
	const std::vector<ModelData::Node>& GetNodes() const { return m_data->nodes; }
	const std::vector<ModelData::Animation>& GetAnimations() const { return m_data->animations; }
	SHADER_TYPE GetShaderType() const{ return mShaderType; }
private:
	std::unique_ptr<ModelData>	m_data;
	std::vector<Material>	diffuses;
	std::vector<Material>	normals;
	std::vector<Material>	bumps;
	std::vector<Mesh>		m_meshes;
	SHADER_TYPE mShaderType;
};
