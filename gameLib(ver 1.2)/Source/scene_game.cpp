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
#include"ranking.h"
#include"hit_area_render.h"
//#ifdef USE_IMGUI
//#include <imgui.h>
//#include <imgui_impl_dx11.h>
//#include <imgui_impl_win32.h>
//#include <imgui_internal.h>
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
//#endif

SceneGame::SceneGame(ID3D11Device* device) : selectSceneFlag(true), editorFlag(false), testGame(false), hitArea(false)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			pCameraManager->Initialize(device, 1);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));

			frameBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			saveFrameBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer2 = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			for (int i = 1; i <= 2; i++)
			{
				shrinkBuffer[i - 1] = std::make_unique<FrameBuffer>(device, 1920 >> i, 1080 >> i, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
			}
			GpuParticleManager::Create();
			pGpuParticleManager->CreateGameBuffer(device);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::SELECT);
			player = std::make_unique<PlayerAI>(device, "Data/FBX/new_player_anim.fbx");
#if (RUNPARTICLE_TYPE==1)
			pGpuParticleManager->GetRunParticle()->SetMeshData(player->GetCharacter()->GetModel(), device);
#endif
			mSManager = std::make_unique<StageManager>(device, 1920, 1080);
			modelRenderer = std::make_unique<ModelRenderer>(device);
			bloom = std::make_unique<BloomRender>(device, 1920, 1080, 1);
			mStageOperation = std::make_unique<StageOperation>();
			pHitAreaDrow.CreateObj(device);
			pLight.CreateLightBuffer(device);
			sky = std::make_unique<SkyMap>(device, L"Data/image/mp_totality.dds", MAPTYPE::BOX);
			{
				D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

				};
				blurShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/zoom_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
			}
			sky = std::make_unique<SkyMap>(device, L"Data/image/sor_sea.dds", MAPTYPE::BOX);
			{
				FILE* fp;
				if (fopen_s(&fp, "Data/file/game_sky_map.bin", "rb") == 0)
				{
					fread(sky->GetPosData(), sizeof(Obj3D), 1, fp);
					fclose(fp);
				}
			}

			mStageSelect = std::make_unique<StageSelect>(device, StageManager::GetMaxStageCount());
			fadeOut = std::make_unique<Fade>(device, Fade::FADE_SCENE::SELECT);
			HitAreaRender::Create();
			HitAreaRender::GetInctance()->Initialize(device);

			if (pSceneManager.GetSceneEditor()->GetSceneNo() == 3)
			{
				mSManager->SetStageNo(pSceneManager.GetSceneEditor()->GetStageNo());
				mSManager->Load();
				HitAreaRender::GetInctance()->SetObjSize(mSManager->GetStages().size() + 1);
				selectSceneFlag = false;
				fadeOut->Clear();
				fadeOut->SetFadeScene(Fade::FADE_SCENE::GAME);
				pGpuParticleManager->SetState(GpuParticleManager::STATE::GAME);
				pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
				fadeOut->StartFadeIn();
				bloom->SetNowScene(2);
			}
			UIManager::Create();
			UIManager::GetInctance()->GameInitialize(device);
		}, device);
	test = std::make_unique<Sprite>(device, L"Data/image/�Q�[���e�X�g.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/now.png");
	siro = std::make_unique<Sprite>(device, L"Data/image/siro.png");
	blend[0] = std::make_unique<blend_state>(device, BLEND_MODE::ALPHA);
	blend[1] = std::make_unique<blend_state>(device, BLEND_MODE::ADD);
	blend[2] = std::make_unique<blend_state>(device, BLEND_MODE::NONE);
	stop = false;
	editorNo = 0;
}
/***************************************************************/
//                          �X�V
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
	/**********************SelectScene�̍X�V*******************************/
	if (selectSceneFlag)
	{
		pCameraManager->Update(elapsed_time);
		pGpuParticleManager->Update(elapsed_time);
		mStageSelect->Update(elapsed_time, mSManager.get());
		if (fadeOut->GetEndFlag())
		{
			if (fadeOut->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
			{
				mSManager->Load();
				HitAreaRender::GetInctance()->SetObjSize(mSManager->GetStages().size() + 1);
				selectSceneFlag = false;
				mStageSelect->SetSelectFlag(true);
				fadeOut->Clear();
				fadeOut->SetFadeScene(Fade::FADE_SCENE::GAME);
				fadeOut->Clear();
				fadeOut->StartFadeIn();
				pGpuParticleManager->SetState(GpuParticleManager::STATE::GAME);
				pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
				bloom->SetNowScene(2);
				return;
			}
		}
		if (!mStageSelect->GetSelectFlag())
		{
			fadeOut->StartFadeOut();
		}
		return;

	}
	/**********************GameScene�̍X�V*******************************/
	//�t�F�[�g�C���̎�
	if (fadeOut->GetFadeScene() == Fade::FADE_MODO::FADEIN)
	{
		if (fadeOut->GetEndFlag())
		{
			fadeOut->Clear();
			if (!editorFlag)UIManager::GetInctance()->GetGameUIMove()->Start();
		}
	}
	//�t�F�[�g�C���̎�
	else if (fadeOut->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
	{

		if (fadeOut->GetEndFlag())
		{
			fadeOut->Clear();
			GpuParticleManager::Destroy();
			UIManager::GetInctance()->ClearUI();
			Ranking::SetStageNo(mSManager->GrtStageNo());
			HitAreaRender::Destroy();

			pSceneManager.ChangeScene(SCENETYPE::RESULT);

			return;
		}

	}
	//�t�F�[�g�C���ł��t�F�[�h�A�E�g�ł��Ȃ���
	else
	{
		//�S�[�����ĂȂ���
		if (!player->GetCharacter()->GetGorlFlag())
		{
			UIManager::GetInctance()->Update(elapsed_time);
		}
		//�J�E���g��0����StartFlag��true�̎�
		if (UIManager::GetInctance()->GetGameUIMove()->GetCount() <= 0)
		{
			if (UIManager::GetInctance()->GetGameUIMove()->GetStartFlag())
			{
				if (!player->GetPlayFlag())player->SetPlayFlag(true);
			}
		}

	}
	if (player->GetPlayFlag())
	{
		if (UIManager::GetInctance()->GetGameUIMove()->GetTime() <= 0 || player->GetCharacter()->GetGorlFlag())
		{
			if (!testGame) fadeOut->StartFadeOut();
			elapsed_time *= 0.3f;
		}
	}
	mStageOperation->Update(elapsed_time, mSManager.get(), player->GetPlayFlag());

	sky->GetPosData()->CalculateTransform();
	pGpuParticleManager->GetStageObjParticle()->SetParticleData(mStageOperation->GetColorType());
	player->Update(elapsed_time, mSManager.get(), mStageOperation.get());


	mSManager->Update(elapsed_time);
	pCameraManager->Update(elapsed_time);
	pGpuParticleManager->GetRunParticle()->SetBoneData(player->GetCharacter()->GetModel());
	pGpuParticleManager->GetRunParticle()->SetPlayerData(player->GetCharacter()->GetVelocity(), player->GetPlayFlag());
	pGpuParticleManager->Update(elapsed_time);
}
/**********************Editor*********************/
bool SceneGame::ImGuiUpdate()
{
#ifdef USE_IMGUI
	/********************Scene�̑I�����ʏ���***************************/
	switch (pSceneManager.GetSceneEditor()->Editor(&editorFlag, StageManager::GetMaxStageCount()))
	{
	case 1:
		GpuParticleManager::Destroy();
		HitAreaRender::Destroy();
		UIManager::Destroy();

		pSceneManager.ChangeScene(SCENETYPE::TITLE);
		return true;
		break;
	case 2:
		if (!selectSceneFlag)
		{
			selectSceneFlag = true;
			fadeOut->Clear();
			fadeOut->SetFadeScene(Fade::FADE_SCENE::SELECT);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::SELECT);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));
			pCameraManager->GetCamera()->SetFocus(VECTOR3F(0, 0, 0));
			pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::NORMAL);
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
			UIManager::GetInctance()->GetGameUIMove()->SetStartFlag(false);
			UIManager::GetInctance()->ResetGameUI();

		}
		break;
	case 3:
		if (selectSceneFlag)
		{
			editorNo = 0;
			selectSceneFlag = false;
			fadeOut->SetFadeScene(Fade::FADE_SCENE::GAME);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::GAME);
		}
		mSManager->SetStageNo(pSceneManager.GetSceneEditor()->GetStageNo());
		mSManager->Clear();
		mSManager->Load();
		HitAreaRender::GetInctance()->ClearData();
		HitAreaRender::GetInctance()->SetObjSize(mSManager->GetStages().size() + 1);
		fadeOut->Clear();
		player->SetPlayFlag(false);
		pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
		break;
	case 4:
		GpuParticleManager::Destroy();
		UIManager::GetInctance()->ClearUI();
		HitAreaRender::Destroy();

		pSceneManager.ChangeScene(SCENETYPE::RESULT);

		return true;
		break;
	}
	//editorFlag��false�̎�
	if (!editorFlag)return false;
	//�O�̃t���[����editorNo��ۑ�
	int beforeEditorNo = editorNo;
	//�G�f�B�^�[�̖��O�����߂�
	std::string editorName = { "scene game" };
	if (selectSceneFlag)editorName = "scene seletc";
	/*******************Editor****************/
	ImGui::Begin(editorName.c_str());
	ImGui::Checkbox("stop", &stop);
	ImGui::Checkbox("hitArea", &hitArea);

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
				UIManager::GetInctance()->GetGameUIMove()->Start();
			}
			if (ImGui::Button("playGame"))
			{
				UIManager::GetInctance()->GetGameUIMove()->Start();
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
				UIManager::GetInctance()->GetGameUIMove()->SetStartFlag(false);
				UIManager::GetInctance()->ResetGameUI();

			}

		}

		ImVec2 view = ImVec2(192 * 3, 108 * 3);
		ImGui::Image(frameBuffer->GetRenderTargetShaderResourceView().Get(), view); ImGui::SameLine();
		ImGui::Image(frameBuffer->GetDepthStencilShaderResourceView().Get(), view);

		ImGui::RadioButton("LIGHT", &editorNo, 2);
		ImGui::RadioButton("STAGE", &editorNo, 3);
		ImGui::RadioButton("PLAYER", &editorNo, 4);
		ImGui::RadioButton("UI", &editorNo, 9);
		ImGui::RadioButton("SKY MAP", &editorNo, 10);
	}
	ImGui::RadioButton("PARTICLE", &editorNo, 5);
	ImGui::RadioButton("CAMERA", &editorNo, 6);
	ImGui::RadioButton("BLOOM", &editorNo, 7);
	ImGui::RadioButton("FADE", &editorNo, 8);
	ImGui::RadioButton("NONE", &editorNo, 0);
	//�Q�[���V�[���̎�
	if (!selectSceneFlag)
	{
		if (beforeEditorNo == 3)
		{
			if (beforeEditorNo != editorNo)
			{
				pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
			}
		}
	}
	ImGui::End();
	/******************���Ԗڂ̃G�f�B�^�[�𑀍삷�邩*********************/
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
		pGpuParticleManager->ImGuiUpdate();
		break;
	case 6:
		pCameraManager->GetCameraOperation()->ImGuiUpdate();
		break;
	case 7:
		bloom->ImGuiUpdate();
		break;
	case 8:
		fadeOut->ImGuiUpdate();
		break;
	case 9:
		UIManager::GetInctance()->ImGuiUpdate();
		break;

	}
	if (editorNo == 10)
	{
		ImGui::Begin("sky map");
		float* position[3] = { &sky->GetPosData()->GetPosition().x,&sky->GetPosData()->GetPosition().y ,&sky->GetPosData()->GetPosition().z };
		ImGui::DragFloat3("position", *position, 10);
		float* scale[3] = { &sky->GetPosData()->GetScale().x,&sky->GetPosData()->GetScale().y ,&sky->GetPosData()->GetScale().z };
		ImGui::DragFloat3("scale", *scale, 10);
		float* color[4] = { &sky->GetPosData()->GetColor().x,&sky->GetPosData()->GetColor().y ,&sky->GetPosData()->GetColor().z ,&sky->GetPosData()->GetColor().w };
		ImGui::ColorEdit4("color", *color);
		if (ImGui::Button("save"))
		{
			FILE* fp;
			fopen_s(&fp, "Data/file/game_sky_map.bin", "wb");
			fwrite(sky->GetPosData(), sizeof(Obj3D), 1, fp);
			fclose(fp);
		}
		ImGui::End();
	}
#endif
	return false;
}


/***************************************************************/
//                          �`��
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
	//view projection�s��̎擾
	FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
	FLOAT4X4 projection = pCameraManager->GetCamera()->GetProjection();

	/*****************SelectScene�̕`�� ******************/
	if (selectSceneFlag)
	{
		frameBuffer->Clear(context);
		frameBuffer->Activate(context);
		pGpuParticleManager->Render(context, view, projection);

		blend[0]->activate(context);

		mStageSelect->Render(context);
		blend[0]->deactivate(context);
		frameBuffer->Deactivate(context);
	}
	/*****************GameScene�̕`�� ******************/
	else
	{
		FLOAT4X4 viewProjection;

		DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
		pLight.ConstanceLightBufferSetShader(context);
		/************************�J���[�}�b�v�e�N�X�`���̍쐬***********************/

		frameBuffer->Clear(context);
		frameBuffer->Activate(context);

		blend[0]->activate(context);
		modelRenderer->Begin(context, viewProjection);
		modelRenderer->Draw(context, *player->GetCharacter()->GetModel(), VECTOR4F(0.5, 0.5, 0.5, 1));
		modelRenderer->End(context);
		if (mSManager->GetStageEditor()->GetEditorFlag())
		{
			mSManager->Render(context, view, projection, mStageOperation->GetColorType());
		}
		else
		{
			pGpuParticleManager->Render(context, view, projection);
			sky->Render(context, view, projection);

			mSManager->Render(context, view, projection, mStageOperation->GetColorType());
			if (hitArea)HitAreaRender::GetInctance()->Render(context, view, projection);
			UIManager::GetInctance()->Render(context);
		}

		blend[0]->deactivate(context);
		frameBuffer->Deactivate(context);
	}
	/****************�u���[����������******************/
	frameBuffer2->Clear(context);
	frameBuffer2->Activate(context);
	blend[1]->activate(context);
	siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);
	blend[1]->deactivate(context);
	frameBuffer2->Deactivate(context);
	if (player->GetCharacter()->GetGorlFlag())
	{
		siro->Render(context, blurShader.get(), frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	}
	else
	{
		siro->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	}
	if (mSManager->GetStageEditor()->GetEditorFlag())
	{
		if (mSManager->GetStageEditor()->GetSceneSaveFlag())
		{
			std::wstring fileName = L"Data/image/stage" + std::to_wstring(mSManager->GrtStageNo()) + L"scne_map.dds";
			frameBuffer2->SaveDDSFile(context, fileName.c_str(), frameBuffer2->GetRenderTargetShaderResourceView().Get());
		}
	}

	blend[0]->activate(context);
	if (editorNo == 3)mSManager->SidoViewRender(context);

	fadeOut->Render(context);
	blend[0]->deactivate(context);

	//siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	HitAreaRender::GetInctance()->ClearCount();
}

SceneGame::~SceneGame()
{
	pCameraManager->DestroyCamera();

}



