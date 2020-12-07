#pragma once
#include"stage_obj.h"
#include"run_particle.h"
#include"stage_scene_particle.h"
#include"title_particle.h"
#include"title_texture_particle.h"
#include"select_scene_particle.h"
#include"singleton_class.h"
#include"fireworks_particle.h"
#include<memory>

class GpuParticleManager:public Singleton<GpuParticleManager>
{
public:
	void CreateTitleBuffer(ID3D11Device* device);
	void CreateGameBuffer(ID3D11Device* device);
	void CreateResultBuffer(ID3D11Device* device);
	void ClearBuffer();
	//�X�V
	void Update(float elapsd_time);
	void ImGuiUpdate();
	//getter
	TitleTextureParticle* GetTitleTextureParticle() { return mTitleTextureParticle.get(); }
	TitleParticle* GetTitleParticle() { return mTitleParticle.get(); }
	RunParticles* GetRunParticle() { return mRunParticle.get(); }
	FireworksParticle* GetFireworksParticle() { return mFireworksParticle.get(); }
	//setter
	void SetState(const int state) { mState = state; }
	//�`��
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, bool drowMullti=false);
	enum STATE
	{
		TITLE,
		SELECT,
		GAME,
		RESULT,
	};
private:
	void TitleImGui();
	void SelectImGui();
	void GameImGui();
	void ResultImGui();
	void CreateBuffer(ID3D11Device* device);
	std::unique_ptr<RunParticles>mRunParticle;
	std::unique_ptr<StageSceneParticle>mStageSceneParticle;
	std::unique_ptr<TitleParticle>mTitleParticle;
	std::unique_ptr<TitleTextureParticle>mTitleTextureParticle;
	std::unique_ptr<SelectSceneParticle>mSelectSceneParticle;
	std::unique_ptr<FireworksParticle>mFireworksParticle;
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