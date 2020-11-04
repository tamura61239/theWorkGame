#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"stage_obj.h"

class StageObjParticle
{
public:
	StageObjParticle(ID3D11Device* device);
	void ImGuiUpdate(float elapsdTime);
	void Update(ID3D11DeviceContext* context, float elapsd_time, const int colorState);
	void Render(ID3D11DeviceContext* context);
private:
};