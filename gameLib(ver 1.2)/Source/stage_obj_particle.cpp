#include "stage_obj_particle.h"
#include"misc.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
StageObjParticle::StageObjParticle(ID3D11Device* device) 
{
}

void StageObjParticle::SetParticleData(const int colorType)
{

}

void StageObjParticle::SetStageData(std::vector<std::shared_ptr<StageObj>> objs)
{
}


void StageObjParticle::ImGuiUpdate()
{
}

void StageObjParticle::Update(ID3D11DeviceContext* context, float elapsd_time)
{

}

void StageObjParticle::Render(ID3D11DeviceContext* context)
{
}

void StageObjParticle::Load()
{
}

void StageObjParticle::Save()
{

}


