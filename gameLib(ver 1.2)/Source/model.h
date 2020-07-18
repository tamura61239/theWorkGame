#pragma once
#include <memory>
#include <d3d11.h>
#include "model_resource.h"

class Model
{
public:
	Model(std::shared_ptr<ModelResource>& resource);
	~Model() {}

	struct Node
	{
		const char*			name;
		Node*				parent;
		VECTOR3F	scale;
		VECTOR4F	rotate;
		VECTOR3F	translate;
		FLOAT4X4	local_transform;
		FLOAT4X4	world_transform;
	};

	// アニメーション
	bool IsPlayAnimation() const { return m_current_animation >= 0; }
	void PlayAnimation(int animation_index, bool loop = false);
	void UpdateAnimation(float elapsed_time);

	// 行列計算
	void CalculateLocalTransform();
	void CalculateWorldTransform(const DirectX::XMMATRIX& world_transform);

	const std::vector<Node>& GetNodes() const { return m_nodes; }
	const ModelResource* GetModelResource() const { return m_model_resource.get(); }

private:
	std::shared_ptr<ModelResource>	m_model_resource;
	std::vector<Node>				m_nodes;
	int								m_current_animation = -1;
	float							m_current_seconds = 0.0f;
	bool							m_loop_animation = false;
	bool							m_end_animation = false;
};
