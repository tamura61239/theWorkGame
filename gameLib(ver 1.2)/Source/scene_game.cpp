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
#include"scene_title.h"
#include"scene_select.h"
#include"scene_result.h"

SceneGame::SceneGame(int stageNo) : testGame(false), hitArea(false), mNowLoading(true), mLoadEnd(false), mStageNo(stageNo)
{

}

void SceneGame::Initialize(ID3D11Device* device)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);

			//カメラ
			pCameraManager->Initialize(device, 1);
			pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);

			//ディファードレンダリング用のテクスチャを作成
			frameBuffer = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer3 = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
			velocityMap = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
			frameBuffer2 = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			shadowMap = std::make_unique<FrameBuffer>(device, 1024 * 5, 1024 * 5, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
			shadowRenderBuffer = std::make_shared<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			//影を付けるようクラスの作成
			mRenderEffects = std::make_unique<RenderEffects>(device, "testShadow");
			//GPUパーティクルマネージャーを作成
			GpuParticleManager::Create();
			player = std::make_shared<PlayerAI>(device, "Data/FBX/new_player_anim.fbx");
			//PUパーティクルマネージャーにプレイヤーのデータをセットする
			pGpuParticleManager->CreateGameBuffer(device, player);

			mSManager = std::make_unique<StageManager>(device, SCREEN_WIDTH, SCREEN_HEIGHT);
			mSManager->SetStageNo(mStageNo);
			mSManager->Load();
			mModelRenderer = std::make_unique<ModelRenderer>(device);
			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1);
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
				mBlurShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/zoom_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
				motionBlurShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/motion_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
			}
			mSky = std::make_unique<SkyMap>(device, L"Data/image/sor_sea.dds", MAPTYPE::BOX);
			{
				FILE* fp;
				if (fopen_s(&fp, "Data/file/game_sky_map.bin", "rb") == 0)
				{
					fread(mSky->GetPosData(), sizeof(Obj3D), 1, fp);
					fclose(fp);
				}
			}

			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::GAME);
			mFade->StartFadeIn();
			HitAreaRender::Create();
			HitAreaRender::GetInctance()->Initialize(device);
			HitAreaRender::GetInctance()->SetObjSize(mSManager->GetStages().size() + 1);

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

			mPhotographTargets.resize(6);
		}, device);
	test = std::make_unique<Sprite>(device, L"Data/image/操作説明.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/now.png");
	siro = std::make_unique<Sprite>(device, L"Data/image/siro.png");
	pushKey = std::make_unique<Sprite>(device, L"Data/image/push key.png");
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	textureNo = 0;
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_LESS_EQUAL));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));


	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);

}

void SceneGame::Editor()
{
#ifdef USE_IMGUI
	if (mNowLoading)
	{
		return;
	}
	/********************Sceneの選択結果処理***************************/
	bool beforeEditorFlag = mEditorFlag;
	switch (pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount()))
	{
	case SceneManager::SCENETYPE::TITLE:

		pSceneManager.ChangeScene(new SceneTitle());
		break;
	case SceneManager::SCENETYPE::SELECT:
		pSceneManager.ChangeScene(new SceneSelect());

		break;
	case SceneManager::SCENETYPE::GAME:
		mSManager->Clear();
		mSManager->SetStageNo(pSceneManager.GetSceneEditor()->GetStageNo());
		mSManager->Load();
		break;
	case SceneManager::SCENETYPE::RESULT:
		pSceneManager.ChangeScene(new SceneResult(0.f,mStageNo));
		break;
	}
	//editorFlagがfalseの時
	if (!mEditorFlag)
	{
		if (beforeEditorFlag && mFade->GetFadeScene() == Fade::FADE_MODO::NONE)
		{
			UIManager::GetInctance()->GetGameUIMove()->Start();
		}
		return;
	}
	if (!beforeEditorFlag)
	{
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
	//前のフレームのeditorNoを保存
	int beforeEditorNo = mEditorNo;
	//エディターの名前を決める
	std::string editorName = { "scene game" };
	/*******************Editor****************/
	ImGui::Begin("scene game");
	ImGui::Checkbox("stop", &mStopTime);
	ImGui::Checkbox("hitArea", &hitArea);
	ImGui::SliderFloat("time", &mElapsdTimeSpeed, 0, 1);

#ifdef _DEBUG

	if (ImGui::CollapsingHeader("screen shot"))
	{
		ImGui::Selectable("player", &mPhotographTargets[0]);
		ImGui::Selectable("stage", &mPhotographTargets[1]);
		ImGui::Selectable("particle", &mPhotographTargets[2]);
		ImGui::Selectable("ui", &mPhotographTargets[3]);
		ImGui::Selectable("hitarea", &mPhotographTargets[4]);
		ImGui::Selectable("sky map", &mPhotographTargets[5]);
		ImGui::InputInt("No", &textureNo, 1);
		if (ImGui::Button("photograph"))
		{
			mScreenShot = true;
		}
	}
#endif
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

	ImGui::RadioButton("LIGHT", &mEditorNo, 2);
	ImGui::RadioButton("STAGE", &mEditorNo, 3);
	ImGui::RadioButton("PLAYER", &mEditorNo, 4);
	ImGui::RadioButton("UI", &mEditorNo, 9);
	ImGui::RadioButton("SKY MAP", &mEditorNo, 10);
	ImGui::RadioButton("SHADOW MAP", &mEditorNo, 11);
	ImGui::RadioButton("LIGHT VIEW", &mEditorNo, 12);
	ImGui::RadioButton("TUTORIAL", &mEditorNo, 13);
	ImGui::RadioButton("ZOOM BLUR", &mEditorNo, 14);

	ImGui::RadioButton("GPU PARTICLE", &mEditorNo, 5);
	ImGui::RadioButton("CAMERA", &mEditorNo, 6);
	ImGui::RadioButton("BLOOM", &mEditorNo, 7);
	ImGui::RadioButton("FADE", &mEditorNo, 8);
	ImGui::RadioButton("NONE", &mEditorNo, 0);
	//ゲームシーンの時
	if (beforeEditorNo == 3)
	{
		if (beforeEditorNo != mEditorNo)
		{
			pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
		}
	}
	ImGui::End();
	/******************何番目のエディターを操作するか*********************/
	switch (mEditorNo)
	{
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
		mBloom->ImGuiUpdate();
		break;
	case 8:
		mFade->ImGuiUpdate();
		break;
	case 9:
		UIManager::GetInctance()->ImGuiUpdate();
		break;
	case 11:
		mRenderEffects->ImGuiUpdate();

		break;
	case 12:
		mLightView->ImGuiUpdate();
		break;
	case 13:
		mTutorialState->ImGuiUpdate();
		break;

	}
	if (mEditorNo == 10)
	{
		ImGui::Begin("sky map");
		float* position[3] = { &mSky->GetPosData()->GetPosition().x,&mSky->GetPosData()->GetPosition().y ,&mSky->GetPosData()->GetPosition().z };
		ImGui::DragFloat3("position", *position, 10);
		float* scale[3] = { &mSky->GetPosData()->GetScale().x,&mSky->GetPosData()->GetScale().y ,&mSky->GetPosData()->GetScale().z };
		ImGui::DragFloat3("scale", *scale, 10);
		float* color[4] = { &mSky->GetPosData()->GetColor().x,&mSky->GetPosData()->GetColor().y ,&mSky->GetPosData()->GetColor().z ,&mSky->GetPosData()->GetColor().w };
		ImGui::ColorEdit4("color", *color);
		if (ImGui::Button("save"))
		{
			FILE* fp;
			fopen_s(&fp, "Data/file/game_sky_map.bin", "wb");
			fwrite(mSky->GetPosData(), sizeof(Obj3D), 1, fp);
			fclose(fp);
		}
		ImGui::End();
		return;
	}
	if (mEditorNo == 14)
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
	if (mStopTime)
	{
		pCameraManager->Update(elapsed_time);
		return;
	}
	elapsed_time *= mElapsdTimeSpeed;
#endif
	mFade->Update(elapsed_time);
	/**********************GameSceneの更新*******************************/
	if (mSManager->GrtStageNo() == 0)
	{
		{
			mStageOperation->Update(elapsed_time, mSManager.get(), player->GetPlayFlag() && (mTutorialState->GetKeyFlag()));
		}
		elapsed_time *= mTutorialState->Update(elapsed_time, player->GetCharacter());
	}
	else
	{
		mStageOperation->Update(elapsed_time, mSManager.get(), player->GetPlayFlag());
	}

	//フェートインの時
	if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEIN)
	{
		if (mFade->GetEndFlag())
		{
			mFade->Clear();
			if (!mEditorFlag)UIManager::GetInctance()->GetGameUIMove()->Start();
		}
	}
	//フェートインの時
	else if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
	{

		if (mFade->GetEndFlag())
		{
			pSceneManager.ChangeScene(new SceneResult(UIManager::GetInctance()->GetGameUIMove()->GetTime(), mStageNo));
			//pSceneManager.ChangeScene(new SceneSelect);
			return;
		}

	}
	//フェートインでもフェードアウトでもない時
	else
	{
		UIManager::GetInctance()->Update(elapsed_time);

		//カウントが0かつStartFlagがtrueの時
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
			if (!testGame) mFade->StartFadeOut();
			elapsed_time *= 0.3f;
		}
	}

	mSky->GetPosData()->CalculateTransform();
	player->Update(elapsed_time, mSManager.get(), mStageOperation.get());


	mSManager->Update(elapsed_time);
	pCameraManager->Update(elapsed_time);
	pGpuParticleManager->Update(elapsed_time);
}
/**********************Editor*********************/

/***************************************************************/
//                          描画
/***************************************************************/

void SceneGame::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);

	mSampler[samplerType::warp]->Activate(context, 0, false, true);
	mSampler[samplerType::border]->Activate(context, 1, false, true);
	mSampler[samplerType::clamp]->Activate(context, 2, false, true);

	mDepth->Activate(context);
	mRasterizer->Activate(context);

	if (mNowLoading)
	{
		mBlend[0]->activate(context);

		if (mLoadEnd)mNowLoading = false;
		test->Render(context, VECTOR2F(0, 0), VECTOR2F(viewport.Width, viewport.Height), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		if (IsNowLoading())nowLoading->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, LoadColor(elapsed_time));
		else pushKey->Render(context, VECTOR2F(1300, 900), VECTOR2F(575, 90), VECTOR2F(0, 0), VECTOR2F(230, 36), 0, LoadColor(elapsed_time));
		mBlend[0]->deactivate(context);
		mSampler[samplerType::warp]->DeActivate(context);
		mSampler[samplerType::border]->DeActivate(context);

		mDepth->DeActive(context);
		mRasterizer->DeActivate(context);

		return;
	}

	//view projection行列の取得
	FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
	FLOAT4X4 projection = pCameraManager->GetCamera()->GetProjection();

	/*****************GameSceneの描画 ******************/
	FLOAT4X4 viewProjection;

	DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
	pLight.ConstanceLightBufferSetShader(context);
	/************************カラーマップテクスチャの作成***********************/

	frameBuffer3->Clear(context);
	frameBuffer3->Activate(context);
	mBlend[0]->activate(context);

	if (mSManager->GetStageEditor()->GetEditorFlag())
	{
		mModelRenderer->Begin(context, viewProjection);
		mModelRenderer->Draw(context, *player->GetCharacter()->GetModel(), VECTOR4F(0.5, 0.5, 0.5, 1));
		mModelRenderer->End(context);

		mSManager->Render(context, view, projection, mStageOperation->GetColorType());
	}
	else
	{
		mSky->Render(context, view, projection);
		pGpuParticleManager->Render(context, view, projection);
		mModelRenderer->Begin(context, viewProjection);
		mModelRenderer->Draw(context, *player->GetCharacter()->GetModel(), VECTOR4F(0.5, 0.5, 0.5, 1));
		mModelRenderer->End(context);

		mSManager->Render(context, view, projection, mStageOperation->GetColorType());
	}
	frameBuffer3->Deactivate(context);

	/************************速度マップテクスチャの作成***********************/
	velocityMap->Clear(context);
	velocityMap->Activate(context);
	pCameraManager->GetCamera()->BeforeActive(context, 5, true, true, true);
	mModelRenderer->VelocityBegin(context, viewProjection);
	mModelRenderer->VelocityDraw(context, *player->GetCharacter()->GetModel());
	mModelRenderer->VelocityEnd(context);
	pCameraManager->GetCamera()->BeforeDactive(context);
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
	mModelRenderer->ShadowBegin(context, lightVP);
	mModelRenderer->ShadowDraw(context, *player->GetCharacter()->GetModel());
	mModelRenderer->ShadowEnd(context);
	mSManager->RenderShadow(context, lightV, lightP);

	shadowMap->Deactivate(context);
	//シーンに影を付ける
	shadowRenderBuffer->Clear(context);
	shadowRenderBuffer->Activate(context);
	mRenderEffects->ShadowRender(context, frameBuffer3->GetRenderTargetShaderResourceView().Get(), frameBuffer3->GetDepthStencilShaderResourceView().Get(), shadowMap->GetDepthStencilShaderResourceView().Get()
		, view, projection, lightV, lightP);
	shadowRenderBuffer->Deactivate(context);

	frameBuffer->Clear(context);
	frameBuffer->Activate(context);
	//シーンにモーションブラーをかける
	velocityMap->SetPsTexture(context, 1);
	siro->Render(context, motionBlurShader.get(), shadowRenderBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(1, 1, &srv);
	//シーンにUIを追加
	UIManager::GetInctance()->Render(context);
	mTutorialState->RenderButton(context);

	mBlend[0]->deactivate(context);
	frameBuffer->Deactivate(context);
	/****************ブルームをかける******************/
	mBlend[1]->activate(context);
	mBloom->BlurTexture(context, frameBuffer->GetRenderTargetShaderResourceView().Get());

	frameBuffer2->Clear(context);
	frameBuffer2->Activate(context);
	siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get());
	frameBuffer2->Deactivate(context);
	mBlend[1]->deactivate(context);

	mBlend[0]->activate(context);

	if (player->GetCharacter()->GetGorlFlag())
	{//ズームブラー
		mCbZoomBuffer->Activate(context, 0, true, true);
		siro->Render(context, mBlurShader.get(), frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		mCbZoomBuffer->DeActivate(context);
	}
	else if (mTutorialState->GetState() == 1 || mTutorialState->GetState() == 3)
	{//チュートリアル
		mTutorialState->GetCbZoom()->Activate(context, 0, true, true);
		siro->Render(context, mBlurShader.get(), frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0, mTutorialState->GetBackGroundColor());
		mTutorialState->GetCbZoom()->DeActivate(context);

	}
	else
	{//普段
		siro->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	}
	if (mScreenShot)
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


	if (mEditorNo == 3)mSManager->SidoViewRender(context);
	mFade->Render(context);
	mTutorialState->RenderText(context);

	mBlend[0]->deactivate(context);
	mSampler[samplerType::warp]->DeActivate(context);
	mSampler[samplerType::border]->DeActivate(context);
	mSampler[samplerType::clamp]->DeActivate(context);

	mDepth->DeActive(context);
	mRasterizer->DeActivate(context);

	HitAreaRender::GetInctance()->ClearCount();
}


void SceneGame::Relese()
{
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
	UIManager::GetInctance()->ClearUI();
}



