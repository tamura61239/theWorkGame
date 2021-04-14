#include "scene_result.h"
#include"ui_manager.h"
#include"scene_manager.h"
#include"camera_manager.h"
#include"gpu_particle_manager.h"
#include"stage_operation.h"
#include"light.h"
#include"screen_size.h"
#include"scene_title.h"
#include"scene_select.h"
#include"scene_game.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

SceneResult::SceneResult(float timer, int nowStageNo) :mNowGameTime(timer), mPlayFlag(true), nowLoading(true)
{
	Ranking::SetStageNo(nowStageNo);
}
/*******************************�������֐�*****************************/
void SceneResult::Initialize(ID3D11Device* device)
{
	//�}���`�X���b�h�ŃN���X�ϐ��̐���
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			//�}���`�X���b�h�̏I���ɕK�v�ȕϐ��̐���
			std::lock_guard<std::mutex> lock(loading_mutex);

			//�`�悷��I�u�W�F�N�g�̐���
			UIManager::Create();
			CameraManager::Create();
			pCameraManager->Initialize(device, 2);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));
			pCameraManager->Update(0);

			GpuParticleManager::Create();
			pGpuParticleManager->CreateResultBuffer(device);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::RESULT);

			UIManager::GetInctance()->ResultInitialize(device);
			mRanking = std::make_unique<Ranking>(device, mNowGameTime);
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::RESULT);

			//�`��p�ϐ��̐���
			frameBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer2 = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			velocityBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);

			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 3);
			sky = std::make_unique<SkyMap>(device, L"Data/AllSkyFree/Cold Night/ColdNight.dds", MAPTYPE::BOX);
			//�X�J�C�}�b�v�̒l���t�@�C������ǂݎ��
			{
				FILE* fp;
				if (fopen_s(&fp, "Data/file/result_sky_map.bin", "rb") == 0)
				{
					fread(sky->GetPosData(), sizeof(Obj3D), 1, fp);
					fclose(fp);
				}
			}
			//�X�J�C�}�b�v�̃��[���h���W�n�s����ŏ��Ɍv�Z���Ƃ�
			sky->GetPosData()->CalculateTransform();

		}, device);
	//NowLoading���Ɏg���ϐ�������ɐ���
	mRenderScene = std::make_unique<Sprite>(device, L"Data/image/now.png");
	renderFlag = false;
	//�`��p�̃X�e�[�g�̐���
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	mSampler.push_back(std::make_unique<SamplerState>(device));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);
	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);

}
/***********************�G�f�B�^�[�֐�(ImGui���g���ăp�����[�^�[�𒲐�����)*******************/
void SceneResult::Editor()
{
#ifdef USE_IMGUI
	//�}���`�X���b�h�̏������I��������ǂ����𒲂ׂ�(�I����ĂȂ�������return)
	if (IsNowLoading())
	{
		return;
	}
	//�V�[���G�f�B�^�[���g���ăV�[���J�ڂ���
	int nextScene = pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount());
	switch (nextScene)
	{
	case 1:
		UIManager::GetInctance()->Clear();
		UIManager::Destroy();
		pCameraManager->DestroyCamera();
		GpuParticleManager::Destroy();

		pSceneManager.ChangeScene(new SceneTitle);
		break;
	case 2:
		pSceneManager.ChangeScene(new SceneSelect);
		break;
	case 3:
		pSceneManager.ChangeScene(new SceneGame(pSceneManager.GetSceneEditor()->GetStageNo()));
		break;
	}
	if (nextScene <= 3 && nextScene >= 1)return;
	if (!mEditorFlag)return;
	/*******************Editor****************/
	ImGui::Begin("scene result");
#ifdef _DEBUG
	//�V�[�����摜�Ƃ��ĕۑ����邩�ǂ�����I������
	if (ImGui::CollapsingHeader("screen shot"))
	{
		ImGui::InputInt("No", &mTextureNo, 1);

		if (ImGui::Button("shot"))
		{
			mScreenShot = true;
		}
	}
#endif
	//�ǂ̃G�f�B�^�[�𑀍삷�邩�I������
	ImGui::RadioButton("NONE", &mEditorNo, 0);
	ImGui::RadioButton("RANKING", &mEditorNo, 1);
	ImGui::RadioButton("UI", &mEditorNo, 2);
	ImGui::RadioButton("FADE", &mEditorNo, 3);
	ImGui::RadioButton("BLOOM", &mEditorNo, 4);
	ImGui::RadioButton("CAMERA", &mEditorNo, 5);
	ImGui::RadioButton("SKY MAP", &mEditorNo, 6);
	ImGui::RadioButton("GPU PARTICLE", &mEditorNo, 7);

	ImGui::End();
	//�I�����ꂽ�G�f�B�^�֐����Ă�
	switch (mEditorNo)
	{
	case 1:
		mRanking->ImGuiUpdate();
		break;
	case 2:
		UIManager::GetInctance()->ImGuiUpdate();
		break;
	case 3:
		mFade->ImGuiUpdate();
		break;
	case 4:
		mBloom->ImGuiUpdate();
		break;
	case 5:
		pCameraManager->ImGuiUpdate();
		break;
	case 7:
		pGpuParticleManager->ImGuiUpdate();
		break;
	}
	if (mEditorNo == 6)
	{
		//�X�J�C�}�b�v�̃p�����[�^�[�𑀍삷��
		ImGui::Begin("sky map");
		//���S���W
		float* position[3] = { &sky->GetPosData()->GetPosition().x,&sky->GetPosData()->GetPosition().y ,&sky->GetPosData()->GetPosition().z };
		ImGui::DragFloat3("position", *position, 10);
		//�X�P�[��
		float* scale[3] = { &sky->GetPosData()->GetScale().x,&sky->GetPosData()->GetScale().y ,&sky->GetPosData()->GetScale().z };
		ImGui::DragFloat3("scale", *scale, 10);
		//�e���̉�]
		float* angle[3] = { &sky->GetPosData()->GetAngle().x,&sky->GetPosData()->GetAngle().y ,&sky->GetPosData()->GetAngle().z };
		ImGui::SliderFloat3("angle", *angle, -3.14f, 3.14f);
		//�F
		float* color[4] = { &sky->GetPosData()->GetColor().x,&sky->GetPosData()->GetColor().y ,&sky->GetPosData()->GetColor().z ,&sky->GetPosData()->GetColor().w };
		ImGui::ColorEdit4("color", *color);
		//�Z�[�u
		if (ImGui::Button("save"))
		{
			//�t�@�C������
			FILE* fp;
			fopen_s(&fp, "Data/file/result_sky_map.bin", "wb");
			fwrite(sky->GetPosData(), sizeof(Obj3D), 1, fp);
			fclose(fp);
		}
		ImGui::End();
	}

#endif

}
/**********************�X�V�֐�*******************************/
void SceneResult::Update(float elapsed_time)
{
	//�}���`�X���b�h�̏������I��������ǂ����𒲂ׂ�(�I����ĂȂ�������return)
	if (IsNowLoading())
	{
		return;
	}
	//�}���`�X���b�h�̏I���֐�
	EndLoading();
	//�t�F�[�h�̍X�V
	mFade->Update(elapsed_time);
	//�V�[���؂�ւ�
	if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
	{
		//�t�F�[�h�A�E�g���I���������ǂ���
		if (mFade->GetEndFlag())
		{
			//�t�F�[�h�A�E�g�I�����ɃV�[���J��
			int type = UIManager::GetInctance()->GetResultUIMove()->GetType();
			switch (type)
			{
			case 0://�O�Ƀv���C�����X�e�[�W��
				pSceneManager.ChangeScene(new SceneGame(Ranking::GetStageNo()));
				break;
			case 1://�^�C�g���V�[����
				pSceneManager.ChangeScene(new SceneTitle);
				break;
			case 2://�Z���N�g�V�[����
				pSceneManager.ChangeScene(new SceneSelect);
				break;
			case 3://���̃X�e�[�W��
				pSceneManager.ChangeScene(new SceneGame(Ranking::GetStageNo() + 1));
				break;
			}
			return;
		}
	}
	//�V�[���̃I�u�W�F�N�g�̍X�V
	sky->GetPosData()->CalculateTransform();
	mRanking->Update(elapsed_time, mPlayFlag);
	UIManager::GetInctance()->Update(elapsed_time);
	pGpuParticleManager->Update(elapsed_time);
	//�t�F�[�h�A�E�g�J�n
	if (UIManager::GetInctance()->GetResultUIMove()->GetDecisionFlag())
	{
		mFade->StartFadeOut();
	}
	//�J�����̍X�V
	pCameraManager->Update(elapsed_time);

}
/******************************�`��֐�**************************************/
void SceneResult::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//�`��p�̃X�e�[�g�̐ݒ�
	mRasterizer->Activate(context);
	mDepth->Activate(context);
	mSampler[samplerType::wrap]->Activate(context, 0, true, true, true);
	mSampler[samplerType::clamp]->Activate(context, 2, true, true, true);
	mBlend[0]->activate(context);

	//�}���`�X���b�h�̏������I��������ǂ����𒲂ׂ�(�I����ĂȂ�������return)
	if (IsNowLoading())
	{
		//NowLoading�̕����̕`��
		mRenderScene->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, LoadColor(elapsed_time));
		//�`��p�̃X�e�[�g�̉���
		mBlend[0]->deactivate(context);
		mRasterizer->DeActivate(context);
		mDepth->DeActive(context);
		mSampler[samplerType::wrap]->DeActivate(context);
		mSampler[samplerType::clamp]->DeActivate(context);

		return;
	}

	//�V�[���̕`��
	pLight.ConstanceLightBufferSetShader(context);
	frameBuffer->Clear(context);
	frameBuffer->Activate(context);
	mBlend[0]->activate(context);
	FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
	FLOAT4X4 projection = pCameraManager->GetCamera()->GetProjection();

	sky->Render(context, view, projection);

	pGpuParticleManager->Render(context, view, projection);
	UIManager::GetInctance()->Render(context);

	mRanking->Render(context);

	frameBuffer->Deactivate(context);
	mBlend[0]->deactivate(context);

	//�u���[���̏���
	mBlend[1]->activate(context);
	mBloom->BlurTexture(context, frameBuffer->GetRenderTargetShaderResourceView().Get());

	//�V�[���Ƀu���[����������
	frameBuffer2->Clear(context);
	frameBuffer2->Activate(context);
	mRenderScene->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get());
	frameBuffer2->Deactivate(context);
	mBlend[1]->deactivate(context);

	//�u���[�����������V�[�����t�����g�o�b�t�@�ɕ`�悷��
	mBlend[0]->activate(context);

	mRenderScene->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	if (mScreenShot)
	{
		//�`�挋�ʂ�ۑ�
		mScreenShot = false;
		std::wstring fileName = L"Data/image/screen_shot/screenShot" + std::to_wstring(mTextureNo) + L".dds";
		frameBuffer2->SaveDDSFile(context, fileName.c_str(), frameBuffer2->GetRenderTargetShaderResourceView().Get());
	}
	mFade->Render(context);
	//�`��p�̃X�e�[�g�̉���
	mBlend[0]->deactivate(context);
	mRasterizer->DeActivate(context);
	mDepth->DeActive(context);
	mSampler[samplerType::wrap]->DeActivate(context);
	mSampler[samplerType::clamp]->DeActivate(context);

}

/************************����֐�***********************/
void SceneResult::Relese()
{
	//�V�[���Ɏg�����f�[�^�̉��
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
	UIManager::GetInctance()->ClearUI();
}


