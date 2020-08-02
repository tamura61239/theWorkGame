#pragma once
#include"geometric_primitive.h"
#include<memory>

class HitAreaDrow
{
public:
	static HitAreaDrow& GetInctance()
	{
		static HitAreaDrow inctance;
		return inctance;
	}
	void CreateObj(ID3D11Device* device);
	void SetCube(const VECTOR3F& min, const VECTOR3F& max);
	void SetSphere(const VECTOR3F& position, const float radius);
	void Update();
	void Render(ID3D11DeviceContext* context, const VECTOR4F& light, const FLOAT4X4& view, const FLOAT4X4& projection);
private:
	std::vector<std::unique_ptr<GeometricPrimitive>>mObjs;
	std::unique_ptr<PrimitiveRender>render;
	HitAreaDrow(){}
	struct VERTEX
	{
		VECTOR3F scale;
		VECTOR3F position;
		FLOAT4X4 world;
		int type;
		void CalculateTransform()
		{
			DirectX::XMMATRIX W, s, r, t;
			//スケール行列
			s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			//回転行列
			r = DirectX::XMMatrixRotationRollPitchYaw(0,0,0);
			//移動行列
			t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			//ワールド行列
			W = s * r * t;
			DirectX::XMStoreFloat4x4(&world, W);
		}
	};
	std::vector< VERTEX>vertexs;
};
#define pHitAreaDrow (HitAreaDrow::GetInctance())