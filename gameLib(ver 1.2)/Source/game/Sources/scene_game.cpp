#include "scene_game.h"
#include"key_board.h"
#include"scene_manager.h"
#include"sound_manager.h"
#include"camera_manager.h"
#include"gamepad.h"
#include"light.h"
#include"Judgment.h"
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

/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/
/**************************コンストラクタ***************************/
SceneGame::SceneGame(int stageNo) : mTestGame(false), mHitArea(false), mNowLoading(true), mLoadEnd(false), mStageNo(stageNo)
{

}
/***********************初期化************************/
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
			player = std::make_unique<PlayerManager>(device, "Data/FBX/new_player_anim.fbx");
			//PUパーティクルマネージャーにプレイヤーのデータをセットする
			pGpuParticleManager->CreateGameBuffer(device, player.get());

			//ステージの生成
			mSManager = std::make_unique<StageManager>(device, SCREEN_WIDTH, SCREEN_HEIGHT);
			mSManager->SetStageNo(mStageNo);
			mSManager->Load();
			//描画関連クラスの生成
			mModelRenderer = std::make_unique<ModelRenderer>(device);
			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1);
			mLightView = std::make_unique<LightView>(device, "LightViewData1");
			mCbMotionBlur = std::make_unique<ConstantBuffer<MotionBlurParameter>>(device);
			FileFunction::Load(mCbMotionBlur->data, "Data/file/game_velocity_map_parameter.bin", "rb");
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::GAME);
			mFade->StartFadeIn();

			//ステージの操作クラスの生成
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
			mPhotographTargets.resize(6);

			mGameMove = std::make_unique<SceneGameMove>(device, mStageNo);
		}, device);
	//NowLoading中に使う変数だけ先に生成
	test = std::make_unique<Sprite>(device, L"Data/image/操作説明.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/now.png");
	pushKey = std::make_unique<Sprite>(device, L"Data/image/push key.png");
	//描画用のステートの生成
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_LESS_EQUAL));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);

	mTextureNo = 0;

}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

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
		pSceneManager.ChangeScene(new SceneResult(0.f, mStageNo));
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
		player->SetPlayFlag(false);
		UIManager::GetInctance()->GetGameUIMove()->SetStartFlag(false);
		UIManager::GetInctance()->ResetGameUI();
		mGameMove->GetTutorial()->ResetParameter();
	}
	//前のフレームのeditorNoを保存
	int beforeEditorNo = mEditorNo;
	/*******************Editor****************/
	ImGui::Begin("scene game");
	ImGui::Checkbox("stop", &mStopTime);
	ImGui::Checkbox("hitArea", &mHitArea);
	ImGui::SliderFloat("time", &mElapsdTimeSpeed, 0, 1);
	ImVec2 size = ImVec2(250, 250);
	ImGui::Image(frameBuffer3->GetRenderTargetShaderResourceView().Get(), size);
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
		ImGui::InputInt("No", &mTextureNo, 1);
		if (ImGui::Button("photograph"))
		{
			mScreenShot = true;
		}
	}
#endif
	if (!mTestGame)
	{
		//テストプレイ開始
		if (ImGui::Button("testGame"))
		{
			mTestGame = true;
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
			mTestGame = false;
			//パラメーターのリセット
			player->SetPlayFlag(false);
			UIManager::GetInctance()->GetGameUIMove()->SetStartFlag(false);
			UIManager::GetInctance()->ResetGameUI();
			mGameMove->GetTutorial()->ResetParameter();
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
		pLight.Editor();
		break;
	case 3:
		mSManager->Editor();
		break;
	case 4:
		player->Editor();
		break;
	case 5:
		pGpuParticleManager->Editor();
		break;
	case 6:
		pCameraManager->Editor();
		break;
	case 7:
		mBloom->Editor();
		break;
	case 8:
		mFade->Editor();
		break;
	case 9:
		UIManager::GetInctance()->Editor();
		break;
	case 11:
		mRenderEffects->Editor();

		break;
	case 12:
		mLightView->Editor();
		break;
	case 13:
		mGameMove->GetTutorial()->Editor();
		break;
	case 14:
		mGameMove->ZoomBlurEditor();
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


/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
/*****************シーンの処理*********************/

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
	mSManager->Update(elapsed_time, player->GetPlayFlag() && mGameMove->GetTutorial()->GetKeyFlag());
	elapsed_time = mGameMove->Update(elapsed_time, player.get(), mTestGame, mEditorFlag);

	//フェード時の処理
	//シーンのオブジェクトの更新
	mSky->GetPosData()->CalculateTransform();
	player->Update(elapsed_time, mSManager.get());
	pGpuParticleManager->Update(elapsed_time);
	//カメラの更新
	pCameraManager->Update(elapsed_time);
}


/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
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
		mSampler[samplerType::clamp]->DeActivate(context);

		mDepth->DeActive(context);
		mRasterizer->DeActivate(context);

		return;
	}

	pLight.ConstanceLightBufferSetShader(context);
	pCameraManager->GetCamera()->NowActive(context, 0, true, true, true);
	/************************カラーマップテクスチャの作成***********************/

	frameBuffer3->Clear(context);
	frameBuffer3->Activate(context);
	//シーンの描画
	if (mSManager->GetStageEditor()->GetEditorFlag())
	{//ステージエディターの時
		player->Render(context, PlayerManager::RENDER_TYPE::COLOR);
		mSManager->Render(context);
	}
	else
	{//通常時
		mSky->Render(context);
		pGpuParticleManager->Render(context);
		player->Render(context, PlayerManager::RENDER_TYPE::COLOR);

		mSManager->Render(context);
	}
	frameBuffer3->Deactivate(context);

	/************************速度マップテクスチャの作成***********************/
	velocityMap->Clear(context);
	velocityMap->Activate(context);
	//定数バッファの設定
	mCbMotionBlur->Activate(context, 6, true, true);
	pCameraManager->GetCamera()->BeforeActive(context, 5, true, true, true);
	//シーンのの描画
	player->Render(context, PlayerManager::RENDER_TYPE::VELOCITY);
	pGpuParticleManager->VelocityRender(context);
	mSManager->RenderVelocity(context);
	//定数バッファの解除
	pCameraManager->GetCamera()->BeforeDactive(context);
	mCbMotionBlur->DeActivate(context);
	velocityMap->Deactivate(context);
	pCameraManager->GetCamera()->NowDactive(context);
	/************************シャドウマップテクスチャの作成***********************/
	shadowMap->Clear(context);
	shadowMap->Activate(context);

	//light視点のカメラの更新と情報の取得
	mLightView->Update(player->GetCharacter()->GetPosition(), context);
	mLightView->GetLightCamera()->NowActive(context, 0, true, true, true);
	//light視点から見たシーンの描画
	player->Render(context, PlayerManager::RENDER_TYPE::SHADOW);
	mSManager->RenderShadow(context);
	mLightView->GetLightCamera()->NowDactive(context);

	shadowMap->Deactivate(context);
	//シーンに影を付ける
	shadowRenderBuffer->Clear(context);
	shadowRenderBuffer->Activate(context);
	mRenderEffects->ShadowRender(context, frameBuffer3->GetRenderTargetShaderResourceView().Get(), frameBuffer3->GetDepthStencilShaderResourceView().Get(), shadowMap->GetDepthStencilShaderResourceView().Get()
		, pCameraManager->GetCamera()->GetView(), pCameraManager->GetCamera()->GetProjection(), mLightView->GetLightCamera()->GetView(), mLightView->GetLightCamera()->GetProjection());
	shadowRenderBuffer->Deactivate(context);
	/******************************モーションブラー************************/
	frameBuffer->Clear(context);
	frameBuffer->Activate(context);

	velocityMap->SetPsTexture(context, 1);
	test->Render(context, motionBlurShader.get(), shadowRenderBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(1, 1, &srv);
	//シーンにUIを追加
	UIManager::GetInctance()->Render(context);
	mGameMove->GetTutorial()->RenderButton(context);

	mBlend[0]->deactivate(context);
	frameBuffer->Deactivate(context);
	mBlend[1]->activate(context);


	/******************************ブルーム************************/
	//ブルームの準備
	mBloom->BlurTexture(context, frameBuffer->GetRenderTargetShaderResourceView().Get());
	//シーンにブルームをかける
	frameBuffer2->Clear(context);
	frameBuffer2->Activate(context);
	test->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get());
	frameBuffer2->Deactivate(context);
	mBlend[1]->deactivate(context);

	mBlend[0]->activate(context);
	/******************************ズームブラー************************/
	mGameMove->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), test.get());
	//描画結果を保存
	if (mScreenShot)
	{//通常時
		std::wstring fileName = L"Data/image/screen_shot/screenShot" + std::to_wstring(mTextureNo) + L".dds";
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
	//mFade->Render(context);
	//チュートリアルのテキストの描画
	mGameMove->GetTutorial()->RenderText(context);

	//描画用のステートの解除
	mBlend[0]->deactivate(context);
	mSampler[samplerType::warp]->DeActivate(context);
	mSampler[samplerType::border]->DeActivate(context);
	mSampler[samplerType::clamp]->DeActivate(context);

	mDepth->DeActive(context);
	mRasterizer->DeActivate(context);

	HitAreaRender::GetInctance()->ClearCount();
}

/*****************************************************/
//　　　　　　　　　　解放関数
/*****************************************************/


void SceneGame::Relese()
{
	//シーンに使ったデータの解放
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
	UIManager::GetInctance()->ClearUI();
}



