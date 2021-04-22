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
	//�p�[�e�B�N���̐���
	void CreateTitleBuffer(ID3D11Device* device);
	void CreateSelectBuffer(ID3D11Device* device);
	void CreateGameBuffer(ID3D11Device* device,std::shared_ptr<PlayerAI>player);
	void CreateResultBuffer(ID3D11Device* device);
	//���
	void ClearBuffer();
	//�X�V
	void Update(float elapsd_time);
	//�G�f�B�^
	void Editor();
	//getter
	TitleTextureParticle* GetTitleTextureParticle() { return mTitleTextureParticle.get(); }
	TitleParticle* GetTitleParticle() { return mTitleParticle.get(); }
	RunParticles* GetRunParticle() { return mRunParticle.get(); }
	FireworksParticle* GetFireworksParticle() { return mFireworksParticle.get(); }
	//setter
	void SetState(const int state) { mState = state; }
	//�`��
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, bool drowMullti=false);
	void VelocityRender(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	//�V�[���̃X�e�[�g
	enum STATE
	{
		TITLE,
		SELECT,
		GAME,
		RESULT,
	};
private:
	//�G�f�B�^
	void TitleEditor();
	void SelectEditor();
	void GameEditor();
	void ResultEditor();
	//buffer�̐���
	void CreateBuffer(ID3D11Device* device);
	//�p�[�e�B�N���ϐ�
	std::unique_ptr<RunParticles>mRunParticle;
	std::unique_ptr<StageSceneParticle>mStageSceneParticle;
	std::unique_ptr<TitleParticle>mTitleParticle;
	std::unique_ptr<TitleTextureParticle>mTitleTextureParticle;
	std::unique_ptr<SelectSceneParticle>mSelectSceneParticle;
	std::unique_ptr<FireworksParticle>mFireworksParticle;
	//�萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbScene;
	struct CbScene
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	//�V�F�[�_�[
	std::unique_ptr<DrowShader>mSSceneShader;
	std::unique_ptr<DrowShader>mSObjShader;
	std::unique_ptr<DrowShader>mRunShader;
	//�V�[���̏��
	int mState;
};
#define pGpuParticleManager (GpuParticleManager::GetInctance())