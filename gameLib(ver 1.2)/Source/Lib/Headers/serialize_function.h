#pragma once
#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>
#include<cereal/types/memory.hpp>
#include<cereal/types/vector.hpp>
#include<cereal/archives/json.hpp>
//シリアライズ関数
template<class Archive>
void serialize(Archive& archive, VECTOR2F& vector)
{
	archive(vector.x, vector.y);
}

template<class Archive>
void serialize(Archive& archive, VECTOR3F& vector)
{
	archive(vector.x, vector.y, vector.z);
}

template<class Archive>
void serialize(Archive& archive, UVECTOR4& vector)
{
	archive(vector.x, vector.y, vector.z, vector.w);
}

template<class Archive>
void serialize(Archive& archive, VECTOR4F& vector)
{
	archive(vector.x, vector.y, vector.z, vector.w);
}

template<class Archive>
void serialize(Archive& archive, FLOAT4X4& vector)
{
	archive(
		vector._11, vector._12, vector._13, vector._14,
		vector._21, vector._22, vector._23, vector._24,
		vector._31, vector._32, vector._33, vector._34,
		vector._41, vector._42, vector._43, vector._44
	);
}
