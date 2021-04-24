#include "scene_title.h"
#include"key_board.h"
#include"scene_manager.h"
#include"gpu_particle_manager.h"
#include"camera_manager.h"
#include"light.h"
#include"ui_manager.h"
#include"stage_manager.h"
#include"screen_size.h"
#include"scene_game.h"
#include"scene_result.h"
#include"scene_select.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�
/*****************************************************/
/**************************�R���X�g���N�^***************************/

SceneTitle::SceneTitle() : mTestMove(false)
{
}
/***********************������************************/
void SceneTitle::Initialize(ID3D11Device* device)
{
	//�}���`�X���b�h�ŃN���X�ϐ��̐���
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			//�}���`�X���b�h�̏I���ɕK�v�ȕϐ��̐���
			std::lock_guard<std::mutex> lock(loading_mutex);
			//�I�u�W�F�N�g�̐���
			GpuParticleManager::Create();
			pGpuParticleManager->CreateTitleBuffer(device);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::TITLE);
			UIManager::Create();
			UIManager::GetInctance()->TitleInitialize(device);
			//�J�����̐���
			pCameraManager->Initialize(device, 0);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));
			pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::TITLE_CAMERA);
			//�`��֘A�̐���
			bloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0);
			frameBuffer[0] = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer[1] = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer[2] = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			pLight.CreateLightBuffer(device);

			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::TITLE);
			mFade->StartFadeIn();
			D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

			};
			mBluer = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/zoom_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
			mCbZoomBuffer = std::make_unique<ConstantBuffer<CbZoom>>(device);
			{
				//�t�@�C������
				FILE* fp;
				if (fopen_s(&fp, "Data/file/title_zoom_blur_parameter.bin", "rb") == 0)
				{
					fread(&mCbZoomBuffer->data, sizeof(mCbZoomBuffer->data), 1, fp);
					fclose(fp);
				}
			}
		}, device);
	//NowLoading���Ɏg���ϐ��̐���
	mLoading = true;
	renderFlag = false;
	test = std::make_unique<Sprite>(device/*, L"Data/image/change_color.png"*/);
	//�`��p�̃X�e�[�g�̐���
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mSampler.push_back(std::make_unique<SamplerState>(device));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);
	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void SceneTitle::Editor()
{
#ifdef USE_IMGUI
	if (mLoading)
	{
		return;
	}
	//�V�[���G�f�B�^�[���g���ăV�[���J�ڂ���
	int newtScene = pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount());
	switch (newtScene)
	{
	case 2:
		pSceneManager.ChangeScene(new SceneSelect);
		break;
	case 3:
		pSceneManager.ChangeScene(new SceneGame(pSceneManager.GetSceneEditor()->GetStageNo()));
		return;
		break;
	case 4:
		pSceneManager.ChangeScene(new SceneResult(0.f, 0));
		return;
		break;
	}
	//�G�f�B�^�[��OFF�̎�
	if (!mEditorFlag)return;
	ImGui::Begin("scene title");
	ImGui::Checkbox("stop", &mStopTime);
	ImGui::SliderFloat("time", &mElapsdTimeSpeed, 0, 1);
	static int editorNum = 1;
#ifdef _DEBUG
	//�V�[�����摜�Ƃ��ĕۑ����邩�ǂ�����I������
	if (ImGui::CollapsingHeader("screen shot"))
	{
		ImGui::InputInt("No", &mTextureNo, 1);
		if (ImGui::Button("photograph"))
		{
			mScreenShot = true;
		}
	}
#endif
	//�ǂ̃G�f�B�^�[�𑀍삷�邩�I������
	ImGui::RadioButton("LIGHT", &editorNum, 0);
	ImGui::RadioButton("UI", &editorNum, 1);
	ImGui::RadioButton("GPU PARTICLE", &editorNum, 2);
	ImGui::RadioButton("CAMERA", &editorNum, 3);
	ImGui::RadioButton("BLOOM", &editorNum, 4);
	ImGui::RadioButton("FADE", &editorNum, 5);
	ImGui::RadioButton("ZOOM BLUR", &editorNum, 6);
	ImGui::End();
	//�I�����ꂽ�G�f�B�^�֐����Ă�
	switch (editorNum)
	{
	case 0:
		pLight.Editor();
		break;
	case 1:
		UIManager::GetInctance()->Editor();
		break;
	case 2:
		pGpuParticleManager->Editor();
		break;
	case 3:
		pCameraManager->Editor();
		break;
	case 4:
		bloom->Editor();
		break;
	case 5:
		mFade->Editor();
		break;

	}
	if (editorNum == 6)
	{
		//�Y�[���u���[�̃p�����[�^�[�𑀍삷��
		ImGui::Begin("zoom blur");
		ImGui::InputFloat("length", &mCbZoomBuffer->data.lenght, 0.1f);
		ImGui::InputInt("division", &mCbZoomBuffer->data.division, 1);
		//�Z�[�u
		if (ImGui::Button("save"))
		{
			FILE* fp;
			fopen_s(&fp, "Data/file/title_zoom_blur_parameter.bin", "wb");
			fwrite(&mCbZoomBuffer->data, sizeof(mCbZoomBuffer->data), 1, fp);
			fclose(fp);

		}
		ImGui::End();

	}
#endif

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
void SceneTitle::Update(float elapsed_time)
{
	//�}���`�X���b�h�̏������I��������ǂ����𒲂ׂ�
	if (IsNowLoading() || !renderFlag)
	{
		return;
	}
	//�}���`�X���b�h�̏I������
	mLoading = false;
	EndLoading();
	//�V�[�����~�߂�
	if (mStopTime) elapsed_time = 0;
	//�V�[���̐i�s���x
	elapsed_time *= mElapsdTimeSpeed;
	//�t�F�[�h�̍X�V
	mFade->Update(elapsed_time);
	//UI�̍X�V
	UIManager::GetInctance()->Update(elapsed_time);
	//�J�����̍X�V
	pCameraManager->Update(elapsed_time);
	//�p�[�e�B�N���̍X�V
	pGpuParticleManager->GetTitleParticle()->SetChangeFlag(pCameraManager->GetCameraOperation()->GetTitleCamera()->GetTitleSceneChangeFlag());
	pGpuParticleManager->Update(elapsed_time);
	if (pCameraManager->GetCameraOperation()->GetTitleCamera()->GetEndTitleFlag())
	{
		//�V�[����J�ڂ���
		pSceneManager.ChangeScene(new SceneSelect);
		return;
	}
	//�V�[���J�ڂ̏������J�n����
	if (UIManager::GetInctance()->GetTitleUIMove()->GetMoveChangeFlag())
	{
		if (pKeyBoad.RisingState(KeyLabel::SPACE))
		{
			UIManager::GetInctance()->GetTitleUIMove()->SetMoveChangeFlag(false);
			pCameraManager->GetCameraOperation()->GetTitleCamera()->SetTitleSceneChangeFlag(true);
			pGpuParticleManager->GetTitleTextureParticle()->SetFullDrowFlag(true);
			UIManager::GetInctance()->ClearUI();
		}
	}
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
void SceneTitle::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//NowLoading�����ǂ���
	if (mLoading)
	{
		renderFlag = true;
		return;
	}
	//�`��p�̃X�e�[�g�̐ݒ�
	mRasterizer->Activate(context);
	mDepth->Activate(context);
	mSampler[samplerType::wrap]->Activate(context, 0, true, true, true);
	mSampler[samplerType::clamp]->Activate(context, 2, true, true, true);
	/************************�J���[�}�b�v�e�N�X�`���̍쐬***********************/
	frameBuffer[0]->Clear(context);
	frameBuffer[0]->Activate(context);
	//view projection�s��̎擾
	//�V�[���̃I�u�W�F�N�g�̕`��
	mBlend[1]->activate(context);
	pCameraManager->GetCamera()->NowActive(context, 0, true, true, true);
	pGpuParticleManager->Render(context);
	pCameraManager->GetCamera()->NowDactive(context);
	if (!mScreenShot && pGpuParticleManager->GetTitleTextureParticle()->GetTextuteFlag())UIManager::GetInctance()->Render(context);
	mBlend[1]->deactivate(context);
	frameBuffer[0]->Deactivate(context);
	mBlend[0]->activate(context);
	/******************************�u���[��************************/
	 //�u���[���̏���
	bloom->BlurTexture(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get());
	//�V�[���Ƀu���[����������
	frameBuffer[1]->Clear(context);
	frameBuffer[1]->Activate(context);
	test->Render(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get());
	frameBuffer[1]->Deactivate(context);
	/******************************�Y�[���u���[************************/
	frameBuffer[2]->Clear(context);
	frameBuffer[2]->Activate(context);
	if (pCameraManager->GetCameraOperation()->GetTitleCamera()->GetMoveFlag())
	{//�Y�[���u���[��������
		mCbZoomBuffer->Activate(context, 0, true, true);
		test->Render(context, mBluer.get(), frameBuffer[1]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		mCbZoomBuffer->DeActivate(context);
	}
	else
	{//�Y�[���u���[�������Ȃ�
		test->Render(context, frameBuffer[1]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	}
	frameBuffer[2]->Deactivate(context);
	//�|�X�g�G�t�F�N�g���������V�[�����t�����g�o�b�t�@�ɕ`��
	test->Render(context, frameBuffer[2]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	//�`�挋�ʂ�ۑ�
	if (mScreenShot)
	{
		std::wstring fileName = L"Data/image/screen_shot/screenShot" + std::to_wstring(mTextureNo) + L".dds";
		frameBuffer[2]->SaveDDSFile(context, fileName.c_str(), frameBuffer[2]->GetRenderTargetShaderResourceView().Get());

	}
	mFade->Render(context);
	//�`��p�̃X�e�[�g�̉���
	mBlend[0]->deactivate(context);
	mRasterizer->DeActivate(context);
	mDepth->DeActive(context);
	mSampler[samplerType::wrap]->DeActivate(context);
	mSampler[samplerType::clamp]->DeActivate(context);
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@����֐�
/*****************************************************/
void SceneTitle::Relese()
{
	//�V�[���Ɏg�����f�[�^�̉��
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
	UIManager::GetInctance()->ClearUI();

}

