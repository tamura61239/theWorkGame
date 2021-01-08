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
			mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
			mRanking = std::make_unique<Ranking>(device, mNowGameTime);
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::RESULT);

			frameBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer2 = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			velocityBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);

			mRenderScene = std::make_unique<Sprite>(device,L"Data/image/siro.png");
			mBloom = std::make_unique<BloomRender>(device, 1920.f, 1080.f, 3);
			//sky = std::make_unique<SkyMap>(device, L"Data/AllSkyFree/Cold Night/ColdNight.dds", MAPTYPE::BOX);
			//sky = std::make_unique<SkyMap>(device, L"Data/image/mp_totality.dds", MAPTYPE::BOX);
			D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

			};
			mMotionBlurShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/motion_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
			sky = std::make_unique<SkyMap>(device, L"Data/AllSkyFree/Cold Night/ColdNight.dds", MAPTYPE::BOX);
			{
				FILE* fp;
				if (fopen_s(&fp, "Data/file/result_sky_map.bin", "rb") == 0)
				{
					fread(sky->GetPosData(), sizeof(Obj3D), 1, fp);
					fclose(fp);
				}
			}
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
				pSceneManager.ChangeScene(SceneManager::SCENETYPE::GAME);
				break;
			case 1:
				pSceneManager.ChangeScene(SceneManager::SCENETYPE::TITLE);
				break;
			}
			return;
		}
	}
	sky->GetPosData()->CalculateTransform();
	mRanking->Update(elapsed_time, mPlayFlag);
	UIManager::GetInctance()->Update(elapsed_time);
	pCameraManager->Update(elapsed_time);
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
	
#if 0
	pGpuParticleManager->Render(context, view, projection);


	mBlend[0]->deactivate(context);

	frameBuffer->Deactivate(context);

	velocityBuffer->Clear(context);
	velocityBuffer->Activate(context);
	pCameraManager->GetCamera()->ShaderSetBeforeBuffer(context, 5);

	pGpuParticleManager->VelocityRender(context, view, projection);
	velocityBuffer->Deactivate(context);

	mBlend[0]->activate(context);

	frameBuffer2->Clear(context);
	frameBuffer2->Activate(context);
	velocityBuffer->SetPsTexture(context, 1);
	mRenderScene->Render(context, mMotionBlurShader.get(), frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	UIManager::GetInctance()->Render(context);

	mRanking->Render(context);

	frameBuffer2->Deactivate(context);
	mRenderScene->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), true);
#else
	pGpuParticleManager->Render(context, view, projection);
	UIManager::GetInctance()->Render(context);

	mRanking->Render(context);

	frameBuffer->Deactivate(context);
	mRenderScene->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);

#endif
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

		pSceneManager.ChangeScene(SceneManager::SCENETYPE::TITLE);
		return true;
		break;
	case 2:
	case 3:
		UIManager::GetInctance()->Clear();
		UIManager::Destroy();
		pCameraManager->DestroyCamera();
		GpuParticleManager::Destroy();

		pSceneManager.ChangeScene(SceneManager::SCENETYPE::GAME);
		return true;
		break;
	}
	if (!mEditorFlag)return false;
	ImGui::Begin("scene result");
	ImVec2 view = ImVec2(192 * 3, 108 * 3);
	ImGui::Image(frameBuffer->GetRenderTargetShaderResourceView().Get(), view); ImGui::SameLine();
	ImGui::Image(velocityBuffer->GetRenderTargetShaderResourceView().Get(), view);
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
		ImGui::SliderFloat3("angle", *angle, -3.14f, 3.14f);
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
