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
#include"file_function.h"

SceneGame::SceneGame(int stageNo) : testGame(false), hitArea(false), mNowLoading(true), mLoadEnd(false), mStageNo(stageNo)
{

}
/***********************初期化関数************************/
void SceneGame::Initialize(ID3D11Device* device)
{
	//マルチスレッドでクラス変数の生成
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			//マルチスレッドの終了に必要な変数の生成
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
			
			//ステージの生成
			mSManager = std::make_unique<StageManager>(device, SCREEN_WIDTH, SCREEN_HEIGHT);
			mSManager->SetStageNo(mStageNo);
			mSManager->Load();
			//描画関連クラスの生成
			mModelRenderer = std::make_unique<ModelRenderer>(device);
			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1);
			mLightView = std::make_unique<LightView>(device, "LightViewData1");
			mCbZoomBuffer = std::make_unique<ConstantBuffer<CbZoom>>(device);
			FileFunction::Load(mCbZoomBuffer->data, "Data/file/game_zoom_blur_parameter.bin", "rb");
			mCbMotionBlur = std::make_unique<ConstantBuffer<MotionBlurParameter>>(device);
			FileFunction::Load(mCbMotionBlur->data, "Data/file/game_velocity_map_parameter.bin", "rb");
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::GAME);
			mFade->StartFadeIn();

			//ステージの操作クラスの生成
			mStageOperation = std::make_unique<StageOperation>();
			//ライトクラスの生成
			pLight.CreateLightBuffer(device);
			//シェーダーの生成
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
			//SkyMapの生成
			mSky = std::make_unique<SkyMap>(device, L"Data/image/sor_sea.dds", MAPTYPE::BOX);
			FileFunction::Load(*mSky->GetPosData(), "Data/file/game_sky_map.bin", "rb");
			//当たり判定の描画用クラス
			HitAreaRender::Create();
			HitAreaRender::GetInctance()->Initialize(device);
			HitAreaRender::GetInctance()->SetObjSize(mSManager->GetStages().size() + 1);
			//UIクラスの生成
			UIManager::Create();
			UIManager::GetInctance()->GameInitialize(device);
			//チュートリアルクラスの生成
			mTutorialState = std::make_unique<TutorialState>(device);
			mPhotographTargets.resize(6);
		}, device);
	//NowLoading中に使う変数だけ先に生成
	test = std::make_unique<Sprite>(device, L"Data/image/操作説明.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/now.png");
	siro = std::make_unique<Sprite>(device, L"Data/image/siro.png");
	pushKey = std::make_unique<Sprite>(device, L"Data/image/push key.png");
	//描画用のステートの生成
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_LESS_EQUAL));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);

	textureNo = 0;

}
/***********************エディター関数(ImGuiを使ってパラメーターを調整する)*******************/

void SceneGame::Editor()
{
#ifdef USE_IMGUI
	//マルチスレッドの処理が終わったかどうかを調べる(終わってなかったらreturn)
	if (mNowLoading)
	{
		return;
	}
	bool beforeEditorFlag = mEditorFlag;
	//シーンエディターを使ってシーン遷移する
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
	//エディターがOFFの時
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
	/*******************Editor****************/
	ImGui::Begin("scene game");
	ImGui::Checkbox("stop", &mStopTime);
	ImGui::Checkbox("hitArea", &hitArea);
	ImGui::SliderFloat("time", &mElapsdTimeSpeed, 0, 1);

#ifdef _DEBUG
	//シーンを画像として保存するかどうかを選択する
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
		//テストプレイ開始
		if (ImGui::Button("testGame"))
		{
			testGame = true;
			UIManager::GetInctance()->GetGameUIMove()->Start();
		}
		//プレイ開始
		if (ImGui::Button("playGame"))
		{
			UIManager::GetInctance()->GetGameUIMove()->Start();
		}
	}
	else
	{
		//テストプレイを終了する
		if (ImGui::Button("reset"))
		{
			testGame = false;
			//パラメーターのリセット
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
	//どのエディターを操作するか選択する
	ImGui::RadioButton("LIGHT", &mEditorNo, 2);
	ImGui::RadioButton("STAGE", &mEditorNo, 3);
	ImGui::RadioButton("PLAYER", &mEditorNo, 4);
	ImGui::RadioButton("UI", &mEditorNo, 9);
	ImGui::RadioButton("SKY MAP", &mEditorNo, 10);
	ImGui::RadioButton("SHADOW MAP", &mEditorNo, 11);
	ImGui::RadioButton("LIGHT VIEW", &mEditorNo, 12);
	ImGui::RadioButton("TUTORIAL", &mEditorNo, 13);
	ImGui::RadioButton("ZOOM BLUR", &mEditorNo, 14);
	ImGui::RadioButton("MOTION BLUR", &mEditorNo, 15);

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
	//選択されたエディタ関数を呼ぶ
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
		player->Editor();
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
		//スカイマップのパラメーターを操作する
		ImGui::Begin("sky map");
		float* position[3] = { &mSky->GetPosData()->GetPosition().x,&mSky->GetPosData()->GetPosition().y ,&mSky->GetPosData()->GetPosition().z };
		//中心座標
		ImGui::DragFloat3("position", *position, 10);
		//スケール
		float* scale[3] = { &mSky->GetPosData()->GetScale().x,&mSky->GetPosData()->GetScale().y ,&mSky->GetPosData()->GetScale().z };
		ImGui::DragFloat3("scale", *scale, 10);
		//色
		float* color[4] = { &mSky->GetPosData()->GetColor().x,&mSky->GetPosData()->GetColor().y ,&mSky->GetPosData()->GetColor().z ,&mSky->GetPosData()->GetColor().w };
		ImGui::ColorEdit4("color", *color);
		//セーブ
		if (ImGui::Button("save"))
		{
			//ファイル操作
			FileFunction::Save(*mSky->GetPosData(), "Data/file/game_sky_map.bin", "wb");
		}
		ImGui::End();
		return;
	}
	if (mEditorNo == 14)
	{
		//ズームブラーのパラメーターを操作する
		ImGui::Begin("zoom blur");
		ImGui::InputFloat("length", &mCbZoomBuffer->data.lenght, 0.1f);
		ImGui::InputInt("division", &mCbZoomBuffer->data.division, 1);
		//セーブ
		if (ImGui::Button("save"))
		{
			FileFunction::Save(mCbZoomBuffer->data, "Data/file/game_zoom_blur_parameter.bin", "wb");
		}
		ImGui::End();
	}
	if (mEditorNo == 15)
	{
		//モーションブラーのパラメーターを操作する
		ImGui::Begin("motion blur");
		ImGui::InputFloat("value", &mCbMotionBlur->data.value, 0.1f);
		//セーブ
		if (ImGui::Button("save"))
		{
			FileFunction::Save(mCbMotionBlur->data, "Data/file/game_velocity_map_parameter.bin", "wb");
		}
		ImGui::End();
	}

#endif

}


/**********************更新関数*******************************/
void SceneGame::Update(float elapsed_time)
{
	//NowLoadingのシーンかどうか
	if (mNowLoading)
	{
		//マルチスレッドの処理が終わったかどうかを調べる
		if (!IsNowLoading())
		{
			//マルチスレッドの処理が終わったらspaceキーを押せる
			if (pKeyBoad.RisingState(KeyLabel::SPACE))
			{
				mLoadEnd = true;
			}
			//マルチスレッドの終了関数
			EndLoading();
		}
		return;
	}
#ifdef USE_IMGUI
	//シーンを止める
	if (mStopTime)
	{
		pCameraManager->Update(elapsed_time);
		return;
	}
	//シーンの進行速度を変える
	elapsed_time *= mElapsdTimeSpeed;
#endif
	if (mSManager->GrtStageNo() == 0)
	{//チュートリアルのステージの時
		{
			mStageOperation->Update(elapsed_time, mSManager.get(), player->GetPlayFlag() && (mTutorialState->GetKeyFlag()));
		}
		elapsed_time *= mTutorialState->Update(elapsed_time, player->GetCharacter());
	}
	else
	{//通常時
		mStageOperation->Update(elapsed_time, mSManager.get(), player->GetPlayFlag());
	}
	//フェード時の処理
	FadeMove(elapsed_time);

	//プレイ中かどうか
	if (player->GetPlayFlag())
	{
		//プレイ中にタイムが0になるかゴールした時
		if (UIManager::GetInctance()->GetGameUIMove()->GetTime() <= 0 || player->GetCharacter()->GetGorlFlag())
		{
			if (!testGame) mFade->StartFadeOut();
			elapsed_time *= 0.3f;
		}
	}
	//シーンのオブジェクトの更新
	mSky->GetPosData()->CalculateTransform();
	player->Update(elapsed_time, mSManager.get(), mStageOperation.get());
	mSManager->Update(elapsed_time);
	pGpuParticleManager->Update(elapsed_time);
	//カメラの更新
	pCameraManager->Update(elapsed_time);
}

//フェード時やその前後の処理関数
void SceneGame::FadeMove(float elapsdTime)
{
	mFade->Update(elapsdTime);
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
			return;
		}

	}
	//フェートインでもフェードアウトでもない時
	else
	{
		UIManager::GetInctance()->Update(elapsdTime);

		//カウントが0かつStartFlagがtrueの時
		if (UIManager::GetInctance()->GetGameUIMove()->GetCount() <= 0)
		{
			if (UIManager::GetInctance()->GetGameUIMove()->GetStartFlag())
			{
				if (!player->GetPlayFlag())player->SetPlayFlag(true);
			}
		}

	}

}

/***************************描画関数************************/
void SceneGame::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//viewportの取得
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);
	//描画用のステートの設定
	mSampler[samplerType::warp]->Activate(context, 0, false, true);
	mSampler[samplerType::border]->Activate(context, 1, false, true);
	mSampler[samplerType::clamp]->Activate(context, 2, false, true);
	mDepth->Activate(context);
	mRasterizer->Activate(context);
	mBlend[0]->activate(context);

	if (mNowLoading)
	{
		//NowLoading画面の時
		if (mLoadEnd)mNowLoading = false;
		test->Render(context, VECTOR2F(0, 0), VECTOR2F(viewport.Width, viewport.Height), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		if (IsNowLoading())nowLoading->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, LoadColor(elapsed_time));
		else pushKey->Render(context, VECTOR2F(1300, 900), VECTOR2F(575, 90), VECTOR2F(0, 0), VECTOR2F(230, 36), 0, LoadColor(elapsed_time));
		//描画用のステートの解除
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

	FLOAT4X4 viewProjection;

	DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
	pLight.ConstanceLightBufferSetShader(context);
	/************************カラーマップテクスチャの作成***********************/

	frameBuffer3->Clear(context);
	frameBuffer3->Activate(context);
	//シーンの描画
	if (mSManager->GetStageEditor()->GetEditorFlag())
	{//ステージエディターの時
		mModelRenderer->Begin(context, viewProjection);
		mModelRenderer->Draw(context, *player->GetCharacter()->GetModel(), VECTOR4F(0.5, 0.5, 0.5, 1));
		mModelRenderer->End(context);

		mSManager->Render(context, view, projection, mStageOperation->GetColorType());
	}
	else
	{//通常時
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
	//定数バッファの設定
	mCbMotionBlur->Activate(context, 6, true, true);
	pCameraManager->GetCamera()->BeforeActive(context, 5, true, true, true);
	//シーンのの描画
	mModelRenderer->VelocityBegin(context, viewProjection);
	mModelRenderer->VelocityDraw(context, *player->GetCharacter()->GetModel());
	mModelRenderer->VelocityEnd(context);
	pGpuParticleManager->VelocityRender(context, view, projection);
	mSManager->RenderVelocity(context, view, projection, mStageOperation->GetColorType());
	//定数バッファの解除
	pCameraManager->GetCamera()->BeforeDactive(context);
	mCbMotionBlur->DeActivate(context);
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
    /******************************モーションブラー************************/
	frameBuffer->Clear(context);
	frameBuffer->Activate(context);
	
	velocityMap->SetPsTexture(context, 1);
	siro->Render(context, motionBlurShader.get(), shadowRenderBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(1, 1, &srv);
	//シーンにUIを追加
	UIManager::GetInctance()->Render(context);
	mTutorialState->RenderButton(context);

	mBlend[0]->deactivate(context);
	frameBuffer->Deactivate(context);
	mBlend[1]->activate(context);


	/******************************ブルーム************************/
	//ブルームの準備
	mBloom->BlurTexture(context, frameBuffer->GetRenderTargetShaderResourceView().Get());
	//シーンにブルームをかける
	frameBuffer2->Clear(context);
	frameBuffer2->Activate(context);
	siro->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get());
	frameBuffer2->Deactivate(context);
	mBlend[1]->deactivate(context);

	mBlend[0]->activate(context);
	/******************************ズームブラー************************/
	if (player->GetCharacter()->GetGorlFlag())
	{//ゴール時
		mCbZoomBuffer->Activate(context, 0, true, true);
		siro->Render(context, mBlurShader.get(), frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		mCbZoomBuffer->DeActivate(context);
	}
	else if (mTutorialState->GetState() == 1 || mTutorialState->GetState() == 3)
	{//チュートリアル時
		mTutorialState->GetCbZoom()->Activate(context, 0, true, true);
		siro->Render(context, mBlurShader.get(), frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0, mTutorialState->GetBackGroundColor());
		mTutorialState->GetCbZoom()->DeActivate(context);

	}
	else
	{//かけない
		siro->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	}
	//描画結果を保存
	if (mScreenShot)
	{//通常時
		std::wstring fileName = L"Data/image/screen_shot/screenShot" + std::to_wstring(textureNo) + L".dds";
		frameBuffer2->SaveDDSFile(context, fileName.c_str(), frameBuffer2->GetRenderTargetShaderResourceView().Get());
	}
	else if (mSManager->GetStageEditor()->GetEditorFlag())
	{//ステージエディター起動中
		if (mSManager->GetStageEditor()->GetSceneSaveFlag())
		{
			std::wstring fileName = L"Data/image/stage" + std::to_wstring(mSManager->GrtStageNo()) + L"scne_map.dds";
			frameBuffer2->SaveDDSFile(context, fileName.c_str(), frameBuffer2->GetRenderTargetShaderResourceView().Get());
		}
	}


	if (mEditorNo == 3)mSManager->SidoViewRender(context);
	mFade->Render(context);
	//チュートリアルのテキストの描画
	mTutorialState->RenderText(context);

	//描画用のステートの解除
	mBlend[0]->deactivate(context);
	mSampler[samplerType::warp]->DeActivate(context);
	mSampler[samplerType::border]->DeActivate(context);
	mSampler[samplerType::clamp]->DeActivate(context);

	mDepth->DeActive(context);
	mRasterizer->DeActivate(context);

	HitAreaRender::GetInctance()->ClearCount();
}

/************************解放関数***********************/

void SceneGame::Relese()
{
	//シーンに使ったデータの解放
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
	UIManager::GetInctance()->ClearUI();
}



