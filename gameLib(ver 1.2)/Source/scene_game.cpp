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
#include"framework.h"
//#ifdef USE_IMGUI
//#include <imgui.h>
//#include <imgui_impl_dx11.h>
//#include <imgui_impl_win32.h>
//#include <imgui_internal.h>
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
//#endif
#define RENDER_MODE 1

SceneGame::SceneGame(ID3D11Device* device)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);

			pCamera.CreateCamera(device);
			pCamera.GetCamera()->SetEye(VECTOR3F(100, 40, -200));

			frameBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
			velocityBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
			nowFrame = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			oldFrame = std::make_unique<FrameBuffer>(device, 1920, 1080, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
			mullti = std::make_unique<MulltiRenderTargetFunction>();
			mullti->SetFrameBuffer(frameBuffer);
			mullti->SetFrameBuffer(velocityBuffer);
			player = std::make_unique<PlayerAI>(device, "Data/FBX/new_player_anim.fbx");
			mSManager = std::make_unique<StageManager>(device, 1920, 1080);
			modelRenderer = std::make_unique<ModelRenderer>(device);
			bloom = std::make_unique<BloomRender>(device, 1920, 1080);
			mStageOperation = std::make_unique<StageOperation>();
			pHitAreaDrow.CreateObj(device);
			pGpuParticleManager.CreateBuffer(device);
			pLight.CreateLightBuffer(device);
			D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

			};
			motionShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/motionblur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
			//sky = std::make_unique<SkyMap>(device, L"Data/image/sor_sea.dds",MAPTYPE::BOX);
			motionBlur = std::make_unique<MotionBlur>(device);
		}, device);
	test = std::make_unique<Sprite>(device, L"Data/image/ゲームテスト.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/wp-thumb.jpg");
	siro = std::make_unique<Sprite>(device, L"Data/image/かめれおんの拝啓.png");
	blend[0] = std::make_unique<blend_state>(device, BLEND_MODE::ALPHA);
	blend[1] = std::make_unique<blend_state>(device, BLEND_MODE::ADD);

}
static bool stop = false;
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
	//VECTOR3F scale = sky->GetPosData()->GetScale();
	//VECTOR3F position = sky->GetPosData()->GetPosition();
	//ImGui::Begin("sky map");
	//ImGui::InputFloat("scale x", &scale.x, 1);
	//ImGui::InputFloat("scale y", &scale.y, 1);
	//ImGui::InputFloat("scale z", &scale.z, 1);

	//ImGui::InputFloat("position x", &position.x, 1);
	//ImGui::InputFloat("position y", &position.y, 1);
	//ImGui::InputFloat("position z", &position.z, 1);

	//ImGui::End();
	//sky->GetPosData()->SetScale(scale);
	//sky->GetPosData()->SetPosition(position);
	//sky->GetPosData()->CalculateTransform();
	ImGui::Begin("scene");
	ImVec2 view = ImGui::GetWindowSize();
	view.y /= 2.5f;
	ImGui::Image(frameBuffer->GetRenderTargetShaderResourceView().Get(), view);
	ImGui::Image(velocityBuffer->GetRenderTargetShaderResourceView().Get(), view);
	ImGui::Checkbox("stop", &stop);
	ImGui::End();
#endif
	if (stop)return;
	mStageOperation->Update(elapsed_time, mSManager.get());

	player->Update(elapsed_time, mSManager.get());
	mSManager->Update(elapsed_time);
#if (RENDER_MODE==1)
	//static float angle = 0;
	//angle += DirectX::XMConvertToRadians(150) * elapsed_time;
	//if (angle >= DirectX::XMConvertToRadians(360))angle -= DirectX::XMConvertToRadians(360);
	//pCamera.GetCamera()->SetEye(pCamera.GetCamera()->GetEye() + VECTOR3F(100/*sinf(angle)*300*/, 0, cosf(angle)*300));
	//pCamera.GetCamera()->SetFocus(pCamera.GetCamera()->GetEye()+VECTOR3F(sinf(angle), -0.1f, cosf(angle)));
#endif
	pCamera.Update(elapsed_time);
	pGpuParticleManager.Update(elapsed_time, mStageOperation->GetColorType(), player->GetCharacter()->GetVelocity(), player->GetCharacter()->GetPosition(), player->GetCharacter()->GetGroundFlag());
	if (pKeyBoad.RisingState(KeyLabel::ENTER))
	{
		pSceneManager.ChangeScene(SCENETYPE::TITLE);
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
	EndLoading();

	FLOAT4X4 view = pCamera.GetCamera()->GetView();
	FLOAT4X4 projection = pCamera.GetCamera()->GetProjection();
	FLOAT4X4 viewProjection;

	DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
	pLight.ConstanceLightBufferSetShader(context);
#if (RENDER_MODE==0)
	/************************カラーマップテクスチャの作成***********************/
	frameBuffer->Clear(context);
	frameBuffer->Activate(context);

	blend[0]->activate(context);

	modelRenderer->Begin(context, viewProjection);
	modelRenderer->Draw(context, *player->GetCharacter()->GetModel());
	modelRenderer->End(context);
	pGpuParticleManager.Render(context, view, projection);

	mSManager->Render(context, view, projection, mStageOperation->GetColorType());

	blend[0]->deactivate(context);
	frameBuffer->Deactivate(context);

	/****************ブルームをかける******************/
	blend[1]->activate(context);
	siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);
	blend[1]->deactivate(context);
#elif (RENDER_MODE==1)
	/************************カラーマップテクスチャの作成***********************/
	if (!stop)
	{
		frameBuffer->Clear(context);
		frameBuffer->Activate(context);
		blend[0]->activate(context);

		pGpuParticleManager.Render(context, view, projection);
		modelRenderer->Begin(context, viewProjection);
		modelRenderer->Draw(context, *player->GetCharacter()->GetModel());
		modelRenderer->End(context);

		mSManager->Render(context, view, projection, mStageOperation->GetColorType());

		blend[0]->deactivate(context);
		frameBuffer->Deactivate(context);

		/************************速度マップテクスチャの作成***********************/
		velocityBuffer->Clear(context);
		velocityBuffer->Activate(context);
		pCamera.GetCamera()->ShaderSetBeforeBuffer(context, 5);
		pGpuParticleManager.RenderVelocity(context, view, projection);
		mSManager->RenderVelocity(context, view, projection, mStageOperation->GetColorType());

		velocityBuffer->Deactivate(context);

	}

	/************************速度マップを使ってブラーをかける***********************/
	nowFrame->Clear(context);
	nowFrame->Activate(context);
	velocityBuffer->SetPsTexture(context, 1);
	siro->Render(context, motionShader.get(), frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	nowFrame->Deactivate(context);

	blend[1]->activate(context);
	siro->Render(context, nowFrame->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, nowFrame->GetRenderTargetShaderResourceView().Get(), true);
	blend[1]->deactivate(context);

#endif
}

SceneGame::~SceneGame()
{
	pCamera.DestroyCamera();
}
