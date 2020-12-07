#include "scene_result.h"
#include"ui_manager.h"
#include"scene_manager.h"
#include"camera_manager.h"
#include"gpu_particle_manager.h"
#include"stage_operation.h"
#include"light.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

SceneResult::SceneResult(ID3D11Device* device) :mNowGameTime(0), mEditorFlag(false), mEditorNo(0),
mPlayFlag(true), nowLoading(true)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);

			if (UIManager::GetInctance() != nullptr)
			{
				if (UIManager::GetInctance()->GetGameUIMove() != nullptr)
				{
					mNowGameTime = UIManager::GetInctance()->GetGameUIMove()->GetTime();
				}
			}
			else
			{
				UIManager::Create();
			}
			CameraManager::Create();
			pCameraManager->Initialize(device, 2);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));
			GpuParticleManager::Create();
			pGpuParticleManager->CreateResultBuffer(device);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::RESULT);

			UIManager::GetInctance()->ResultInitialize(device);
			mBlend.push_back(std::make_unique<blend_state>(device, BLEND_MODE::ALPHA));
			mRanking = std::make_unique<Ranking>(device, mNowGameTime);
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::RESULT);
			frameBuffer = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			mRenderScene = std::make_unique<Sprite>(device);
			mBloom = std::make_unique<BloomRender>(device, 1920, 1080, 3);
			//sky = std::make_unique<SkyMap>(device, L"Data/AllSkyFree/Cold Night/ColdNight.dds", MAPTYPE::BOX);
			//sky = std::make_unique<SkyMap>(device, L"Data/image/mp_totality.dds", MAPTYPE::BOX);
#if (RESULT_TYPE==0)
			sky = std::make_unique<SkyMap>(device, L"Data/AllSkyFree/Cold Night/ColdNight.dds", MAPTYPE::BOX);
			{
				FILE* fp;
				if (fopen_s(&fp, "Data/file/result_sky_map.bin", "rb") == 0)
				{
					fread(sky->GetPosData(), sizeof(Obj3D), 1, fp);
					fclose(fp);
				}
			}
#else
			sky = std::make_unique<SkyMap>(device, L"Data/image/sor_sea.dds", MAPTYPE::BOX);
			{
				FILE* fp;
				if (fopen_s(&fp, "Data/file/game_sky_map.bin", "rb") == 0)
				{
					fread(sky->GetPosData(), sizeof(Obj3D), 1, fp);
					fclose(fp);
				}
			}
			mSManager = std::make_unique<StageManager>(device, 1902, 1080);
			mSManager->SetStageNo(Ranking::GetStageNo());
			mSManager->Load();
			mRenderScene->Load(device, L"Data/image/siro.png");
			std::unique_ptr<StageOperation>stageOperation;
			stageOperation = std::make_unique<StageOperation>();
			stageOperation->Update(0, mSManager.get(), false);
			pLight.CreateLightBuffer(device);

#endif
		}, device);

}

void SceneResult::Update(float elapsed_time)
{
	nowLoading = IsNowLoading();
	if (nowLoading)
	{
		return;
	}
	EndLoading();
	if (ImGuiUpdate())return;
	mFade->Update(elapsed_time);
	//ƒV[ƒ“Ø‚è‘Ö‚¦
	if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
	{
		if (mFade->GetEndFlag())
		{
			int type = UIManager::GetInctance()->GetResultUIMove()->GetType();
			mFade->Clear();
			UIManager::Destroy();
			pCameraManager->DestroyCamera();
			GpuParticleManager::Destroy();
			switch (type)
			{
			case 0:
				pSceneManager.ChangeScene(SCENETYPE::GAME);
				break;
			case 1:
				pSceneManager.ChangeScene(SCENETYPE::TITLE);
				break;
			}
			return;
		}
	}
	sky->GetPosData()->CalculateTransform();
	mRanking->Update(elapsed_time, mPlayFlag);
	UIManager::GetInctance()->Update(elapsed_time);
	pCameraManager->Update(elapsed_time);
#if (RESULT_TYPE==1)

	mSManager->Update(elapsed_time);
#endif
	pGpuParticleManager->Update(elapsed_time);
	if (UIManager::GetInctance()->GetResultUIMove()->GetDecisionFlag())
	{
		mFade->StartFadeOut();
	}
}

void SceneResult::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	if (nowLoading)
	{
		return;
	}
	pLight.ConstanceLightBufferSetShader(context);
	frameBuffer->Clear(context);
	frameBuffer->Activate(context);
	mBlend[0]->activate(context);
	FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
	FLOAT4X4 projection = pCameraManager->GetCamera()->GetProjection();

	sky->Render(context, view, projection);
	//mRenderScene->Render(context, VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0, VECTOR4F(0.6f, 0.6f, 0.6f, 1));
#if (RESULT_TYPE==1)

	mSManager->Render(context, view, projection, 0);
	pGpuParticleManager->Render(context, view, projection);
#else
	pGpuParticleManager->Render(context, view, projection);

#endif

	mRanking->Render(context);
	UIManager::GetInctance()->Render(context);

	mBlend[0]->deactivate(context);

	frameBuffer->Deactivate(context);
	mBlend[0]->activate(context);

	mRenderScene->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);
	mFade->Render(context);
	mBlend[0]->deactivate(context);
}

SceneResult::~SceneResult()
{
}

bool SceneResult::ImGuiUpdate()
{
#ifdef USE_IMGUI
	switch (pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount()))
	{
	case 1:
		UIManager::GetInctance()->Clear();
		UIManager::Destroy();
		pCameraManager->DestroyCamera();
		GpuParticleManager::Destroy();

		pSceneManager.ChangeScene(SCENETYPE::TITLE);
		return true;
		break;
	case 2:
	case 3:
		UIManager::GetInctance()->Clear();
		UIManager::Destroy();
		pCameraManager->DestroyCamera();
		GpuParticleManager::Destroy();

		pSceneManager.ChangeScene(SCENETYPE::GAME);
		return true;
		break;
	}
	if (!mEditorFlag)return false;
	ImGui::Begin("scene result");
	ImGui::RadioButton("NONE", &mEditorNo, 0);
	ImGui::RadioButton("RANKING", &mEditorNo, 1);
	ImGui::RadioButton("UI", &mEditorNo, 2);
	ImGui::RadioButton("FADE", &mEditorNo, 3);
	ImGui::RadioButton("BLOOM", &mEditorNo, 4);
	ImGui::RadioButton("CAMERA", &mEditorNo, 5);
	ImGui::RadioButton("SKY MAP", &mEditorNo, 6);
	ImGui::RadioButton("PARTICLE", &mEditorNo, 7);

	ImGui::End();
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
		ImGui::Begin("sky map");
		float* position[3] = { &sky->GetPosData()->GetPosition().x,&sky->GetPosData()->GetPosition().y ,&sky->GetPosData()->GetPosition().z };
		ImGui::DragFloat3("position", *position, 10);
		float* scale[3] = { &sky->GetPosData()->GetScale().x,&sky->GetPosData()->GetScale().y ,&sky->GetPosData()->GetScale().z };
		ImGui::DragFloat3("scale", *scale, 10);
		float* angle[3] = { &sky->GetPosData()->GetAngle().x,&sky->GetPosData()->GetAngle().y ,&sky->GetPosData()->GetAngle().z };
		ImGui::SliderFloat3("angle", *angle, -3.14, 3.14);
		float* color[4] = { &sky->GetPosData()->GetColor().x,&sky->GetPosData()->GetColor().y ,&sky->GetPosData()->GetColor().z ,&sky->GetPosData()->GetColor().w };
		ImGui::ColorEdit4("color", *color);
		if (ImGui::Button("save"))
		{
			FILE* fp;
			fopen_s(&fp, "Data/file/result_sky_map.bin", "wb");
			fwrite(sky->GetPosData(), sizeof(Obj3D), 1, fp);
			fclose(fp);
		}
		ImGui::End();
	}

#endif
	return false;
}
