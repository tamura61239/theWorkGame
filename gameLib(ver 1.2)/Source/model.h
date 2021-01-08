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
		FLOAT4X4	localTransform;
		FLOAT4X4	worldTransform;
		FLOAT4X4    beforeWorldTransform;
	};

	// アニメーション
	bool IsPlayAnimation() const { return mCurrentAnimation >= 0; }
	void PlayAnimation(int animationIndex, bool loop = false);
	void UpdateAnimation(float elapsedTime);

	// 行列計算
	void CalculateLocalTransform();
	void CalculateWorldTransform(const DirectX::XMMATRIX& worldTransform);

	const std::vector<Node>& GetNodes() const { return mNodes; }
	const ModelResource* GetModelResource() const { return mModelResource.get(); }

private:
	std::shared_ptr<ModelResource>	mModelResource;
	std::vector<Node>				mNodes;
	int								mCurrentAnimation = -1;
	float							mCurrentSeconds = 0.0f;
	bool							mLoopAnimation = false;
	bool							mEndAnimation = false;
};
