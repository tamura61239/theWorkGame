#include "scene_game.h"
#include"key_board.h"
#include"scene_manager.h"
#include"sound_manager.h"
#include"camera_manager.h"
#include"gamepad.h"
#include"light.h"
#include"Judgment.h"
#include"hit_area_drow.h"
#include"gpu_particle_manager.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

SceneGame::SceneGame(ID3D11Device* device)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			frameBuffer[0] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
			frameBuffer[1] = std::make_unique<FrameBuffer>(device, 1920, 1080, false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN);
			renderEffects = std::make_unique<RenderEffects>(device);
			player = std::make_unique<PlayerAI>(device,"Data/FBX/new_player_anim.fbx");
			mSManager = std::make_unique<StageManager>(device, 1920, 1080);
			modelRenderer = std::make_unique<ModelRenderer>(device);
			bloom = std::make_unique<BloomRender>(device, 1920, 1080);
			mStageOperation = std::make_unique<StageOperation>();
			pHitAreaDrow.CreateObj(device);
			pGpuParticleManager.CreateBuffer(device);
		}, device);
	test = std::make_unique<Sprite>(device, L"Data/image/ゲームテスト.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/wp-thumb.jpg");
	siro = std::make_unique<Sprite>(device,L"Data/image/かめれおんの拝啓.png");
	blend[0] = std::make_unique<blend_state>(device, BLEND_MODE::ALPHA);
	blend[1] = std::make_unique<blend_state>(device, BLEND_MODE::ADD);
	pCamera.CreateCamera();
	pCamera.GetCamera()->SetEye(VECTOR3F(100, 40, -200));
	//pSoundManager.Play(0, true);
	//pSoundManager.SetVolume(0, 1.0f);
	pLight.CreateLightBuffer(device);
}
void SceneGame::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();

#ifdef USE_IMGUI
	bloom->ImGuiUpdate();
	pLight.ImGuiUpdate();
#endif
	mStageOperation->Update(elapsed_time, mSManager.get());

	player->Update(elapsed_time,mSManager.get());
	mSManager->Update(elapsed_time);
	pCamera.Update(elapsed_time);
	pGpuParticleManager.Update(elapsed_time, mStageOperation->GetColorType(),player->GetCharacter()->GetVelocity(),player->GetCharacter()->GetPosition(),player->GetCharacter()->GetGroundFlag());
	if (pKeyBoad.RisingState(KeyLabel::ENTER))
	{
		pSceneManager.ChangeScene(SCENETYPE::OVER);
		//pSoundManager.Stop(0);
		pGpuParticleManager.ClearBuffer();
		return;
	}
}

void SceneGame::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);

	if (IsNowLoading())
	{
		static float loadTimer = 0;

		VECTOR4F color;
		if (loadTimer < 1)
		{
			color = VECTOR4F(1, 1, 1, loadTimer);
		}
		else if (loadTimer < 3)
		{
			color = VECTOR4F(1, 1, 1, 1);
		}
		else if (loadTimer < 4)
		{
			color = VECTOR4F(1, 1, 1, 4 - loadTimer);
		}
		else
		{
			loadTimer = 0;
			color = VECTOR4F(1, 1, 1, 0);
		}
		loadTimer += elapsed_time;
		blend[0]->activate(context);
		nowLoading->Render(context, VECTOR2F(0, 0), VECTOR2F(viewport.Width, viewport.Height), VECTOR2F(0, 0), VECTOR2F(1200, 675), 0, color);
		blend[0]->deactivate(context);

		return;
	}
	FLOAT4X4 view = pCamera.GetCamera()->GetView();
	FLOAT4X4 projection = pCamera.GetCamera()->GetProjection();
	FLOAT4X4 viewProjection;

	DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));

	/************************カラーマップテクスチャの作成***********************/
	frameBuffer[0]->Clear(context);
	frameBuffer[0]->Activate(context);
	pLight.ConstanceLightBufferSetShader(context);


	blend[0]->activate(context);

	modelRenderer->Begin(context, viewProjection);
	modelRenderer->Draw(context, *player->GetCharacter()->GetModel());
	modelRenderer->End(context);

	mSManager->Render(context, view, projection,mStageOperation->GetColorType());
	pGpuParticleManager.Render(context, view, projection);

	blend[0]->deactivate(context);

	frameBuffer[0]->Deactivate(context);
	/****************ブルームをかける******************/
	//renderEffects->ShadowRender(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), frameBuffer[0]->GetDepthStencilShaderResourceView().Get(), shadowMap->GetDepthStencilShaderResourceView().Get(), view, projection, lightCamera.GetView(), lightCamera.GetProjection());
	blend[1]->activate(context);
	siro->Render(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), true);
	blend[1]->deactivate(context);
}

SceneGame::~SceneGame()
{
	pCamera.DestroyCamera();
}
