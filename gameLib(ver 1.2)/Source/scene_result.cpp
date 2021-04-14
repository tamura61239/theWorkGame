#include "scene_result.h"
#include"ui_manager.h"
#include"scene_manager.h"
#include"camera_manager.h"
#include"gpu_particle_manager.h"
#include"stage_operation.h"
#include"light.h"
#include"screen_size.h"
#include"scene_title.h"
#include"scene_select.h"
#include"scene_game.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

SceneResult::SceneResult(float timer, int nowStageNo) :mNowGameTime(timer), mPlayFlag(true), nowLoading(true)
{
	Ranking::SetStageNo(nowStageNo);
}
/*******************************初期化関数*****************************/
void SceneResult::Initialize(ID3D11Device* device)
{
	//マルチスレッドでクラス変数の生成
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			//マルチスレッドの終了に必要な変数の生成
			std::lock_guard<std::mutex> lock(loading_mutex);

			//描画するオブジェクトの生成
			UIManager::Create();
			CameraManager::Create();
			pCameraManager->Initialize(device, 2);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));
			pCameraManager->Update(0);

			GpuParticleManager::Create();
			pGpuParticleManager->CreateResultBuffer(device);
			pGpuParticleManager->SetState(GpuParticleManager::STATE::RESULT);

			UIManager::GetInctance()->ResultInitialize(device);
			mRanking = std::make_unique<Ranking>(device, mNowGameTime);
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::RESULT);

			//描画用変数の生成
			frameBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			frameBuffer2 = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			velocityBuffer = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);

			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 3);
			sky = std::make_unique<SkyMap>(device, L"Data/AllSkyFree/Cold Night/ColdNight.dds", MAPTYPE::BOX);
			//スカイマップの値をファイルから読み取る
			{
				FILE* fp;
				if (fopen_s(&fp, "Data/file/result_sky_map.bin", "rb") == 0)
				{
					fread(sky->GetPosData(), sizeof(Obj3D), 1, fp);
					fclose(fp);
				}
			}
			//スカイマップのワールド座標系行列を最初に計算しとく
			sky->GetPosData()->CalculateTransform();

		}, device);
	//NowLoading中に使う変数だけ先に生成
	mRenderScene = std::make_unique<Sprite>(device, L"Data/image/now.png");
	renderFlag = false;
	//描画用のステートの生成
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	mSampler.push_back(std::make_unique<SamplerState>(device));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);
	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);

}
/***********************エディター関数(ImGuiを使ってパラメーターを調整する)*******************/
void SceneResult::Editor()
{
#ifdef USE_IMGUI
	//マルチスレッドの処理が終わったかどうかを調べる(終わってなかったらreturn)
	if (IsNowLoading())
	{
		return;
	}
	//シーンエディターを使ってシーン遷移する
	int nextScene = pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount());
	switch (nextScene)
	{
	case 1:
		UIManager::GetInctance()->Clear();
		UIManager::Destroy();
		pCameraManager->DestroyCamera();
		GpuParticleManager::Destroy();

		pSceneManager.ChangeScene(new SceneTitle);
		break;
	case 2:
		pSceneManager.ChangeScene(new SceneSelect);
		break;
	case 3:
		pSceneManager.ChangeScene(new SceneGame(pSceneManager.GetSceneEditor()->GetStageNo()));
		break;
	}
	if (nextScene <= 3 && nextScene >= 1)return;
	if (!mEditorFlag)return;
	/*******************Editor****************/
	ImGui::Begin("scene result");
#ifdef _DEBUG
	//シーンを画像として保存するかどうかを選択する
	if (ImGui::CollapsingHeader("screen shot"))
	{
		ImGui::InputInt("No", &mTextureNo, 1);

		if (ImGui::Button("shot"))
		{
			mScreenShot = true;
		}
	}
#endif
	//どのエディターを操作するか選択する
	ImGui::RadioButton("NONE", &mEditorNo, 0);
	ImGui::RadioButton("RANKING", &mEditorNo, 1);
	ImGui::RadioButton("UI", &mEditorNo, 2);
	ImGui::RadioButton("FADE", &mEditorNo, 3);
	ImGui::RadioButton("BLOOM", &mEditorNo, 4);
	ImGui::RadioButton("CAMERA", &mEditorNo, 5);
	ImGui::RadioButton("SKY MAP", &mEditorNo, 6);
	ImGui::RadioButton("GPU PARTICLE", &mEditorNo, 7);

	ImGui::End();
	//選択されたエディタ関数を呼ぶ
	switch (mEditorNo)
	{
	case 1:
		mRanking->ImGuiUpdate();
		break;
	case 2:
		UIManager::GetInctance()->ImGuiUpdate();
		break;
	case 3:
		mFade->ImGuiUpdate();
		break;
	case 4:
		mBloom->ImGuiUpdate();
		break;
	case 5:
		pCameraManager->ImGuiUpdate();
		break;
	case 7:
		pGpuParticleManager->ImGuiUpdate();
		break;
	}
	if (mEditorNo == 6)
	{
		//スカイマップのパラメーターを操作する
		ImGui::Begin("sky map");
		//中心座標
		float* position[3] = { &sky->GetPosData()->GetPosition().x,&sky->GetPosData()->GetPosition().y ,&sky->GetPosData()->GetPosition().z };
		ImGui::DragFloat3("position", *position, 10);
		//スケール
		float* scale[3] = { &sky->GetPosData()->GetScale().x,&sky->GetPosData()->GetScale().y ,&sky->GetPosData()->GetScale().z };
		ImGui::DragFloat3("scale", *scale, 10);
		//各軸の回転
		float* angle[3] = { &sky->GetPosData()->GetAngle().x,&sky->GetPosData()->GetAngle().y ,&sky->GetPosData()->GetAngle().z };
		ImGui::SliderFloat3("angle", *angle, -3.14f, 3.14f);
		//色
		float* color[4] = { &sky->GetPosData()->GetColor().x,&sky->GetPosData()->GetColor().y ,&sky->GetPosData()->GetColor().z ,&sky->GetPosData()->GetColor().w };
		ImGui::ColorEdit4("color", *color);
		//セーブ
		if (ImGui::Button("save"))
		{
			//ファイル操作
			FILE* fp;
			fopen_s(&fp, "Data/file/result_sky_map.bin", "wb");
			fwrite(sky->GetPosData(), sizeof(Obj3D), 1, fp);
			fclose(fp);
		}
		ImGui::End();
	}

#endif

}
/**********************更新関数*******************************/
void SceneResult::Update(float elapsed_time)
{
	//マルチスレッドの処理が終わったかどうかを調べる(終わってなかったらreturn)
	if (IsNowLoading())
	{
		return;
	}
	//マルチスレッドの終了関数
	EndLoading();
	//フェードの更新
	mFade->Update(elapsed_time);
	//シーン切り替え
	if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
	{
		//フェードアウトが終了したかどうか
		if (mFade->GetEndFlag())
		{
			//フェードアウト終了時にシーン遷移
			int type = UIManager::GetInctance()->GetResultUIMove()->GetType();
			switch (type)
			{
			case 0://前にプレイしたステージへ
				pSceneManager.ChangeScene(new SceneGame(Ranking::GetStageNo()));
				break;
			case 1://タイトルシーンへ
				pSceneManager.ChangeScene(new SceneTitle);
				break;
			case 2://セレクトシーンへ
				pSceneManager.ChangeScene(new SceneSelect);
				break;
			case 3://次のステージへ
				pSceneManager.ChangeScene(new SceneGame(Ranking::GetStageNo() + 1));
				break;
			}
			return;
		}
	}
	//シーンのオブジェクトの更新
	sky->GetPosData()->CalculateTransform();
	mRanking->Update(elapsed_time, mPlayFlag);
	UIManager::GetInctance()->Update(elapsed_time);
	pGpuParticleManager->Update(elapsed_time);
	//フェードアウト開始
	if (UIManager::GetInctance()->GetResultUIMove()->GetDecisionFlag())
	{
		mFade->StartFadeOut();
	}
	//カメラの更新
	pCameraManager->Update(elapsed_time);

}
/******************************描画関数**************************************/
void SceneResult::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//描画用のステートの設定
	mRasterizer->Activate(context);
	mDepth->Activate(context);
	mSampler[samplerType::wrap]->Activate(context, 0, true, true, true);
	mSampler[samplerType::clamp]->Activate(context, 2, true, true, true);
	mBlend[0]->activate(context);

	//マルチスレッドの処理が終わったかどうかを調べる(終わってなかったらreturn)
	if (IsNowLoading())
	{
		//NowLoadingの文字の描画
		mRenderScene->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, LoadColor(elapsed_time));
		//描画用のステートの解除
		mBlend[0]->deactivate(context);
		mRasterizer->DeActivate(context);
		mDepth->DeActive(context);
		mSampler[samplerType::wrap]->DeActivate(context);
		mSampler[samplerType::clamp]->DeActivate(context);

		return;
	}

	//シーンの描画
	pLight.ConstanceLightBufferSetShader(context);
	frameBuffer->Clear(context);
	frameBuffer->Activate(context);
	mBlend[0]->activate(context);
	FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
	FLOAT4X4 projection = pCameraManager->GetCamera()->GetProjection();

	sky->Render(context, view, projection);

	pGpuParticleManager->Render(context, view, projection);
	UIManager::GetInctance()->Render(context);

	mRanking->Render(context);

	frameBuffer->Deactivate(context);
	mBlend[0]->deactivate(context);

	//ブルームの準備
	mBlend[1]->activate(context);
	mBloom->BlurTexture(context, frameBuffer->GetRenderTargetShaderResourceView().Get());

	//シーンにブルームをかける
	frameBuffer2->Clear(context);
	frameBuffer2->Activate(context);
	mRenderScene->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get());
	frameBuffer2->Deactivate(context);
	mBlend[1]->deactivate(context);

	//ブルームをかけたシーンをフロントバッファに描画する
	mBlend[0]->activate(context);

	mRenderScene->Render(context, frameBuffer2->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	if (mScreenShot)
	{
		//描画結果を保存
		mScreenShot = false;
		std::wstring fileName = L"Data/image/screen_shot/screenShot" + std::to_wstring(mTextureNo) + L".dds";
		frameBuffer2->SaveDDSFile(context, fileName.c_str(), frameBuffer2->GetRenderTargetShaderResourceView().Get());
	}
	mFade->Render(context);
	//描画用のステートの解除
	mBlend[0]->deactivate(context);
	mRasterizer->DeActivate(context);
	mDepth->DeActive(context);
	mSampler[samplerType::wrap]->DeActivate(context);
	mSampler[samplerType::clamp]->DeActivate(context);

}

/************************解放関数***********************/
void SceneResult::Relese()
{
	//シーンに使ったデータの解放
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
	UIManager::GetInctance()->ClearUI();
}


