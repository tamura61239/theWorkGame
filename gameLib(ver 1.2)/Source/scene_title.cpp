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
			frameBuffer = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			//std::unique_ptr<ModelData>data = std::make_unique<ModelData>("Data/FBX/new_player_anim.fbx");
			//std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
			pLight.CreateLightBuffer(device);
			
			//obj = std::make_unique<StaticObj>(device, "Data/FBX/Mr.Incredible/Mr.Incredible.fbx");
			//mRender = std::make_unique<MeshRender>(device);
			//character->GetModel()->PlayAnimation(0, true);
			UIManager::Create();
			UIManager::GetInctance()->TitleInitialize(device);
			modelRender = std::make_unique<ModelRenderer>(device);
			pGpuParticleManager->GetTitleTextureParticle()->CreateBuffer(device);
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::TITLE);
			mFade->StartFadeIn();
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
	if (UIManager::GetInctance()->GetTitleMoveChangeFlag())
	{
		if (pKeyBoad.RisingState(KeyLabel::SPACE))
		{
			pCameraManager->GetCameraOperation()->GetTitleCamera()->SetTitleSceneChangeFlag(true);
			pGpuParticleManager->GetTitleTextureParticle()->SetStartFlag(true);
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
	frameBuffer->Clear(context);
	frameBuffer->Activate(context);
	FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
	FLOAT4X4 projection = pCameraManager->GetCamera()->GetProjection();

	pGpuParticleManager->Render(context, view, projection);
	blend[1]->activate(context);
	UIManager::GetInctance()->Render(context);
	blend[1]->deactivate(context);

	frameBuffer->Deactivate(context);
	blend[0]->activate(context);
	test->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);
	blend[0]->deactivate(context);
	blend[1]->activate(context);
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
