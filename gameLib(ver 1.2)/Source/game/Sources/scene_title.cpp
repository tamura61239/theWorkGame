#include "scene_title.h"
#include"key_board.h"
#include"scene_manager.h"
#include"gpu_particle_manager.h"
#include"camera_manager.h"
#include"light.h"
#include"ui_manager.h"
#include"stage_manager.h"
#include"screen_size.h"
#include"scene_game.h"
#include"scene_result.h"
#include"scene_select.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/
/**************************コンストラクタ***************************/

SceneTitle::SceneTitle() : mTestMove(false)
{
}
/***********************初期化************************/
void SceneTitle::Initialize(ID3D11Device* device)
{
	//マルチスレッドでクラス変数の生成
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			//マルチスレッドの終了に必要な変数の生成
			std::lock_guard<std::mutex> lock(loading_mutex);
			//オブジェクトの生成
			GpuParticleManager::Create();
			pGpuParticleManager->CreateTitleBuffer(device);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::TITLE);
			UIManager::Create();
			UIManager::GetInctance()->TitleInitialize(device);
			//カメラの生成
			pCameraManager->Initialize(device, 0);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));
			pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::TITLE_CAMERA);
			//描画関連の生成
			bloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0);
			frameBuffer[0] = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer[1] = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer[2] = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			pLight.CreateLightBuffer(device);

			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::TITLE);
			mFade->StartFadeIn();
			D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

			};
			mBluer = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/zoom_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
			mCbZoomBuffer = std::make_unique<ConstantBuffer<CbZoom>>(device);
			{
				//ファイル操作
				FILE* fp;
				if (fopen_s(&fp, "Data/file/title_zoom_blur_parameter.bin", "rb") == 0)
				{
					fread(&mCbZoomBuffer->data, sizeof(mCbZoomBuffer->data), 1, fp);
					fclose(fp);
				}
			}
		}, device);
	//NowLoading中に使う変数の生成
	mLoading = true;
	renderFlag = false;
	test = std::make_unique<Sprite>(device/*, L"Data/image/change_color.png"*/);
	//描画用のステートの生成
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mSampler.push_back(std::make_unique<SamplerState>(device));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);
	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);

}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void SceneTitle::Editor()
{
#ifdef USE_IMGUI
	if (mLoading)
	{
		return;
	}
	//シーンエディターを使ってシーン遷移する
	int newtScene = pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount());
	switch (newtScene)
	{
	case 2:
		pSceneManager.ChangeScene(new SceneSelect);
		break;
	case 3:
		pSceneManager.ChangeScene(new SceneGame(pSceneManager.GetSceneEditor()->GetStageNo()));
		return;
		break;
	case 4:
		pSceneManager.ChangeScene(new SceneResult(0.f, 0));
		return;
		break;
	}
	//エディターがOFFの時
	if (!mEditorFlag)return;
	ImGui::Begin("scene title");
	ImGui::Checkbox("stop", &mStopTime);
	ImGui::SliderFloat("time", &mElapsdTimeSpeed, 0, 1);
	static int editorNum = 1;
#ifdef _DEBUG
	//シーンを画像として保存するかどうかを選択する
	if (ImGui::CollapsingHeader("screen shot"))
	{
		ImGui::InputInt("No", &mTextureNo, 1);
		if (ImGui::Button("photograph"))
		{
			mScreenShot = true;
		}
	}
#endif
	//どのエディターを操作するか選択する
	ImGui::RadioButton("LIGHT", &editorNum, 0);
	ImGui::RadioButton("UI", &editorNum, 1);
	ImGui::RadioButton("GPU PARTICLE", &editorNum, 2);
	ImGui::RadioButton("CAMERA", &editorNum, 3);
	ImGui::RadioButton("BLOOM", &editorNum, 4);
	ImGui::RadioButton("FADE", &editorNum, 5);
	ImGui::RadioButton("ZOOM BLUR", &editorNum, 6);
	ImGui::End();
	//選択されたエディタ関数を呼ぶ
	switch (editorNum)
	{
	case 0:
		pLight.Editor();
		break;
	case 1:
		UIManager::GetInctance()->Editor();
		break;
	case 2:
		pGpuParticleManager->Editor();
		break;
	case 3:
		pCameraManager->Editor();
		break;
	case 4:
		bloom->Editor();
		break;
	case 5:
		mFade->Editor();
		break;

	}
	if (editorNum == 6)
	{
		//ズームブラーのパラメーターを操作する
		ImGui::Begin("zoom blur");
		ImGui::InputFloat("length", &mCbZoomBuffer->data.lenght, 0.1f);
		ImGui::InputInt("division", &mCbZoomBuffer->data.division, 1);
		//セーブ
		if (ImGui::Button("save"))
		{
			FILE* fp;
			fopen_s(&fp, "Data/file/title_zoom_blur_parameter.bin", "wb");
			fwrite(&mCbZoomBuffer->data, sizeof(mCbZoomBuffer->data), 1, fp);
			fclose(fp);

		}
		ImGui::End();

	}
#endif

}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void SceneTitle::Update(float elapsed_time)
{
	//マルチスレッドの処理が終わったかどうかを調べる
	if (IsNowLoading() || !renderFlag)
	{
		return;
	}
	//マルチスレッドの終了処理
	mLoading = false;
	EndLoading();
	//シーンを止める
	if (mStopTime) elapsed_time = 0;
	//シーンの進行速度
	elapsed_time *= mElapsdTimeSpeed;
	//フェードの更新
	mFade->Update(elapsed_time);
	//UIの更新
	UIManager::GetInctance()->Update(elapsed_time);
	//カメラの更新
	pCameraManager->Update(elapsed_time);
	//パーティクルの更新
	pGpuParticleManager->GetTitleParticle()->SetChangeFlag(pCameraManager->GetCameraOperation()->GetTitleCamera()->GetTitleSceneChangeFlag());
	pGpuParticleManager->Update(elapsed_time);
	if (pCameraManager->GetCameraOperation()->GetTitleCamera()->GetEndTitleFlag())
	{
		//シーンを遷移する
		pSceneManager.ChangeScene(new SceneSelect);
		return;
	}
	//シーン遷移の処理を開始する
	if (UIManager::GetInctance()->GetTitleUIMove()->GetMoveChangeFlag())
	{
		if (pKeyBoad.RisingState(KeyLabel::SPACE))
		{
			UIManager::GetInctance()->GetTitleUIMove()->SetMoveChangeFlag(false);
			pCameraManager->GetCameraOperation()->GetTitleCamera()->SetTitleSceneChangeFlag(true);
			pGpuParticleManager->GetTitleTextureParticle()->SetFullDrowFlag(true);
			UIManager::GetInctance()->ClearUI();
		}
	}
}

/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
void SceneTitle::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//NowLoading中かどうか
	if (mLoading)
	{
		renderFlag = true;
		return;
	}
	//描画用のステートの設定
	mRasterizer->Activate(context);
	mDepth->Activate(context);
	mSampler[samplerType::wrap]->Activate(context, 0, true, true, true);
	mSampler[samplerType::clamp]->Activate(context, 2, true, true, true);
	/************************カラーマップテクスチャの作成***********************/
	frameBuffer[0]->Clear(context);
	frameBuffer[0]->Activate(context);
	//view projection行列の取得
	//シーンのオブジェクトの描画
	mBlend[1]->activate(context);
	pCameraManager->GetCamera()->NowActive(context, 0, true, true, true);
	pGpuParticleManager->Render(context);
	pCameraManager->GetCamera()->NowDactive(context);
	if (!mScreenShot && pGpuParticleManager->GetTitleTextureParticle()->GetTextuteFlag())UIManager::GetInctance()->Render(context);
	mBlend[1]->deactivate(context);
	frameBuffer[0]->Deactivate(context);
	mBlend[0]->activate(context);
	/******************************ブルーム************************/
	 //ブルームの準備
	bloom->BlurTexture(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get());
	//シーンにブルームをかける
	frameBuffer[1]->Clear(context);
	frameBuffer[1]->Activate(context);
	test->Render(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get());
	frameBuffer[1]->Deactivate(context);
	/******************************ズームブラー************************/
	frameBuffer[2]->Clear(context);
	frameBuffer[2]->Activate(context);
	if (pCameraManager->GetCameraOperation()->GetTitleCamera()->GetMoveFlag())
	{//ズームブラーをかける
		mCbZoomBuffer->Activate(context, 0, true, true);
		test->Render(context, mBluer.get(), frameBuffer[1]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		mCbZoomBuffer->DeActivate(context);
	}
	else
	{//ズームブラーをかけない
		test->Render(context, frameBuffer[1]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	}
	frameBuffer[2]->Deactivate(context);
	//ポストエフェクトをかけたシーンをフロントバッファに描画
	test->Render(context, frameBuffer[2]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	//描画結果を保存
	if (mScreenShot)
	{
		std::wstring fileName = L"Data/image/screen_shot/screenShot" + std::to_wstring(mTextureNo) + L".dds";
		frameBuffer[2]->SaveDDSFile(context, fileName.c_str(), frameBuffer[2]->GetRenderTargetShaderResourceView().Get());

	}
	mFade->Render(context);
	//描画用のステートの解除
	mBlend[0]->deactivate(context);
	mRasterizer->DeActivate(context);
	mDepth->DeActive(context);
	mSampler[samplerType::wrap]->DeActivate(context);
	mSampler[samplerType::clamp]->DeActivate(context);
}

/*****************************************************/
//　　　　　　　　　　解放関数
/*****************************************************/
void SceneTitle::Relese()
{
	//シーンに使ったデータの解放
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
	UIManager::GetInctance()->ClearUI();

}

