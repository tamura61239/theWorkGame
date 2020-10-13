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
#define RENDER_MODE 0

SceneGame::SceneGame(ID3D11Device* device)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);

			pCamera.CreateCamera(device);
			pCamera.GetCamera()->SetEye(VECTOR3F(0, 0, -200));

			frameBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			for (int i = 1; i <= 2; i++)
			{
				shrinkBuffer[i-1] = std::make_unique<FrameBuffer>(device, 1920 >> i, 1080 >> i, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
			}
			velocityBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
			depthBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
			nowFrame = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			oldFrame = std::make_unique<FrameBuffer>(device, 1920, 1080, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
			mullti = std::make_unique<MulltiRenderTargetFunction>();
			mullti->SetFrameBuffer(frameBuffer);
			mullti->SetFrameBuffer(depthBuffer);
			pGpuParticleManager.CreateGameBuffer(device);
			pGpuParticleManager.SetState(GpuParticleManager::STATE::SELECT);

			player = std::make_unique<PlayerAI>(device, "Data/FBX/new_player_anim.fbx");
			mSManager = std::make_unique<StageManager>(device, 1920, 1080);
			modelRenderer = std::make_unique<ModelRenderer>(device);
			bloom = std::make_unique<BloomRender>(device, 1920, 1080);
			mStageOperation = std::make_unique<StageOperation>();
			pHitAreaDrow.CreateObj(device);
			pLight.CreateLightBuffer(device);
			{
				D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

				};
				motionShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/motionblur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
				depthShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/depth_of_field_synthetic_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
				blurShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/gaussian_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
			}
			{
				D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				modelBlurShader = std::make_unique<DrowShader>(device, "Data/shader/model_blur_vs.cso", "", "Data/shader/model_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
				modelDepthShader = std::make_unique<DrowShader>(device, "Data/shader/model_vs.cso", "", "Data/shader/deferred_depth_model_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
				
			}
			{
				D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				staticMeshDepthShader = std::make_unique<DrowShader>(device, "Data/shader/deferred_depth_static_mesh_vs.cso", "", "Data/shader/deferred_depth_static_mesh_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
			}
			sky = std::make_unique<SkyMap>(device, L"Data/image/sor_sea.dds", MAPTYPE::BOX);
			motionBlur = std::make_unique<MotionBlur>(device);
			mStageSelect = std::make_unique<StageSelect>(device);
		}, device);
	test = std::make_unique<Sprite>(device, L"Data/image/ゲームテスト.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/now.png");
	siro = std::make_unique<Sprite>(device, L"Data/image/かめれおんの拝啓.png");
	blend[0] = std::make_unique<blend_state>(device, BLEND_MODE::ALPHA);
	blend[1] = std::make_unique<blend_state>(device, BLEND_MODE::ADD);
	blend[2] = std::make_unique<blend_state>(device, BLEND_MODE::NONE);
	stop = false;
	editorNo = 0;
}
void SceneGame::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
	if (mStageSelect->GetSelectFlag())
	{
#ifdef USE_IMGUI
		static bool selects2[4] = { false,false,false,false };
		ImGui::Begin("select scene");
		ImGui::Selectable("SELECT_SCENE", &selects2[0]);
		ImGui::Selectable("PARTICLE", &selects2[1]);
		ImGui::Selectable("BLOOM", &selects2[2]);
		ImGui::Selectable("CAMERA", &selects2[3]);
		ImGui::End();
		if (selects2[0])
		{
			mStageSelect->ImGuiUpdate();
		}
		if (selects2[1])
		{
			pGpuParticleManager.ImGuiUpdate();
		}
		if (selects2[2])
		{
			bloom->ImGuiUpdate();
		}
		if (selects2[3])
		{
			pCamera.GetCameraOperation()->ImGuiUpdate();
		}

#endif
		pCamera.Update(elapsed_time);
		pGpuParticleManager.Update(elapsed_time);
		mStageSelect->Update(elapsed_time, mSManager.get());

		return;

	}
#ifdef USE_IMGUI
	ImGui::Begin("game scene");
	ImVec2 view = ImVec2(192*3,108*3);
	ImGui::Image(frameBuffer->GetRenderTargetShaderResourceView().Get(), view); 
	ImGui::Image(frameBuffer->GetDepthStencilShaderResourceView().Get(), view);ImGui::SameLine();
	ImGui::Checkbox("stop", &stop);
	if (ImGui::Button("return select scene"))
	{
		mSManager->Clear();
		mStageSelect->SetSelectFlag(true);
		pGpuParticleManager.SetState(GpuParticleManager::STATE::SELECT);
		pCamera.GetCamera()->SetEye(VECTOR3F(0, 0, -200));
		pCamera.GetCamera()->SetFocus(VECTOR3F(0, 0, 0));
		pCamera.GetCamera()->SetUp(VECTOR3F(0, 1, 0));
		pCamera.Update(elapsed_time);
		ImGui::End();
		return;
	}
	static bool selects[6] = { false,false,false,false,false,false };
	ImGui::Selectable("BLOOM", &selects[0]);
	ImGui::Selectable("LIGHT", &selects[1]);
	ImGui::Selectable("STAGE", &selects[2]);
	ImGui::Selectable("CAMERA", &selects[3]);
	ImGui::Selectable("PLAYER", &selects[4]);
	ImGui::Selectable("PARTICLE", &selects[5]);
	ImGui::End();
	if (selects[0])
	{
		bloom->ImGuiUpdate();
	}
	if (selects[1])
	{
		pLight.ImGuiUpdate();
	}
	if (selects[2])
	{
		mStageOperation->ImGuiUpdate();
		mSManager->ImGuiUpdate();
	}
	if (selects[3])
	{
		pCamera.GetCameraOperation()->ImGuiUpdate();
	}
	if (selects[4])
	{
		player->ImGuiUpdate();
	}
	if (selects[5])
	{
		pGpuParticleManager.ImGuiUpdate();
	}

	if (stop)return;
#endif
	mStageOperation->Update(elapsed_time, mSManager.get());
	pCamera.Update(elapsed_time);
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
	pGpuParticleManager.GetRunParticle()->SetPlayerData(player->GetCharacter()->GetVelocity(), player->GetCharacter()->GetGroundFlag(), player->GetCharacter()->GetPosition());
	pGpuParticleManager.Update(elapsed_time);
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
		nowLoading->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, color);
		blend[0]->deactivate(context);

		return;
	}
	EndLoading();
	if (mStageSelect->GetSelectFlag())
	{
		FLOAT4X4 view = pCamera.GetCamera()->GetView();
		FLOAT4X4 projection = pCamera.GetCamera()->GetProjection();
		frameBuffer->Clear(context);
		frameBuffer->Activate(context);
		pGpuParticleManager.Render(context, view, projection);

		blend[0]->activate(context);

		mStageSelect->Render(context);
		blend[0]->deactivate(context);
		frameBuffer->Deactivate(context);

		blend[1]->activate(context);
		siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		bloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);
		blend[1]->deactivate(context);

		return;
	}
	FLOAT4X4 view = pCamera.GetCamera()->GetView();
	FLOAT4X4 projection = pCamera.GetCamera()->GetProjection();
	FLOAT4X4 viewProjection;

	DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
	pLight.ConstanceLightBufferSetShader(context);
#if (RENDER_MODE==0)
	/************************カラーマップテクスチャの作成***********************/
	if (!stop)
	{

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
	}
	/****************ブルームをかける******************/
	blend[1]->activate(context);
	siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);
	blend[1]->deactivate(context);
	//siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

#elif (RENDER_MODE==1)
	/************************カラーマップテクスチャの作成***********************/
	if (!stop)
	{
		frameBuffer->Clear(context);
		frameBuffer->Activate(context);
		blend[0]->activate(context);
		//sky->Render(context, view, projection, VECTOR4F(1, 1, 1, 1));

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
		blend[0]->activate(context);

		pCamera.GetCamera()->ShaderSetBeforeBuffer(context, 5);
		pGpuParticleManager.RenderVelocity(context, view, projection);
		modelRenderer->Begin(context, viewProjection);
		modelRenderer->Draw(context, modelBlurShader.get(), *player->GetCharacter()->GetModel());
		modelRenderer->End(context);

		//sky->Render(context, skyBlurShader.get(), view, projection);
		//sky->SaveBeforeWorld();
		mSManager->RenderVelocity(context, view, projection, mStageOperation->GetColorType());
		blend[0]->deactivate(context);

		velocityBuffer->Deactivate(context);

	}

	/************************速度マップを使ってブラーをかける***********************/
	nowFrame->Clear(context);
	nowFrame->Activate(context);
	//motionBlur->CreateNeighborMaxBuffer(context, velocityBuffer->GetRenderTargetShaderResourceView().Get());
	//motionBlur->SetPsTexture(context,1);
	velocityBuffer->SetPsTexture(context, 1);
	siro->Render(context, motionShader.get(), frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);


	nowFrame->Deactivate(context);


	blend[1]->activate(context);
	siro->Render(context, nowFrame->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, nowFrame->GetRenderTargetShaderResourceView().Get(), true);
	blend[1]->deactivate(context);

#elif (RENDER_MODE==2)
	if (!stop)
	{
		//frameBuffer->Clear(context);
		//frameBuffer->Activate(context);
		//blend[0]->activate(context);
		////sky->Render(context, view, projection, VECTOR4F(1, 1, 1, 1));

		//pGpuParticleManager.Render(context, view, projection);
		//modelRenderer->Begin(context, viewProjection);
		//modelRenderer->Draw(context, *player->GetCharacter()->GetModel());
		//modelRenderer->End(context);
		//mSManager->Render(context, view, projection, mStageOperation->GetColorType());

		//blend[0]->deactivate(context);

		//frameBuffer->Deactivate(context);

		mullti->Clear(context);
		mullti->Activate(context);
		//blend[0]->activate(context);
		//sky->Render(context, view, projection, VECTOR4F(1, 1, 1, 1));

		pGpuParticleManager.Render(context, view, projection,true);
		modelRenderer->Begin(context, viewProjection);
		modelRenderer->Draw(context, modelDepthShader.get(), *player->GetCharacter()->GetModel());
		modelRenderer->End(context);
		mSManager->Render(context, view, projection, mStageOperation->GetColorType(), staticMeshDepthShader.get());

		//blend[0]->deactivate(context);

		mullti->Deactivate(context);

		for (int i = 0; i < 2; i++)
		{
			shrinkBuffer[i]->Clear(context);
			shrinkBuffer[i]->Activate(context);
			siro->Render(context, blurShader.get(), frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920>>(i+1), 1080 >> (i + 1)), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
			shrinkBuffer[i]->Deactivate(context);
		}
	}
	//depthOfField->ConversionDepthTexture(context, depthBuffer->GetRenderTargetShaderResourceView().Get());
	depthBuffer->SetPsTexture(context, 1);
	shrinkBuffer[0]->SetPsTexture(context,2);
	shrinkBuffer[1]->SetPsTexture(context, 3);

	siro->Render(context, depthShader.get(), frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	//siro->Render(context, depthShader.get(), frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

#endif
}

SceneGame::~SceneGame()
{
	pCamera.DestroyCamera();
}
