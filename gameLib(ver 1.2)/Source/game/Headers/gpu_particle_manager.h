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
	//パーティクルの生成
	void CreateTitleBuffer(ID3D11Device* device);
	void CreateSelectBuffer(ID3D11Device* device);
	void CreateGameBuffer(ID3D11Device* device,std::shared_ptr<PlayerAI>player);
	void CreateResultBuffer(ID3D11Device* device);
	//解放
	void ClearBuffer();
	//更新
	void Update(float elapsd_time);
	//エディタ
	void Editor();
	//getter
	TitleTextureParticle* GetTitleTextureParticle() { return mTitleTextureParticle.get(); }
	TitleParticle* GetTitleParticle() { return mTitleParticle.get(); }
	RunParticles* GetRunParticle() { return mRunParticle.get(); }
	FireworksParticle* GetFireworksParticle() { return mFireworksParticle.get(); }
	//setter
	void SetState(const int state) { mState = state; }
	//描画
	void Render(ID3D11DeviceContext* context, bool drowMullti=false);
	void VelocityRender(ID3D11DeviceContext* context);
	//シーンのステート
	enum STATE
	{
		TITLE,
		SELECT,
		GAME,
		RESULT,
	};
private:
	//エディタ
	void TitleEditor();
	void SelectEditor();
	void GameEditor();
	void ResultEditor();
	//bufferの生成
	void CreateBuffer(ID3D11Device* device);
	//パーティクル変数
	std::unique_ptr<RunParticles>mRunParticle;
	std::unique_ptr<StageSceneParticle>mStageSceneParticle;
	std::unique_ptr<TitleParticle>mTitleParticle;
	std::unique_ptr<TitleTextureParticle>mTitleTextureParticle;
	std::unique_ptr<SelectSceneParticle>mSelectSceneParticle;
	std::unique_ptr<FireworksParticle>mFireworksParticle;
	//定数バッファ
	struct CbTimer
	{
		float elapsdTime;
		VECTOR3F dummy;
	};
	std::unique_ptr<ConstantBuffer<CbTimer>>mCbTimer;
	//シェーダー
	std::unique_ptr<DrowShader>mSSceneShader;
	std::unique_ptr<DrowShader>mSObjShader;
	std::unique_ptr<DrowShader>mRunShader;
	//シーンの状態
	int mState;
};
#define pGpuParticleManager (GpuParticleManager::GetInctance())