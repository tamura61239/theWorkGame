#include "scene_title.h"
#include"key_board.h"
#include"scene_manager.h"
#include"gpu_particle_manager.h"
#include"camera_manager.h"
#include"light.h"
#include"ui_manager.h"
#include"stage_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
SceneTitle::SceneTitle(ID3D11Device* device):mEditorFlag(true), mTestMove(false)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			GpuParticleManager::Create();
			pGpuParticleManager->CreateTitleBuffer(device);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::TITLE);
			pCameraManager->Initialize(device,0);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));
			pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::TITLE_CAMERA);
			pCameraManager->GetCameraOperation()->GetTitleCamera()->Load();
			bloom = std::make_unique<BloomRender>(device, 1920, 1080, 0);
			//bloom = std::make_unique<BloomRender>(device, 1920, 1080);
			frameBuffer[0] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer[1] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			//std::unique_ptr<ModelData>data = std::make_unique<ModelData>("Data/FBX/new_player_anim.fbx");
			//std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
			pLight.CreateLightBuffer(device);
			
			//obj = std::make_unique<StaticObj>(device, "Data/FBX/Mr.Incredible/Mr.Incredible.fbx");
			//mRender = std::make_unique<MeshRender>(device);
			//character->GetModel()->PlayAnimation(0, true);
			UIManager::Create();
			UIManager::GetInctance()->TitleInitialize(device);
			modelRender = std::make_unique<ModelRenderer>(device);
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::TITLE);
			mFade->StartFadeIn();
			D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

			};
			mBluer = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/zoom_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));

		}, device);
	test = std::make_unique<Sprite>(device/*, L"Data/image/change_color.png"*/);
	blend[0] = std::make_unique<blend_state>(device, BLEND_MODE::ADD);
	blend[1] = std::make_unique<blend_state>(device, BLEND_MODE::ALPHA);

}

void SceneTitle::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
	if (ImGuiUpdate())return;
	mFade->Update(elapsed_time);
	UIManager::GetInctance()->Update(elapsed_time);
	pCameraManager->Update(elapsed_time);
	pGpuParticleManager->GetTitleParticle()->SetChangeFlag(pCameraManager->GetCameraOperation()->GetTitleCamera()->GetTitleSceneChangeFlag());
	pGpuParticleManager->Update(elapsed_time);
	if (pCameraManager->GetCameraOperation()->GetTitleCamera()->GetEndTitleFlag())
	{
		GpuParticleManager::Destroy();
		UIManager::Destroy();
		pSceneManager.ChangeScene(SCENETYPE::GAME);

		return;
	}
	if (UIManager::GetInctance()->GetTitleUIMove()->GetMoveChangeFlag())
	{
		if (pKeyBoad.RisingState(KeyLabel::SPACE))
		{
			UIManager::GetInctance()->GetTitleUIMove()->SetMoveChangeFlag(false);
			pCameraManager->GetCameraOperation()->GetTitleCamera()->SetTitleSceneChangeFlag(true);
			pGpuParticleManager->GetTitleTextureParticle()->SetSceneDrowFlag(true);
			UIManager::GetInctance()->ClearUI();
		}
	}
}

void SceneTitle::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
	frameBuffer[0]->Clear(context);
	frameBuffer[0]->Activate(context);
	FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
	FLOAT4X4 projection = pCameraManager->GetCamera()->GetProjection();
    blend[1]->activate(context);
	pGpuParticleManager->Render(context, view, projection);
	
	UIManager::GetInctance()->Render(context);
	blend[1]->deactivate(context);

	frameBuffer[0]->Deactivate(context);

	frameBuffer[1]->Clear(context);
	frameBuffer[1]->Activate(context);

	blend[0]->activate(context);
	test->Render(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), true);
	blend[0]->deactivate(context);
	frameBuffer[1]->Deactivate(context);

	blend[1]->activate(context);
	if (pCameraManager->GetCameraOperation()->GetTitleCamera()->GetMoveFlag())
	{
		test->Render(context,mBluer.get(), frameBuffer[1]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	}
	else
	{
		test->Render(context, frameBuffer[1]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	}
	mFade->Render(context);
	blend[1]->deactivate(context);

}

SceneTitle::~SceneTitle()
{
}

bool SceneTitle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	switch (pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount()))
	{
	case 2:
	case 3:
		GpuParticleManager::Destroy();
		UIManager::GetInctance()->Clear();
		UIManager::Destroy();

		pSceneManager.ChangeScene(SCENETYPE::GAME);
		return true;
		break;
	case 4:
		GpuParticleManager::Destroy();
		UIManager::GetInctance()->Clear();
		UIManager::Destroy();

		pSceneManager.ChangeScene(SCENETYPE::RESULT);
		return true;
		break;
	}
	if (!mEditorFlag)return false;
	ImGui::Begin("scene title");
	static int editorNum = 1;
	ImGui::RadioButton("LIGHT", &editorNum, 0);
	ImGui::RadioButton("UI", &editorNum, 1);
	ImGui::RadioButton("GPU PARTICLE", &editorNum, 2);
	ImGui::RadioButton("CAMERA", &editorNum, 3);
	ImGui::RadioButton("BLOOM", &editorNum, 4);
	ImGui::RadioButton("FADE", &editorNum, 5);
	ImGui::End();
	switch (editorNum)
	{
	case 0:
		pLight.ImGuiUpdate();
		break;
	case 1:
		UIManager::GetInctance()->ImGuiUpdate();
		break;
	case 2:
		pGpuParticleManager->ImGuiUpdate();
		break;
	case 3:
		pCameraManager->GetCameraOperation()->ImGuiUpdate();
		break;
	case 4:
		bloom->ImGuiUpdate();
		break;
	case 5:
		mFade->ImGuiUpdate();
		break;

	}

#endif

	return false;
}
