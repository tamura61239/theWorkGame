#pragma once
#include <string>
#include <vector>
#include"vector.h"
#include"serialize_function.h"
//モデルのデータクラス	
struct ModelData
{
	//ボーン(骨)のデータ
	struct Node
	{
		std::string			name;
		int					parentIndex;
		VECTOR3F	scale;
		VECTOR4F	rotate;
		VECTOR3F	translate;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				name,
				parentIndex,
				scale,
				rotate,
				translate
			);
		}
	};
	//頂点データ
	struct Vertex
	{
		VECTOR3F	position;
		VECTOR3F	normal;
		VECTOR2F	texcoord;
		VECTOR4F	boneWeight;
		UVECTOR4	boneIndex;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				position,
				normal,
				texcoord,
				boneWeight,
				boneIndex
			);
		}

	};
	//マテリアルごとにメッシュを区切る
	struct Subset
	{
		int					materialIndex;
		int					startIndex;
		int					indexCount;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				materialIndex,
				startIndex,
				indexCount
			);
		}

	};
	//メッシュデータ
	struct Mesh
	{
		std::vector<Vertex>	vertices;
		std::vector<int>	indices;
		std::vector<Subset>	subsets;

		int					nodeIndex;

		std::vector<int>					nodeIndices;
		std::vector<FLOAT4X4>	inverseTransforms;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				vertices,
				indices,
				subsets,
				nodeIndex,
				nodeIndices,
				inverseTransforms
			);
		}

	};
	//マテリアルデータ
	struct Material
	{
		VECTOR4F	color;
		std::string			textureFilename;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				color.x, color.y, color.z, color.w,
				textureFilename
			);
		}

	};
	//キーフレームのノードデータ
	struct NodeKeyData
	{
		VECTOR3F	scale;
		VECTOR4F	rotate;
		VECTOR3F	translate;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				scale,
				rotate,
				translate
			);
		}

	};
	//キーフレーム
	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	nodeKeys;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				seconds,
				nodeKeys
			);
		}

	};
	//アニメーション
	struct Animation
	{
		float						secondsLength;
		std::vector<Keyframe>		keyframes;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				secondsLength,
				keyframes
			);
		}

	};

	std::vector<Node>		nodes;

	std::vector<Mesh>		meshes;
	std::vector<Material>	diffuses;
	std::vector<Material>	normals;
	std::vector<Material>	bumps;

	std::vector<Animation>	animations;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			nodes,
			meshes,
			diffuses,
			normals,
			bumps,
			animations
		);
	}
	//コンストラクタ
	ModelData(const char* filename,const bool textureFlag=false, int filetype = 0);
};