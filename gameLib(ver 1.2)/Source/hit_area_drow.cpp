#include "hit_area_drow.h"

void HitAreaDrow::CreateObj(ID3D11Device* device)
{
	mObjs.push_back(std::make_unique<GeometricCube>(device));
	mObjs.push_back(std::make_unique<GeometricSphere>(device,32,16));
	render = std::make_unique<PrimitiveRender>(device);
}

void HitAreaDrow::SetCube(const VECTOR3F& min, const VECTOR3F& max)
{
	VERTEX vertex;
	vertex.scale = max - min;
	vertex.position = min + vertex.scale;
	vertex.type = 0;
	vertexs.push_back(vertex);
}

void HitAreaDrow::SetSphere(const VECTOR3F& position, const float radius)
{
	VERTEX vertex;
	vertex.position = position;
	vertex.scale = VECTOR3F(radius, radius, radius);
	vertex.type = 1;
	vertexs.push_back(vertex);
}

void HitAreaDrow::Update()
{
	for (auto& vertex : vertexs)
	{
		vertex.CalculateTransform();
	}
}

void HitAreaDrow::Render(ID3D11DeviceContext* context, const VECTOR4F& light, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	render->Begin(context, light, view, projection);
	for (auto& vertex : vertexs)
	{
		render->Render(context, mObjs[vertex.type].get(), vertex.world);
	}
	render->End(context);
	if (vertexs.size() > 0)vertexs.clear();
}
