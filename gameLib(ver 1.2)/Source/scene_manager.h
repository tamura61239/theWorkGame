#pragma once
#include<memory>
#include"scene.h"
#include"scene_editor.h"

class SceneManager
{
public:
	void Initialize(ID3D11Device* device);
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	void ChangeScene(Scene*scene);
	Scene* SceneSelect(const int sceneNum);
	//getter
	SceneEditor* GetSceneEditor() { return mEditor.get(); }
	static SceneManager& GetInctance()
	{
		static SceneManager manager;
		return manager;
	}
	enum SCENETYPE
	{
		TITLE = 1,
		SELECT,
		GAME,
		RESULT,
	};

private:
	SceneManager()
	{
		mEditor = std::make_unique<SceneEditor>();
	}
	std::unique_ptr<Scene>mScene;
	std::unique_ptr<Scene>mNextScene;
	ID3D11Device* mDevice = nullptr;
	std::unique_ptr<SceneEditor>mEditor;
};
#define pSceneManager (SceneManager::GetInctance())
