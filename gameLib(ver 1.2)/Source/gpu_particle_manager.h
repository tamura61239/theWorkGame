#pragma once
#include"stage_obj_particle.h"
#include"stage_obj.h"
#include"run_particle.h"
#include"stage_scene_particle.h"
#include"title_particle.h"
#include<memory>

class GpuParticleManager
{
public:
	static GpuParticleManager& GetInctance()
	{
		static GpuParticleManager manager;
		return manager;
	}
	void CreateTitleBuffer(ID3D11Device* device);
	void CreateGameBuffer(ID3D11Device* device);
	void ClearBuffer();
	//パーティクル生成関数
	void CreateStageObjParticle(std::vector<std::shared_ptr<StageObj>>objs);
	//更新
	void Update(float elapsd_time,float colorType=0,const VECTOR3F&velocity=VECTOR3F(0,0,0),const VECTOR3F&position = VECTOR3F(0, 0, 0),const bool groundFlag=false);
	//描画
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, bool drowMullti=false);
	void RenderVelocity(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
private:
	void CreateBuffer(ID3D11Device* device);
	GpuParticleManager(){}
	std::unique_ptr<StageObjParticle>mStageObjParticle;
	std::unique_ptr<RunParticles>mRunParticle;
	std::unique_ptr<StageSceneParticle>mStageSceneParticle;
	std::unique_ptr<TitleParticle>mTitleParticle;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepth;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbScene;
	struct CbScene
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	std::unique_ptr<DrowShader>mSSceneShader;
	std::unique_ptr<DrowShader>mSObjShader;
	std::unique_ptr<DrowShader>mRunShader;
};
#define pGpuParticleManager (GpuParticleManager::GetInctance())