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
#include"ui_manager.h"
//#ifdef USE_IMGUI
//#include <imgui.h>
//#include <imgui_impl_dx11.h>
//#include <imgui_impl_win32.h>
//#include <imgui_internal.h>
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
//#endif

SceneGame::SceneGame(ID3D11Device* device) : selectSceneFlag(true), editorFlag(false), testGame(false)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);

			pCameraManager.CreateCamera(device, 1);
			pCameraManager.GetCamera()->SetEye(VECTOR3F(0, 0, -200));

			frameBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			for (int i = 1; i <= 2; i++)
			{
				shrinkBuffer[i - 1] = std::make_unique<FrameBuffer>(device, 1920 >> i, 1080 >> i, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
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
			mesh = std::make_unique<StaticMesh>(device, "Data/FBX/anim_data.fbx");
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
			mStageSelect = std::make_unique<StageSelect>(device, StageManager::GetMaxStageCount());
			fadeOut = std::make_unique<Fade>(device, Fade::FADE_SCENE::SELECT);
			if(pSceneManager.GetSceneEditor()->GetSceneNo()==3)
			{
				mSManager->SetStageNo(pSceneManager.GetSceneEditor()->GetStageNo());
				mSManager->Load();
				selectSceneFlag = false;
				fadeOut->Clear();
				fadeOut->SetFadeScene(Fade::FADE_SCENE::GAME);
				pGpuParticleManager.SetState(GpuParticleManager::STATE::GAME);
				pCameraManager.GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
				fadeOut->StartFadeIn();
			}
			UIManager::GetInctance().GameInitialize(device);
		}, device);
	test = std::make_unique<Sprite>(device, L"Data/image/ゲームテスト.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/now.png");
	siro = std::make_unique<Sprite>(device, L"Data/image/siro.png");
	blend[0] = std::make_unique<blend_state>(device, BLEND_MODE::ALPHA);
	blend[1] = std::make_unique<blend_state>(device, BLEND_MODE::ADD);
	blend[2] = std::make_unique<blend_state>(device, BLEND_MODE::NONE);
	stop = false;
	editorNo = 0;
}
/***************************************************************/
//                          更新
/***************************************************************/
void SceneGame::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
	/********************Editor************************/
#ifdef USE_IMGUI
	if (ImGuiUpdate())return;
	if (stop)return;
#endif
	fadeOut->Update(elapsed_time);
	/**********************SelectSceneの更新*******************************/
	if (selectSceneFlag)
	{
		pCameraManager.Update(elapsed_time);
		pGpuParticleManager.Update(elapsed_time);
		mStageSelect->Update(elapsed_time, mSManager.get());
		if (fadeOut->GetEndFlag())
		{
			if (fadeOut->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
			{
				mSManager->Load();
				selectSceneFlag = false;
				mStageSelect->SetSelectFlag(true);
				fadeOut->Clear();
				fadeOut->SetFadeScene(Fade::FADE_SCENE::GAME);
				fadeOut->StartFadeIn();
				pGpuParticleManager.SetState(GpuParticleManager::STATE::GAME);
				pCameraManager.GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
				return;
			}
		}
		if (!mStageSelect->GetSelectFlag())
		{
			fadeOut->StartFadeOut();
		}
		return;

	}
	/**********************GameSceneの更新*******************************/
	if (fadeOut->GetFadeScene() == Fade::FADE_MODO::FADEIN)
	{
		if (fadeOut->GetEndFlag() && !editorFlag)
		{
			fadeOut->Clear();
			UIManager::GetInctance().GetGameUIMove()->Start();
		}
	}
	else if (fadeOut->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
	{
		if (fadeOut->GetEndFlag())
		{
			fadeOut->Clear();
			pSceneManager.ChangeScene(SCENETYPE::CLEAR);
			pGpuParticleManager.ClearBuffer();
			UIManager::GetInctance().Clear();

			return;
		}

	}
	UIManager::GetInctance().Update(elapsed_time);
	if (UIManager::GetInctance().GetGameUIMove()->GetCount() <= 0)
	{
		if (UIManager::GetInctance().GetGameUIMove()->GetStartFlag())
		{
			player->SetPlayFlag(true);
		}
	}

	mStageOperation->Update(elapsed_time, mSManager.get(), player->GetPlayFlag());
	player->Update(elapsed_time, mSManager.get(),mStageOperation.get());
	if (player->GetCharacter()->GetGorlFlag())
	{
		fadeOut->StartFadeOut();
	}
	mSManager->Update(elapsed_time);
	pCameraManager.Update(elapsed_time);
	pGpuParticleManager.GetRunParticle()->SetBoneData(player->GetCharacter()->GetModel());
	pGpuParticleManager.GetRunParticle()->SetPlayerData(player->GetCharacter()->GetVelocity(), true);
	pGpuParticleManager.Update(elapsed_time);
}
/**********************Editor*********************/
bool SceneGame::ImGuiUpdate()
{
#ifdef USE_IMGUI
	/********************Sceneの選択結果処理***************************/
	switch (pSceneManager.GetSceneEditor()->Editor(&editorFlag,StageManager::GetMaxStageCount()))
	{
	case 1:
		pSceneManager.ChangeScene(SCENETYPE::TITLE);
		pGpuParticleManager.ClearBuffer();
		UIManager::GetInctance().Clear();
		return true;
		break;
	case 2:
		if (!selectSceneFlag)
		{
			selectSceneFlag = true;
			fadeOut->Clear();
			fadeOut->SetFadeScene(Fade::FADE_SCENE::SELECT);
			pGpuParticleManager.SetState(GpuParticleManager::STATE::SELECT);
			pCameraManager.GetCamera()->SetEye(VECTOR3F(0, 0, -200));
			pCameraManager.GetCamera()->SetFocus(VECTOR3F(0, 0, 0));
			pCameraManager.GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::NORMAL);
			mSManager->Clear();
			mSManager->GetStageEditor()->Clear();
			mSManager->GetStageEditor()->ClearFileState();
			editorNo = 0;
			testGame = false;
			player->GetCharacter()->SetBeforePosition(VECTOR3F(0, 10, 0));
			player->GetCharacter()->SetPosition(VECTOR3F(0, 10, 0));
			player->GetCharacter()->SetAngle(VECTOR3F(0, 0, 0));
			player->GetCharacter()->SetVelocity(VECTOR3F(0, 0, 0));
			player->GetCharacter()->SetGorlFlag(false);
			player->SetPlayFlag(false);
			UIManager::GetInctance().GetGameUIMove()->SetStartFlag(false);
			UIManager::GetInctance().ResetGameUI();

		}
		break;
	case 3:
		if (selectSceneFlag)
		{
			editorNo = 0;
			selectSceneFlag = false;
		}
		mSManager->SetStageNo(pSceneManager.GetSceneEditor()->GetStageNo());
		mSManager->Clear();
		mSManager->Load();
		fadeOut->Clear();
		fadeOut->SetFadeScene(Fade::FADE_SCENE::GAME);
		pGpuParticleManager.SetState(GpuParticleManager::STATE::GAME);
		pCameraManager.GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
		break;
	case 4:
		pSceneManager.ChangeScene(SCENETYPE::CLEAR);
		pGpuParticleManager.ClearBuffer();
		UIManager::GetInctance().Clear();

		return true;
		break;
	case 5:
		pSceneManager.ChangeScene(SCENETYPE::OVER);
		pGpuParticleManager.ClearBuffer();
		UIManager::GetInctance().Clear();

		return true;
		break;
	}
	//editorFlagがfalseの時
	if (!editorFlag)return false;
	//前のフレームのeditorNoを保存
	int beforeEditorNo = editorNo;
	//エディターの名前を決める
	std::string editorName = { "scene game" };
	if (selectSceneFlag)editorName = "scene seletc";
	/*******************Editor****************/
	ImGui::Begin(editorName.c_str());
	ImGui::Checkbox("stop", &stop);

	if (selectSceneFlag)
	{
		ImGui::RadioButton("SELECT_SCENE", &editorNo, 1);
	}
	else
	{
		if (!testGame)
		{
			if (ImGui::Button("testGame"))
			{
				testGame = true;
				UIManager::GetInctance().GetGameUIMove()->Start();
			}
		}
		else
		{
			if (ImGui::Button("reset"))
			{
				testGame = false;
				player->GetCharacter()->SetBeforePosition(VECTOR3F(0, 10, 0));
				player->GetCharacter()->SetPosition(VECTOR3F(0, 10, 0));
				player->GetCharacter()->SetAngle(VECTOR3F(0, 0, 0));
				player->GetCharacter()->SetVelocity(VECTOR3F(0, 0, 0));
				player->GetCharacter()->SetGorlFlag(false);
				player->SetPlayFlag(false);
				UIManager::GetInctance().GetGameUIMove()->SetStartFlag(false);
				UIManager::GetInctance().ResetGameUI();

			}

		}

		ImVec2 view = ImVec2(192 * 3, 108 * 3);
		ImGui::Image(frameBuffer->GetRenderTargetShaderResourceView().Get(), view); ImGui::SameLine();
		ImGui::Image(frameBuffer->GetDepthStencilShaderResourceView().Get(), view);

		ImGui::RadioButton("LIGHT", &editorNo, 2);
		ImGui::RadioButton("STAGE", &editorNo, 3);
		ImGui::RadioButton("PLAYER", &editorNo, 4);
		ImGui::RadioButton("UI", &editorNo, 9);
	}
	ImGui::RadioButton("PARTICLE", &editorNo, 5);
	ImGui::RadioButton("CAMERA", &editorNo, 6);
	ImGui::RadioButton("BLOOM", &editorNo, 7);
	ImGui::RadioButton("FADE", &editorNo, 8);
	ImGui::RadioButton("NONE", &editorNo, 0);
	//ゲームシーンの時
	if (!selectSceneFlag)
	{
		if (beforeEditorNo == 3)
		{
			if (beforeEditorNo != editorNo)
			{
				pCameraManager.GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
			}
		}
	}
	ImGui::End();
	/******************何番目のエディターを操作するか*********************/
	switch (editorNo)
	{
	case 1:
		mStageSelect->ImGuiUpdate();
		break;
	case 2:
		pLight.ImGuiUpdate();
		break;
	case 3:
		mStageOperation->ImGuiUpdate();
		mSManager->ImGuiUpdate();
		break;
	case 4:
		player->ImGuiUpdate();
		break;
	case 5:
		pGpuParticleManager.ImGuiUpdate();
		break;
	case 6:
		pCameraManager.GetCameraOperation()->ImGuiUpdate();
		break;
	case 7:
		bloom->ImGuiUpdate();
		break;
	case 8:
		fadeOut->ImGuiUpdate();
		break;
	case 9:
		UIManager::GetInctance().ImGuiUpdate();
		break;

	}
#endif
	return false;
}


/***************************************************************/
//                          描画
/***************************************************************/

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
	//view projection行列の取得
	FLOAT4X4 view = pCameraManager.GetCamera()->GetView();
	FLOAT4X4 projection = pCameraManager.GetCamera()->GetProjection();

	/*****************SelectSceneの描画 ******************/
	if (selectSceneFlag)
	{
		frameBuffer->Clear(context);
		frameBuffer->Activate(context);
		pGpuParticleManager.Render(context, view, projection);

		blend[0]->activate(context);

		mStageSelect->Render(context);
		blend[0]->deactivate(context);
		frameBuffer->Deactivate(context);
	}
	/*****************GameSceneの描画 ******************/
	else
	{
		FLOAT4X4 viewProjection;

		DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
		pLight.ConstanceLightBufferSetShader(context);
		/************************カラーマップテクスチャの作成***********************/

		frameBuffer->Clear(context);
		frameBuffer->Activate(context);

		blend[0]->activate(context);

		modelRenderer->Begin(context, viewProjection);
		modelRenderer->Draw(context, *player->GetCharacter()->GetModel());
		modelRenderer->End(context);
		pGpuParticleManager.Render(context, view, projection);

		mSManager->Render(context, view, projection, mStageOperation->GetColorType());
		UIManager::GetInctance().Render(context);

		blend[0]->deactivate(context);
		frameBuffer->Deactivate(context);
	}
	/****************ブルームをかける******************/
	blend[1]->activate(context);
	siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);
	blend[1]->deactivate(context);
	blend[0]->activate(context);
	if (editorNo == 3)mSManager->SidoViewRender(context);

	fadeOut->Render(context);
	blend[0]->deactivate(context);
	//siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

}

SceneGame::~SceneGame()
{
	pCameraManager.DestroyCamera();
}



