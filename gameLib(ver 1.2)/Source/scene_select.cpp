#include "scene_select.h"
#include"camera_manager.h"
#include"screen_size.h"
#include"gpu_particle_manager.h"
#include"scene_manager.h"
#include"scene_title.h"
#include"scene_game.h"
#include"scene_result.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/
void SceneSelect::Initialize(ID3D11Device* device)
{
	//マルチスレッド
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);


			mSelect = std::make_unique<StageSelect>(device, StageManager::GetMaxStageCount());
			pCameraManager->Initialize(device, 2);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));

			mColorMap = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			mSceneFrame = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			GpuParticleManager::Create();

			pGpuParticleManager->CreateSelectBuffer(device);

			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1);
		
			mPhotographTargets.resize(2);
			//memset(&mPhotographTargets[0], 0, sizeof(bool) * mPhotographTargets.size());
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::SELECT);
		}, device);
	//NowLoadingの時に描画するもの
	mRenderTexture = std::make_unique<Sprite>(device, L"Data/image/now.png");
	
	//描画用のステートの生成
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP));
	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);

}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void SceneSelect::Editor()
{
	//マルチスレッドの処理が終わったかどうかを調べる(終わってなかったらreturn)
	if (IsNowLoading())
	{
		return;
	}
	bool beforeEditorFlag = mEditorFlag;
	switch (pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount()))
	{
	case SceneManager::SCENETYPE::TITLE:
		pSceneManager.ChangeScene(new SceneTitle);
		break;
	case SceneManager::SCENETYPE::GAME:
		pSceneManager.ChangeScene(new SceneGame(pSceneManager.GetSceneEditor()->GetStageNo()));
		break;
	case SceneManager::SCENETYPE::RESULT:
		pSceneManager.ChangeScene(new SceneResult(0.f,0));
		break;
	}
	//エディターがOFFの時
	if (!mEditorFlag)return;
	ImGui::Begin("scene select");
#ifdef _DEBUG
	//シーンを画像として保存するかどうかを選択する
	if (ImGui::CollapsingHeader("screen shot"))
	{
		ImGui::Selectable("gpu particle", &mPhotographTargets[0]);
		ImGui::Selectable("stage board", &mPhotographTargets[1]);
		if (ImGui::Button("photograph"))
		{
			mScreenShot = true;
		}
	}
#endif
	//どのエディターを操作するか選択する
	ImGui::RadioButton("NONE", &mEditorNo, 0);
	ImGui::RadioButton("SELECT_SCENE", &mEditorNo, 1);
	ImGui::RadioButton("BLOOM", &mEditorNo, 2);
	ImGui::RadioButton("CAMERA", &mEditorNo, 3);
	ImGui::RadioButton("FADE", &mEditorNo, 4);
	ImGui::RadioButton("GPU PARTICLE", &mEditorNo, 5);

	ImGui::End();
	//選択されたエディタ関数を呼ぶ
	switch (mEditorNo)
	{
	case 1:
		mSelect->Editor();
		break;
	case 2:
		mBloom->Editor();
		break;
	case 3:
		pCameraManager->Editor();
		break;
	case 4:
		mFade->Editor();
		break;
	case 5:
		pGpuParticleManager->Editor();
		break;
	}
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void SceneSelect::Update(float elapsed_time)
{
	//マルチスレッドの処理が終わったかどうかを調べる(終わってなかったらreturn)
	if (IsNowLoading())
	{
		return;
	}
	//マルチスレッドの終了関数
	EndLoading();

	if (mSelect->Update(elapsed_time))
	{//フェードアウト開始
		mFade->StartFadeOut();
	}
	//フェートの更新
	mFade->Update(elapsed_time);
	//パーティクルの更新
	pGpuParticleManager->Update(elapsed_time);
	//フェードアウトが終わったかどうか
	if (mFade->GetEndFlag())
	{
		if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
		{
			//選択したステージに遷移
			pSceneManager.ChangeScene(new SceneGame(mSelect->GetSelectNumber()));

		}
	}
	//カメラの更新
	pCameraManager->Update(elapsed_time);
}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
void SceneSelect::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//描画用のステートの設定
	mSampler[0]->Activate(context, 0, false, true);
	mDepth->Activate(context);
	mRasterizer->Activate(context);
	mBlend[0]->activate(context);
	

	if (IsNowLoading())
	{//NowLoading画面の時
		mRenderTexture->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, LoadColor(elapsed_time));
		//描画用のステートのの解除
		mSampler[0]->DeActivate(context);
		mDepth->DeActive(context);
		mRasterizer->DeActivate(context);
		mBlend[0]->deactivate(context);
		return;
	}
	/**************カラーマップ*************/
	
	mColorMap->Clear(context);
	mColorMap->Activate(context);


	pGpuParticleManager->Render(context, pCameraManager->GetCamera()->GetView(), pCameraManager->GetCamera()->GetProjection());
	mSelect->Render(context);

	
	mColorMap->Deactivate(context);

	mBlend[0]->deactivate(context);

	/****************ブルーム***************/	
	mBlend[1]->activate(context);
	//ブルームの準備
	mBloom->BlurTexture(context, mColorMap->GetRenderTargetShaderResourceView().Get());
	//ブルームをかける
	mSceneFrame->Clear(context);
	mSceneFrame->Activate(context);
	mRenderTexture->Render(context, mColorMap->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, mColorMap->GetRenderTargetShaderResourceView().Get());
	mSceneFrame->Deactivate(context);
	mBlend[1]->deactivate(context);

	//ブルームをかけたシーンをフロントバッファに描画する
	mBlend[0]->activate(context);

	mRenderTexture->Render(context, mSceneFrame->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	mFade->Render(context);
	//描画用のステートのの解除
	mSampler[0]->DeActivate(context);
	mDepth->DeActive(context);
	mRasterizer->DeActivate(context);
	mBlend[0]->deactivate(context);

}
/*****************************************************/
//　　　　　　　　　　解放関数
/*****************************************************/
void SceneSelect::Relese()
{
	//シーンに使ったデータの解放
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
}
