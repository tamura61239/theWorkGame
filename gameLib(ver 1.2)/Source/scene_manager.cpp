#include "scene_manager.h"
#include"scene_title.h"
#include"scene_game.h"
#include"scene_result.h"
#include"key_board.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif


/************************初期化関数****************************/
void SceneManager::Initialize(ID3D11Device* device)
{
	mScene = std::make_unique<SceneTitle>();
	mDevice = device;
	mScene->Initialize(device);
}
/********************更新関数***********************/
void SceneManager::Update(float elapsed_time)
{
	//遷移先のシーンがある時
	if (mNextScene.get() != nullptr)
	{
		//シーン遷移
		if(mScene!=nullptr)mScene->Relese();
		mScene.reset();
		mNextScene->Initialize(mDevice);
		mScene = std::move(mNextScene);
		//mNextScene.reset();
	}
	//キーボード入力の更新
	pKeyBoad.Update();
	//エディターの更新
	mScene->Editor();
	//シーンの更新
	mScene->Update(elapsed_time);
}
/*******************描画関数*********************/
void SceneManager::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//シーンの描画
	mScene->Render(context, elapsed_time);
}
//遷移先のシーンの生成
void SceneManager::ChangeScene(Scene*scene)
{
	mNextScene.reset(scene);
}

