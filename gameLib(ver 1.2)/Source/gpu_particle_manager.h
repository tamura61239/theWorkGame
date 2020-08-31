#pragma once
#include"stage_obj_particle.h"
#include"stage_obj.h"
#include"run_particle.h"
#include<memory>

class GpuParticleManager
{
public:
	static GpuParticleManager& GetInctance()
	{
		static GpuParticleManager manager;
		return manager;
	}
	void CreateBuffer(ID3D11Device* device);
	void ClearBuffer();
	//パーティクル生成関数
	void CreateStageObjParticle(std::vector<std::shared_ptr<StageObj>>objs);
	//更新
	void Update(float elapsd_time,float colorType,const VECTOR3F&velocity,const VECTOR3F&position,const bool groundFlag);
	//描画
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
private:
	GpuParticleManager(){}
	std::unique_ptr<StageObjParticle>mStageObjParticle;
	std::unique_ptr<RunParticles>mRunParticle;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepth;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbScene;
	struct CbScene
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
};
#define pGpuParticleManager (GpuParticleManager::GetInctance())