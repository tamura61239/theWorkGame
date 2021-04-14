#pragma once
#include<memory>
#include"scene.h"
#include"scene_editor.h"

class SceneManager
{
public:
	//������
	void Initialize(ID3D11Device* device);
	//�X�V
	void Update(float elapsed_time);
	//�`��
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	//�V�[���J�ڊ֐�
	void ChangeScene(Scene*scene);
	//getter
	SceneEditor* GetSceneEditor() { return mEditor.get(); }
	//�V���O���g��
	static SceneManager& GetInctance()
	{
		static SceneManager manager;
		return manager;
	}
	//�V�[����enum
	enum SCENETYPE
	{
		TITLE = 1,
		SELECT,
		GAME,
		RESULT,
	};

private:
	//�R���X�g���N�^
	SceneManager()
	{
		mEditor = std::make_unique<SceneEditor>();
	}
	//���̃V�[��
	std::unique_ptr<Scene>mScene;
	//���̃V�[��
	std::unique_ptr<Scene>mNextScene;
	//�V�[���G�f�B�^�[
	std::unique_ptr<SceneEditor>mEditor;
};
#define pSceneManager (SceneManager::GetInctance())
