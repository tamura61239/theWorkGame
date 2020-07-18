#pragma once
#include <string>
#include <vector>
#include"vector.h"
#include"serialize_function.h"
struct ModelData
{
	struct Node
	{
		std::string			name;
		int					parent_index;
		VECTOR3F	scale;
		VECTOR4F	rotate;
		VECTOR3F	translate;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				name,
				parent_index,
				scale,
				rotate,
				translate
			);
		}
	};

	struct Vertex
	{
		VECTOR3F	position;
		VECTOR3F	normal;
		VECTOR2F	texcoord;
		VECTOR4F	bone_weight;
		UVECTOR4	bone_index;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				position,
				normal,
				texcoord,
				bone_weight,
				bone_index
			);
		}

	};

	struct Subset
	{
		int					material_index;
		int					start_index;
		int					index_count;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				material_index,
				start_index,
				index_count
			);
		}

	};

	struct Mesh
	{
		std::vector<Vertex>	vertices;
		std::vector<int>	indices;
		std::vector<Subset>	subsets;

		int					node_index;

		std::vector<int>					node_indices;
		std::vector<FLOAT4X4>	inverse_transforms;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				vertices,
				indices,
				subsets,
				node_index,
				node_indices,
				inverse_transforms
			);
		}

	};

	struct Material
	{
		VECTOR4F	color;
		std::string			texture_filename;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				color.x, color.y, color.z, color.w,
				texture_filename
			);
		}

	};

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

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	node_keys;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				seconds,
				node_keys
			);
		}

	};
	struct Animation
	{
		float						seconds_length;
		std::vector<Keyframe>		keyframes;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				seconds_length,
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
	ModelData(const char* filename,const bool textureFlag, int filetype = 0);
};