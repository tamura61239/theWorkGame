#include "scene_select.h"
#include"camera_manager.h"
#include"screen_size.h"
#include"gpu_particle_manager.h"
#include"scene_manager.h"
#include"scene_title.h"
#include"scene_game.h"
#include"scene_result.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�
/*****************************************************/
void SceneSelect::Initialize(ID3D11Device* device)
{
	//�}���`�X���b�h
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);


			mSelect = std::make_unique<StageSelect>(device, StageManager::GetMaxStageCount());
			pCameraManager->Initialize(device, 2);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));

			mColorMap = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			mSceneFrame = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			GpuParticleManager::Create();

			pGpuParticleManager->CreateSelectBuffer(device);

			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1);
		
			mPhotographTargets.resize(2);
			//memset(&mPhotographTargets[0], 0, sizeof(bool) * mPhotographTargets.size());
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::SELECT);
		}, device);
	//NowLoading�̎��ɕ`�悷�����
	mRenderTexture = std::make_unique<Sprite>(device, L"Data/image/now.png");
	
	//�`��p�̃X�e�[�g�̐���
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP));
	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void SceneSelect::Editor()
{
	//�}���`�X���b�h�̏������I��������ǂ����𒲂ׂ�(�I����ĂȂ�������return)
	if (IsNowLoading())
	{
		return;
	}
	bool beforeEditorFlag = mEditorFlag;
	switch (pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount()))
	{
	case SceneManager::SCENETYPE::TITLE:
		pSceneManager.ChangeScene(new SceneTitle);
		break;
	case SceneManager::SCENETYPE::GAME:
		pSceneManager.ChangeScene(new SceneGame(pSceneManager.GetSceneEditor()->GetStageNo()));
		break;
	case SceneManager::SCENETYPE::RESULT:
		pSceneManager.ChangeScene(new SceneResult(0.f,0));
		break;
	}
	//�G�f�B�^�[��OFF�̎�
	if (!mEditorFlag)return;
	ImGui::Begin("scene select");
#ifdef _DEBUG
	//�V�[�����摜�Ƃ��ĕۑ����邩�ǂ�����I������
	if (ImGui::CollapsingHeader("screen shot"))
	{
		ImGui::Selectable("gpu particle", &mPhotographTargets[0]);
		ImGui::Selectable("stage board", &mPhotographTargets[1]);
		if (ImGui::Button("photograph"))
		{
			mScreenShot = true;
		}
	}
#endif
	//�ǂ̃G�f�B�^�[�𑀍삷�邩�I������
	ImGui::RadioButton("NONE", &mEditorNo, 0);
	ImGui::RadioButton("SELECT_SCENE", &mEditorNo, 1);
	ImGui::RadioButton("BLOOM", &mEditorNo, 2);
	ImGui::RadioButton("CAMERA", &mEditorNo, 3);
	ImGui::RadioButton("FADE", &mEditorNo, 4);
	ImGui::RadioButton("GPU PARTICLE", &mEditorNo, 5);

	ImGui::End();
	//�I�����ꂽ�G�f�B�^�֐����Ă�
	switch (mEditorNo)
	{
	case 1:
		mSelect->Editor();
		break;
	case 2:
		mBloom->Editor();
		break;
	case 3:
		pCameraManager->Editor();
		break;
	case 4:
		mFade->Editor();
		break;
	case 5:
		pGpuParticleManager->Editor();
		break;
	}
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
void SceneSelect::Update(float elapsed_time)
{
	//�}���`�X���b�h�̏������I��������ǂ����𒲂ׂ�(�I����ĂȂ�������return)
	if (IsNowLoading())
	{
		return;
	}
	//�}���`�X���b�h�̏I���֐�
	EndLoading();

	if (mSelect->Update(elapsed_time))
	{//�t�F�[�h�A�E�g�J�n
		mFade->StartFadeOut();
	}
	//�t�F�[�g�̍X�V
	mFade->Update(elapsed_time);
	//�p�[�e�B�N���̍X�V
	pGpuParticleManager->Update(elapsed_time);
	//�t�F�[�h�A�E�g���I��������ǂ���
	if (mFade->GetEndFlag())
	{
		if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
		{
			//�I�������X�e�[�W�ɑJ��
			pSceneManager.ChangeScene(new SceneGame(mSelect->GetSelectNumber()));

		}
	}
	//�J�����̍X�V
	pCameraManager->Update(elapsed_time);
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
void SceneSelect::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//�`��p�̃X�e�[�g�̐ݒ�
	mSampler[0]->Activate(context, 0, false, true);
	mDepth->Activate(context);
	mRasterizer->Activate(context);
	mBlend[0]->activate(context);
	

	if (IsNowLoading())
	{//NowLoading��ʂ̎�
		mRenderTexture->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, LoadColor(elapsed_time));
		//�`��p�̃X�e�[�g�̂̉���
		mSampler[0]->DeActivate(context);
		mDepth->DeActive(context);
		mRasterizer->DeActivate(context);
		mBlend[0]->deactivate(context);
		return;
	}
	/**************�J���[�}�b�v*************/
	
	mColorMap->Clear(context);
	mColorMap->Activate(context);


	pGpuParticleManager->Render(context, pCameraManager->GetCamera()->GetView(), pCameraManager->GetCamera()->GetProjection());
	mSelect->Render(context);

	
	mColorMap->Deactivate(context);

	mBlend[0]->deactivate(context);

	/****************�u���[��***************/	
	mBlend[1]->activate(context);
	//�u���[���̏���
	mBloom->BlurTexture(context, mColorMap->GetRenderTargetShaderResourceView().Get());
	//�u���[����������
	mSceneFrame->Clear(context);
	mSceneFrame->Activate(context);
	mRenderTexture->Render(context, mColorMap->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, mColorMap->GetRenderTargetShaderResourceView().Get());
	mSceneFrame->Deactivate(context);
	mBlend[1]->deactivate(context);

	//�u���[�����������V�[�����t�����g�o�b�t�@�ɕ`�悷��
	mBlend[0]->activate(context);

	mRenderTexture->Render(context, mSceneFrame->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	mFade->Render(context);
	//�`��p�̃X�e�[�g�̂̉���
	mSampler[0]->DeActivate(context);
	mDepth->DeActive(context);
	mRasterizer->DeActivate(context);
	mBlend[0]->deactivate(context);

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@����֐�
/*****************************************************/
void SceneSelect::Relese()
{
	//�V�[���Ɏg�����f�[�^�̉��
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
}
