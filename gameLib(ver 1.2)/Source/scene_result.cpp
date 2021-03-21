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

void SceneResult::Initialize(ID3D11Device* device)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);

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

			frameBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer2 = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			velocityBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);

			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 3);
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
			sky->GetPosData()->CalculateTransform();

		}, device);
	mRenderScene = std::make_unique<Sprite>(device, L"Data/image/now.png");
	renderFlag = false;
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	mSampler.push_back(std::make_unique<SamplerState>(device));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);
	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);

}

void SceneResult::Editor()
{
#ifdef USE_IMGUI
	if (IsNowLoading())
	{
		return;
	}

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

	ImGui::Begin("scene result");
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("screen shot"))
	{
		ImGui::InputInt("No", &mTextureNo, 1);

		if (ImGui::Button("shot"))
		{
			mScreenShot = true;
		}
	}
#endif
	ImVec2 view = ImVec2(192 * 3, 108 * 3);
	//ImGui::Image(frameBuffer->GetRenderTargetShaderResourceView().Get(), view); ImGui::SameLine();
	//ImGui::Image(velocityBuffer->GetRenderTargetShaderResourceView().Get(), view);
	ImGui::RadioButton("NONE", &mEditorNo, 0);
	ImGui::RadioButton("RANKING", &mEditorNo, 1);
	ImGui::RadioButton("UI", &mEditorNo, 2);
	ImGui::RadioButton("FADE", &mEditorNo, 3);
	ImGui::RadioButton("BLOOM", &mEditorNo, 4);
	ImGui::RadioButton("CAMERA", &mEditorNo, 5);
	ImGui::RadioButton("SKY MAP", &mEditorNo, 6);
	ImGui::RadioButton("GPU PARTICLE", &mEditorNo, 7);

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

}

void SceneResult::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
	mFade->Update(elapsed_time);
	//ƒV[ƒ“Ø‚è‘Ö‚¦
	if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
	{
		if (mFade->GetEndFlag())
		{
			int type = UIManager::GetInctance()->GetResultUIMove()->GetType();
			switch (type)
			{
			case 0:
				pSceneManager.ChangeScene(new SceneGame(Ranking::GetStageNo()));
				break;
			case 1:
				pSceneManager.ChangeScene(new SceneTitle);
				break;
			case 2:
				pSceneManager.ChangeScene(new SceneSelect);
				break;
			case 3:
				pSceneManager.ChangeScene(new SceneGame(Ranking::GetStageNo() + 1));
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
	mRasterizer->Activate(context);
	mDepth->Activate(context);
	mSampler[samplerType::wrap]->Activate(context, 0, true, true, true);
	mSampler[samplerType::clamp]->Activate(context, 2, true, true, true);
	mBlend[0]->activate(context);

	if (IsNowLoading())
	{
		mRenderScene->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, LoadColor(elapsed_time));
		mBlend[0]->deactivate(context);
		mRasterizer->DeActivate(context);
		mDepth->DeActive(context);
		mSampler[samplerType::wrap]->DeActivate(context);
		mSampler[samplerType::clamp]->DeActivate(context);

		return;
	}


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

	mBlend[1]->activate(context);
	mBloom->BlurTexture(context, frameBuffer->GetRenderTargetShaderResourceView().Get());

	frameBuffer2->Clear(context);
	frameBuffer2->Activate(context);
	mRenderScene->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get());
	frameBuffer2->Deactivate(context);
	mBlend[1]->deactivate(context);
	mBlend[0]->activate(context);

	mRenderScene->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	if (mScreenShot)
	{
		mScreenShot = false;
		std::wstring fileName = L"Data/image/screen_shot/screenShot" + std::to_wstring(mTextureNo) + L".dds";
		frameBuffer2->SaveDDSFile(context, fileName.c_str(), frameBuffer2->GetRenderTargetShaderResourceView().Get());
	}
	mFade->Render(context);
	mBlend[0]->deactivate(context);
	mRasterizer->DeActivate(context);
	mDepth->DeActive(context);
	mSampler[samplerType::wrap]->DeActivate(context);
	mSampler[samplerType::clamp]->DeActivate(context);

}

void SceneResult::Relese()
{
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
	UIManager::GetInctance()->ClearUI();
}


