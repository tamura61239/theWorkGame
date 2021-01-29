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
#include <codecvt>
#include <locale>
#include"screen_size.h"

SceneGame::SceneGame(ID3D11Device* device) : selectSceneFlag(true), editorFlag(false), testGame(false), hitArea(false), screenShot(false), mNowLoading(true), mLoadEnd(false)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			pCameraManager->Initialize(device, 1);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));

			frameBuffer = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer3 = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
			velocityMap = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
			saveFrameBuffer = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer2 = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			shadowMap = std::make_unique<FrameBuffer>(device, 1024 * 5, 1024 * 5, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
			shadowRenderBuffer = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);

			renderEffects = std::make_unique<RenderEffects>(device, "testShadow");
			GpuParticleManager::Create();
			player = std::make_shared<PlayerAI>(device, "Data/FBX/new_player_anim.fbx");
			pGpuParticleManager->CreateGameBuffer(device, player);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::SELECT);
#if (RUNPARTICLE_TYPE==1)
			pGpuParticleManager->GetRunParticle()->SetMeshData(player->GetCharacter()->GetModel(), device);
#endif
			mSManager = std::make_unique<StageManager>(device, SCREEN_WIDTH, SCREEN_HEIGHT);
			modelRenderer = std::make_unique<ModelRenderer>(device);
			bloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1);
			mStageOperation = std::make_unique<StageOperation>();
			pHitAreaDrow.CreateObj(device);
			pLight.CreateLightBuffer(device);
			//sky = std::make_unique<SkyMap>(device, L"Data/image/mp_totality.dds", MAPTYPE::BOX);
			{
				D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

				};
				blurShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/zoom_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
				motionBlurShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/motion_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
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
			mLightView = std::make_unique<LightView>(device, "LightViewData1");
			mTutorialState = std::make_unique<TutorialState>(device);
			mCbZoomBuffer = std::make_unique<ConstantBuffer<CbZoom>>(device);
			{
				FILE* fp;
				if (fopen_s(&fp, "Data/file/game_zoom_blur_parameter.bin", "rb") == 0)
				{
					fread(&mCbZoomBuffer->data, sizeof(mCbZoomBuffer->data), 1, fp);
					fclose(fp);
				}

			}
		}, device);
	test = std::make_unique<Sprite>(device, L"Data/image/操作説明.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/now.png");
	siro = std::make_unique<Sprite>(device, L"Data/image/siro.png");
	pushKey = std::make_unique<Sprite>(device, L"Data/image/push key.png");
	blend[0] = std::make_unique<BlendState>(device, BLEND_MODE::ALPHA);
	blend[1] = std::make_unique<BlendState>(device, BLEND_MODE::ADD);
	blend[2] = std::make_unique<BlendState>(device, BLEND_MODE::NONE);
	stop = false;
	editorNo = 0;
	textureNo = 0;
}


/***************************************************************/
//                          更新
/***************************************************************/
void SceneGame::Update(float elapsed_time)
{
	if (mNowLoading)
	{

		if (!IsNowLoading() && pKeyBoad.RisingState(KeyLabel::SPACE))
		{
			mLoadEnd = true;
		}
		return;
	}
	EndLoading();
	/********************Editor************************/
#ifdef USE_IMGUI
	if (ImGuiUpdate())return;
	if (stop)
	{
		pCameraManager->Update(elapsed_time);
		return;
	}
	elapsed_time *= elapsedTimemMagnification;
#endif
	fadeOut->Update(elapsed_time);
	/**********************SelectSceneの更新*******************************/
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
	/**********************GameSceneの更新*******************************/

	//フェートインの時
	if (fadeOut->GetFadeScene() == Fade::FADE_MODO::FADEIN)
	{
		if (fadeOut->GetEndFlag())
		{
			fadeOut->Clear();
			if (!editorFlag)UIManager::GetInctance()->GetGameUIMove()->Start();
		}
	}
	//フェートインの時
	else if (fadeOut->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
	{

		if (fadeOut->GetEndFlag())
		{
			fadeOut->Clear();
			UIManager::GetInctance()->ClearUI();
			Ranking::SetStageNo(mSManager->GrtStageNo());
			Relese();

			pSceneManager.ChangeScene(SceneManager::SCENETYPE::RESULT);

			return;
		}

	}
	//フェートインでもフェードアウトでもない時
	else
	{
		//カウントが0かつStartFlagがtrueの時
		if (UIManager::GetInctance()->GetGameUIMove()->GetCount() <= 0)
		{
			if (UIManager::GetInctance()->GetGameUIMove()->GetStartFlag())
			{
				if (!player->GetPlayFlag())player->SetPlayFlag(true);
			}
		}

	}
	if (mSManager->GrtStageNo() == 0)
	{
		{
			mStageOperation->Update(elapsed_time, mSManager.get(), player->GetPlayFlag()&&(mTutorialState->GetKeyFlag()));
		}
		elapsed_time *= mTutorialState->Update(elapsed_time, player->GetCharacter());
	}
	else
	{
		mStageOperation->Update(elapsed_time, mSManager.get(), player->GetPlayFlag());
	}
	UIManager::GetInctance()->Update(elapsed_time);

	if (player->GetPlayFlag())
	{
		if (UIManager::GetInctance()->GetGameUIMove()->GetTime() <= 0 || player->GetCharacter()->GetGorlFlag())
		{
			if (!testGame) fadeOut->StartFadeOut();
			elapsed_time *= 0.3f;
		}
	}

	sky->GetPosData()->CalculateTransform();
	player->Update(elapsed_time, mSManager.get(), mStageOperation.get());


	mSManager->Update(elapsed_time);
	pCameraManager->Update(elapsed_time);
	pGpuParticleManager->Update(elapsed_time);
}
/**********************Editor*********************/
bool SceneGame::ImGuiUpdate()
{
#ifdef USE_IMGUI
	/********************Sceneの選択結果処理***************************/
	switch (pSceneManager.GetSceneEditor()->Editor(&editorFlag, StageManager::GetMaxStageCount()))
	{
	case 1:
		UIManager::Destroy();
		Relese();
		pSceneManager.ChangeScene(SceneManager::SCENETYPE::TITLE);

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
		UIManager::GetInctance()->ClearUI();
		Ranking::SetStageNo(mSManager->GrtStageNo());
		Relese();
		pSceneManager.ChangeScene(SceneManager::SCENETYPE::RESULT);

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
	screenShot = false;
	ImGui::Begin(editorName.c_str());
	ImGui::Checkbox("stop", &stop);
	ImGui::Checkbox("hitArea", &hitArea);
	ImGui::SliderFloat("time", &elapsedTimemMagnification, 0, 1);
	if (selectSceneFlag)
	{
		if (ImGui::CollapsingHeader("screen shot"))
		{
			ImGui::InputInt("No", &textureNo, 1);
			if (ImGui::Button("photograph"))
			{
				screenShot = true;
			}
		}

		ImGui::RadioButton("SELECT_SCENE", &editorNo, 1);
	}
	else
	{
		if (ImGui::CollapsingHeader("screen shot"))
		{
			ImGui::Selectable("player", &target[0]);
			ImGui::Selectable("stage", &target[1]);
			ImGui::Selectable("particle", &target[2]);
			ImGui::Selectable("ui", &target[3]);
			ImGui::Selectable("hitarea", &target[4]);
			ImGui::Selectable("sky map", &target[5]);
			ImGui::InputInt("No", &textureNo, 1);
			if (ImGui::Button("photograph"))
			{
				screenShot = true;
			}
		}
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
				mTutorialState->ResetParameter();
			}

		}

		ImVec2 view = ImVec2(192 * 3, 108 * 3);
		ImGui::Image(shadowMap->GetDepthStencilShaderResourceView().Get(), view); ImGui::SameLine();
		ImGui::Image(shadowMap->GetRenderTargetShaderResourceView().Get(), view);

		ImGui::RadioButton("LIGHT", &editorNo, 2);
		ImGui::RadioButton("STAGE", &editorNo, 3);
		ImGui::RadioButton("PLAYER", &editorNo, 4);
		ImGui::RadioButton("UI", &editorNo, 9);
		ImGui::RadioButton("SKY MAP", &editorNo, 10);
		ImGui::RadioButton("SHADOW MAP", &editorNo, 11);
		ImGui::RadioButton("LIGHT VIEW", &editorNo, 12);
		ImGui::RadioButton("TUTORIAL", &editorNo, 13);
		ImGui::RadioButton("ZOOM BLUR", &editorNo, 14);
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
				pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
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
		pGpuParticleManager->ImGuiUpdate();
		break;
	case 6:
		pCameraManager->ImGuiUpdate();
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
	case 11:
		renderEffects->ImGuiUpdate();

		break;
	case 12:
		mLightView->ImGuiUpdate();
		break;
	case 13:
		mTutorialState->ImGuiUpdate();
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
	else if (editorNo == 14)
	{
		ImGui::Begin("zoom blur");
		ImGui::InputFloat("length", &mCbZoomBuffer->data.lenght, 0.1f);
		ImGui::InputInt("division", &mCbZoomBuffer->data.division, 1);
		if (ImGui::Button("save"))
		{
			FILE* fp;
			fopen_s(&fp, "Data/file/game_zoom_blur_parameter.bin", "wb");
			fwrite(&mCbZoomBuffer->data, sizeof(mCbZoomBuffer->data), 1, fp);
			fclose(fp);

		}
		ImGui::End();
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

	if (mNowLoading)
	{
		if (mLoadEnd)mNowLoading = false;
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
		test->Render(context, VECTOR2F(0, 0), VECTOR2F(viewport.Width, viewport.Height), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		if (IsNowLoading())nowLoading->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, color);
		else pushKey->Render(context, VECTOR2F(1300, 900), VECTOR2F(575, 90), VECTOR2F(0, 0), VECTOR2F(230, 36), 0, color);
		blend[0]->deactivate(context);

		return;
	}

	//view projection行列の取得
	FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
	FLOAT4X4 projection = pCameraManager->GetCamera()->GetProjection();

	/*****************SelectSceneの描画 ******************/
	if (selectSceneFlag)
	{
		frameBuffer->Clear(context);
		frameBuffer->Activate(context);
		blend[0]->activate(context);
		if (screenShot)
		{
			pGpuParticleManager->Render(context, view, projection);
		}
		else
		{
			pGpuParticleManager->Render(context, view, projection);


			mStageSelect->Render(context);

		}
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

		frameBuffer3->Clear(context);
		frameBuffer3->Activate(context);
		blend[0]->activate(context);

		if (mSManager->GetStageEditor()->GetEditorFlag())
		{
			modelRenderer->Begin(context, viewProjection);
			modelRenderer->Draw(context, *player->GetCharacter()->GetModel(), VECTOR4F(0.5, 0.5, 0.5, 1));
			modelRenderer->End(context);

			mSManager->Render(context, view, projection, mStageOperation->GetColorType());
		}
		else
		{
			sky->Render(context, view, projection);
			pGpuParticleManager->Render(context, view, projection);
			modelRenderer->Begin(context, viewProjection);
			modelRenderer->Draw(context, *player->GetCharacter()->GetModel(), VECTOR4F(0.5, 0.5, 0.5, 1));
			modelRenderer->End(context);

			mSManager->Render(context, view, projection, mStageOperation->GetColorType());
			if (hitArea)HitAreaRender::GetInctance()->Render(context, view, projection);
		}
		frameBuffer3->Deactivate(context);

		/************************速度マップテクスチャの作成***********************/
		velocityMap->Clear(context);
		velocityMap->Activate(context);
		pCameraManager->GetCamera()->ShaderSetBeforeBuffer(context, 5);
		modelRenderer->VelocityBegin(context, viewProjection);
		modelRenderer->VelocityDraw(context, *player->GetCharacter()->GetModel());
		modelRenderer->VelocityEnd(context);
		pGpuParticleManager->VelocityRender(context, view, projection);

		mSManager->RenderVelocity(context, view, projection, mStageOperation->GetColorType());

		velocityMap->Deactivate(context);
		/************************シャドウマップテクスチャの作成***********************/
		shadowMap->Clear(context);
		shadowMap->Activate(context);

		//light視点のカメラの更新と情報の取得
		mLightView->Update(player->GetCharacter()->GetPosition(), context);
		FLOAT4X4 lightVP, lightV = mLightView->GetLightCamera()->GetView(), lightP = mLightView->GetLightCamera()->GetProjection();
		DirectX::XMStoreFloat4x4(&lightVP, DirectX::XMLoadFloat4x4(&lightV) * DirectX::XMLoadFloat4x4(&lightP));

		//light視点から見たシーンの描画
		modelRenderer->ShadowBegin(context, lightVP);
		modelRenderer->ShadowDraw(context, *player->GetCharacter()->GetModel());
		modelRenderer->ShadowEnd(context);
		mSManager->RenderShadow(context, lightV, lightP);

		shadowMap->Deactivate(context);

		shadowRenderBuffer->Clear(context);
		shadowRenderBuffer->Activate(context);
		renderEffects->ShadowRender(context, frameBuffer3->GetRenderTargetShaderResourceView().Get(), frameBuffer3->GetDepthStencilShaderResourceView().Get(), shadowMap->GetDepthStencilShaderResourceView().Get()
			, view, projection, lightV, lightP);
		shadowRenderBuffer->Deactivate(context);

		frameBuffer->Clear(context);
		frameBuffer->Activate(context);
		//renderEffects->ShadowRender(context, frameBuffer3->GetRenderTargetShaderResourceView().Get(), frameBuffer3->GetDepthStencilShaderResourceView().Get(), shadowMap->GetDepthStencilShaderResourceView().Get()
		//	, view, projection, lightV, lightP);

		velocityMap->SetPsTexture(context, 1);
		siro->Render(context, motionBlurShader.get(), shadowRenderBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		//if (screenShot) 
		//{
		//	if (target[0])
		//	{
		//		modelRenderer->Begin(context, viewProjection);
		//		modelRenderer->Draw(context, *player->GetCharacter()->GetModel(), VECTOR4F(0.5, 0.5, 0.5, 1));
		//		modelRenderer->End(context);
		//	}
		//	
		//	if (target[5])sky->RenderButton(context, view, projection);
		//	if (target[2])pGpuParticleManager->RenderButton(context, view, projection);

		//	if(target[1])mSManager->RenderButton(context, view, projection, mStageOperation->GetColorType());
		//	if (target[4])if (hitArea)HitAreaRender::GetInctance()->RenderButton(context, view, projection);
		//	if (target[3])UIManager::GetInctance()->RenderButton(context);

		//}
		//else
		//{
		//	modelRenderer->Begin(context, viewProjection);
		//	modelRenderer->Draw(context, *player->GetCharacter()->GetModel(), VECTOR4F(0.5, 0.5, 0.5, 1));
		//	modelRenderer->End(context);
		//	if (mSManager->GetStageEditor()->GetEditorFlag())
		//	{
		//		mSManager->RenderButton(context, view, projection, mStageOperation->GetColorType());
		//	}
		//	else
		//	{
		//		sky->RenderButton(context, view, projection);
		//		pGpuParticleManager->RenderButton(context, view, projection);

		//		mSManager->RenderButton(context, view, projection, mStageOperation->GetColorType());
		//		if (hitArea)HitAreaRender::GetInctance()->RenderButton(context, view, projection);
		//		UIManager::GetInctance()->RenderButton(context);
		//	}

		//}

		UIManager::GetInctance()->Render(context);
		mTutorialState->RenderButton(context);

		blend[0]->deactivate(context);
		frameBuffer->Deactivate(context);
	}
	/****************ブルームをかける******************/
	frameBuffer2->Clear(context);
	frameBuffer2->Activate(context);
	blend[1]->activate(context);
	siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);
	blend[1]->deactivate(context);
	frameBuffer2->Deactivate(context);
	blend[0]->activate(context);

	if (player->GetCharacter()->GetGorlFlag())
	{
		mCbZoomBuffer->Activate(context, 0, true, true);
		siro->Render(context, blurShader.get(), frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		mCbZoomBuffer->DeActivate(context);
	}
	else if (mTutorialState->GetState() == 1 || mTutorialState->GetState() == 3)
	{
		mTutorialState->GetCbZoom()->Activate(context, 0, true, true);
		siro->Render(context, blurShader.get(), frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0, mTutorialState->GetBackGroundColor());
		mTutorialState->GetCbZoom()->DeActivate(context);

	}
	else
	{
		siro->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	}
	if (screenShot)
	{
		std::wstring fileName = L"Data/image/screen_shot/screenShot" + std::to_wstring(textureNo) + L".dds";
		frameBuffer2->SaveDDSFile(context, fileName.c_str(), frameBuffer2->GetRenderTargetShaderResourceView().Get());
	}
	else if (mSManager->GetStageEditor()->GetEditorFlag())
	{
		if (mSManager->GetStageEditor()->GetSceneSaveFlag())
		{
			std::wstring fileName = L"Data/image/stage" + std::to_wstring(mSManager->GrtStageNo()) + L"scne_map.dds";
			frameBuffer2->SaveDDSFile(context, fileName.c_str(), frameBuffer2->GetRenderTargetShaderResourceView().Get());
		}
	}


	if (editorNo == 3)mSManager->SidoViewRender(context);
	fadeOut->Render(context);
	mTutorialState->RenderText(context);

	blend[0]->deactivate(context);

	//siro->RenderButton(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	HitAreaRender::GetInctance()->ClearCount();
}

SceneGame::~SceneGame()
{


}

void SceneGame::Relese()
{
	pCameraManager->DestroyCamera();
	HitAreaRender::Destroy();
	GpuParticleManager::Destroy();
}



