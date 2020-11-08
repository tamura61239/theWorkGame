#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"stage_obj.h"
#include"drow_shader.h"

class StageObjParticle
{
public:
	StageObjParticle(ID3D11Device* device);
	//setter
	void SetParticleData(const int colorType);
	void SetStageData( std::vector<std::shared_ptr<StageObj>>objs);

	void ImGuiUpdate();
	void Update(ID3D11DeviceContext* context, float elapsd_time);
	void Render(ID3D11DeviceContext* context);
private:
	void Load();
	void Save();
private:
};