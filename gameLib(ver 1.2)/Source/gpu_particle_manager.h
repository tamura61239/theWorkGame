#pragma once
#include"stage_obj_particle.h"
#include"stage_obj.h"
#include"run_particle.h"
#include"stage_scene_particle.h"
#include"title_particle.h"
#include"title_texture_particle.h"
#include"select_scene_particle.h"
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
	void Update(float elapsd_time);
	void ImGuiUpdate();
	//getter
	TitleTextureParticle* GetTitleTextureParticle() { return mTitleTextureParticle.get(); }
	TitleParticle* GetTitleParticle() { return mTitleParticle.get(); }
	RunParticles* GetRunParticle() { return mRunParticle.get(); }
	//setter
	void SetState(const int state) { mState = state; }
	//描画
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, bool drowMullti=false);
	void RenderVelocity(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	enum STATE
	{
		TITLE,
		SELECT,
		GAME,
	};
private:
	void TitleImGui();
	void SelectImGui();
	void GameImGui();
	void CreateBuffer(ID3D11Device* device);
	GpuParticleManager(){}
	std::unique_ptr<StageObjParticle>mStageObjParticle;
	std::unique_ptr<RunParticles>mRunParticle;
	std::unique_ptr<StageSceneParticle>mStageSceneParticle;
	std::unique_ptr<TitleParticle>mTitleParticle;
	std::unique_ptr<TitleTextureParticle>mTitleTextureParticle;
	std::unique_ptr<SelectSceneParticle>mSelectSceneParticle;
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
	int mState;
};
#define pGpuParticleManager (GpuParticleManager::GetInctance())