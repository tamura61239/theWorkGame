#include "scene_manager.h"
#include"scene_title.h"
#include"scene_game.h"
#include"scene_result.h"
#include"key_board.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif


/************************�������֐�****************************/
void SceneManager::Initialize(ID3D11Device* device)
{
	mScene = std::make_unique<SceneTitle>();
	mDevice = device;
	mScene->Initialize(device);
}
/********************�X�V�֐�***********************/
void SceneManager::Update(float elapsed_time)
{
	//�J�ڐ�̃V�[�������鎞
	if (mNextScene.get() != nullptr)
	{
		//�V�[���J��
		if(mScene!=nullptr)mScene->Relese();
		mScene.reset();
		mNextScene->Initialize(mDevice);
		mScene = std::move(mNextScene);
		//mNextScene.reset();
	}
	//�L�[�{�[�h���͂̍X�V
	pKeyBoad.Update();
	//�G�f�B�^�[�̍X�V
	mScene->Editor();
	//�V�[���̍X�V
	mScene->Update(elapsed_time);
}
/*******************�`��֐�*********************/
void SceneManager::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//�V�[���̕`��
	mScene->Render(context, elapsed_time);
}
//�J�ڐ�̃V�[���̐���
void SceneManager::ChangeScene(Scene*scene)
{
	mNextScene.reset(scene);
}

