#pragma once
#include<memory>
#include"scene.h"
#include"scene_editor.h"

class SceneManager
{
public:
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	void ChangeScene(const int sceneNum);
	Scene* SceneSelect(const int sceneNum);
	//setter
	void SetDevice(ID3D11Device* device) { mDevice = device; }
	//getter
	SceneEditor* GetSceneEditor() { return mEditor.get(); }
	static SceneManager& GetInctance()
	{
		static SceneManager manager;
		return manager;
	}
private:
	SceneManager()
	{
		mEditor = std::make_unique<SceneEditor>();
	}
	std::unique_ptr<Scene>mScene;
	ID3D11Device* mDevice = nullptr;
	std::unique_ptr<SceneEditor>mEditor;
};
#define pSceneManager (SceneManager::GetInctance())
enum SCENETYPE
{
	TITLE,
	GAME,
	RESULT,
};
