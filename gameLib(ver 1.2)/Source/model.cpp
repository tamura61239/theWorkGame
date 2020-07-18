#include "logger.h"
#include "model.h"
#include "misc.h"

Model::Model(std::shared_ptr<ModelResource>& resource)
{
	m_model_resource = resource;

	// ノード
	const std::vector<ModelData::Node>& res_nodes = resource->GetNodes();

	m_nodes.resize(res_nodes.size());
	for (size_t node_index = 0; node_index < m_nodes.size(); ++node_index)
	{
		auto&& src = res_nodes.at(node_index);
		auto&& dst = m_nodes.at(node_index);

		dst.name = src.name.c_str();
		dst.parent = src.parent_index >= 0 ? &m_nodes.at(src.parent_index) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;
	}
}

// アニメーション再生
void Model::PlayAnimation(int animation_index, bool loop)
{
	m_current_animation = animation_index;
	m_loop_animation = loop;
	m_end_animation = false;
	m_current_seconds = 0.0f;
}

// アニメーション計算
void Model::UpdateAnimation(float elapsed_time)
{
	if (m_current_animation < 0)
	{
		return;
	}

	if (m_model_resource->GetAnimations().empty())
	{
		return;
	}

	const ModelData::Animation& animation = m_model_resource->GetAnimations().at(m_current_animation);

	const std::vector<ModelData::Keyframe>& keyframes = animation.keyframes;
	int key_count = static_cast<int>(keyframes.size());
	for (int key_index = 0; key_index < key_count - 1; ++key_index)
	{
		// 現在の時間がどのキーフレームの間にいるか判定する
		const ModelData::Keyframe& keyframe0 = keyframes.at(key_index);
		const ModelData::Keyframe& keyframe1 = keyframes.at(key_index + 1);
		if (m_current_seconds >= keyframe0.seconds && m_current_seconds < keyframe1.seconds)
		{
			float rate = (m_current_seconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

			assert(m_nodes.size() == keyframe0.node_keys.size());
			assert(m_nodes.size() == keyframe1.node_keys.size());
			int node_count = static_cast<int>(m_nodes.size());
			for (int node_index = 0; node_index < node_count; ++node_index)
			{
				// ２つのキーフレーム間の補完計算
				const ModelData::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
				const ModelData::NodeKeyData& key1 = keyframe1.node_keys.at(node_index);

				Node& node = m_nodes[node_index];

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
	if (m_end_animation)
	{
		m_end_animation = false;
		m_current_animation = -1;
		return;
	}

	// 時間経過
	m_current_seconds += elapsed_time;
	if (m_current_seconds >= animation.seconds_length)
	{
		if (m_loop_animation)
		{
			m_current_seconds -= animation.seconds_length;
		}
		else
		{
			m_current_seconds = animation.seconds_length;
			m_end_animation = true;
		}
	}
}

// ローカル変換行列計算
void Model::CalculateLocalTransform()
{
	for (Node& node : m_nodes)
	{
		DirectX::XMMATRIX scale, rotate, translate, local;
		scale = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		rotate = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotate.x, node.rotate.y, node.rotate.z, node.rotate.w));
		translate = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);

		local = scale * rotate * translate;
		DirectX::XMStoreFloat4x4(&node.local_transform, local);
	}
}

// ワールド変換行列計算
void Model::CalculateWorldTransform(const DirectX::XMMATRIX& world_transform)
{
	for (Node& node : m_nodes)
	{
		if (node.parent != nullptr)
		{
			DirectX::XMMATRIX local_transform = DirectX::XMLoadFloat4x4(&node.local_transform);
			DirectX::XMMATRIX parent_world_transform = DirectX::XMLoadFloat4x4(&node.parent->world_transform);
			DirectX::XMStoreFloat4x4(&node.world_transform, local_transform * parent_world_transform);
		}
		else
		{
			DirectX::XMMATRIX local_transform = DirectX::XMLoadFloat4x4(&node.local_transform);
			DirectX::XMStoreFloat4x4(&node.world_transform, local_transform * world_transform);
		}
	}
}
