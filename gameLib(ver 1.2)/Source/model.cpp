#include "logger.h"
#include "model.h"
#include "misc.h"

Model::Model(std::shared_ptr<ModelResource>& resource)
{
	mModelResource = resource;

	// ノード
	const std::vector<ModelData::Node>& res_nodes = resource->GetNodes();

	mNodes.resize(res_nodes.size());
	mChangeSceneNodes.resize(res_nodes.size());
	for (size_t node_index = 0; node_index < mNodes.size(); ++node_index)
	{
		auto&& src = res_nodes.at(node_index);
		auto&& dst = mNodes.at(node_index);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &mNodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;
	}
}

// アニメーション再生
void Model::PlayAnimation(int animation_index, bool loop, float changeTime)
{
	mCurrentAnimation = animation_index;
	mLoopAnimation = loop;
	mEndAnimation = false;
	mCurrentSeconds = 0.0f;
	mChangeTime = changeTime;
	if (mChangeTime > 0.f)mChangeAnimation = true;
	else mChangeAnimation = false;
	memcpy(&mChangeSceneNodes[0], &mNodes[0], sizeof(Node) * mNodes.size());

}

// アニメーション計算
void Model::UpdateAnimation(float elapsed_time)
{
	if (mCurrentAnimation < 0)
	{
		return;
	}

	if (mModelResource->GetAnimations().empty())
	{
		return;
	}
	if (mChangeAnimation)
	{
		BlendAnimation(elapsed_time);
	}
	else
	{

		const ModelData::Animation& animation = mModelResource->GetAnimations().at(mCurrentAnimation);

		const std::vector<ModelData::Keyframe>& keyframes = animation.keyframes;
		int key_count = static_cast<int>(keyframes.size());
		for (int key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const ModelData::Keyframe& keyframe0 = keyframes.at(key_index);
			const ModelData::Keyframe& keyframe1 = keyframes.at(key_index + 1);
			if (mCurrentSeconds >= keyframe0.seconds && mCurrentSeconds < keyframe1.seconds)
			{
				float rate = (mCurrentSeconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

				assert(mNodes.size() == keyframe0.nodeKeys.size());
				assert(mNodes.size() == keyframe1.nodeKeys.size());
				int node_count = static_cast<int>(mNodes.size());
				for (int node_index = 0; node_index < node_count; ++node_index)
				{
					// ２つのキーフレーム間の補完計算
					const ModelData::NodeKeyData& key0 = keyframe0.nodeKeys.at(node_index);
					const ModelData::NodeKeyData& key1 = keyframe1.nodeKeys.at(node_index);

					Node& node = mNodes[node_index];

					DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
					DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
					DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
					DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

					DirectX::XMStoreFloat3(&node.scale, s);
					DirectX::XMStoreFloat4(&node.rotate, r);
					DirectX::XMStoreFloat3(&node.translate, t);
				}
				break;
			}
		}

		// 最終フレーム処理
		if (mEndAnimation)
		{
			mEndAnimation = false;
			mCurrentAnimation = -1;
			return;
		}

		// 時間経過
		mCurrentSeconds += elapsed_time;
		if (mCurrentSeconds >= animation.secondsLength)
		{
			if (mLoopAnimation)
			{
				mCurrentSeconds -= animation.secondsLength;
			}
			else
			{
				mCurrentSeconds = animation.secondsLength;
				mEndAnimation = true;
			}
		}
	}
}
void Model::BlendAnimation(float elapsed_time)
{
	const ModelData::Animation& animation = mModelResource->GetAnimations().at(mCurrentAnimation);

	const ModelData::Keyframe& keyframe = animation.keyframes.at(0);

	float rate = (mCurrentSeconds / mChangeTime);

	assert(mNodes.size() == keyframe.nodeKeys.size());
	assert(mNodes.size() == mChangeSceneNodes.size());

	int node_count = static_cast<int>(mNodes.size());

	for (int node_index = 0; node_index < node_count; ++node_index)
	{
		const Node& changeNode = mChangeSceneNodes.at(node_index);
		const ModelData::NodeKeyData& key0 = keyframe.nodeKeys.at(node_index);
		Node& node = mNodes[node_index];

		DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&changeNode.scale);
		DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key0.scale);
		DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&changeNode.rotate);
		DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key0.rotate);
		DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&changeNode.translate);
		DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key0.translate);

		DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
		DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
		DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

		DirectX::XMStoreFloat3(&node.scale, s);
		DirectX::XMStoreFloat4(&node.rotate, r);
		DirectX::XMStoreFloat3(&node.translate, t);

	}

	mCurrentSeconds += elapsed_time;
	if (mCurrentSeconds > mChangeTime)
	{
		mCurrentSeconds = 0.f;
		mChangeAnimation = false;
	}
}

// ローカル変換行列計算
void Model::CalculateLocalTransform()
{
	bool lFlag = false;
	for (Node& node : mNodes)
	{
		DirectX::XMMATRIX scale, rotate, translate, local;
		scale = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		rotate = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotate.x, node.rotate.y, node.rotate.z, node.rotate.w));
		translate = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		if (!lFlag)
		{
			DirectX::XMMATRIX C = DirectX::XMMatrixSet(
				-1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			);
			local = scale * rotate * translate*C;
			lFlag = true;
		}
		else
		{
			local = scale * rotate * translate;

		}
		DirectX::XMStoreFloat4x4(&node.localTransform, local);
	}
}

// ワールド変換行列計算
void Model::CalculateWorldTransform(const DirectX::XMMATRIX& world_transform)
{
	for (Node& node : mNodes)
	{
		node.beforeWorldTransform = node.worldTransform;
		if (node.parent != nullptr)
		{
			DirectX::XMMATRIX local_transform = DirectX::XMLoadFloat4x4(&node.localTransform);
			DirectX::XMMATRIX parent_world_transform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
			DirectX::XMStoreFloat4x4(&node.worldTransform, local_transform * parent_world_transform);
		}
		else
		{
			DirectX::XMMATRIX local_transform = DirectX::XMLoadFloat4x4(&node.localTransform);
			DirectX::XMStoreFloat4x4(&node.worldTransform, local_transform * world_transform);
		}
	}
}
