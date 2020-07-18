#pragma once
#include<d3d11.h>

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;
	virtual void Update(float elapsed_time) = 0;
	virtual void Render(ID3D11DeviceContext* context, float elapsed_time) = 0;
};