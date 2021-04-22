#pragma once
#include <memory>
#include <d3d11.h>
#include "model_resource.h"

//アニメーションするモデルのクラス
class Model
{
public:
	//コンストラクタ
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
	//終了
	bool IsPlayAnimation() const { return mCurrentAnimation >= 0; }
	//開始
	void PlayAnimation(int animationIndex, bool loop = false, float changeTime=0);
	//更新
	void UpdateAnimation(float elapsedTime);

	// 行列計算
	//ボーンローカル空間行列の更新
	void CalculateLocalTransform();
	//ボーンのワールド空間行列の更新
	void CalculateWorldTransform(const DirectX::XMMATRIX& worldTransform);
	//getter
	const std::vector<Node>& GetNodes() const { return mNodes; }
	const ModelResource* GetModelResource() const { return mModelResource.get(); }

private:
	//アニメーションブレンド
	void BlendAnimation(float elapsed_time);
	//モデルのデータ
	std::shared_ptr<ModelResource>	mModelResource;
	std::vector<Node>				mNodes;
	std::vector<Node>				mChangeSceneNodes;
	//アニメーション更新に使う変数
	int								mCurrentAnimation = -1;
	float							mCurrentSeconds = 0.0f;
	bool							mLoopAnimation = false;
	bool							mEndAnimation = false;
	bool                            mChangeAnimation = false;
	float                           mChangeTime = 0.f;

};
