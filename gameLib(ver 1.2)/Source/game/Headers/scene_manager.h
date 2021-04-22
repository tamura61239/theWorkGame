#pragma once
#include<memory>
#include"scene.h"
#include"scene_editor.h"

class SceneManager
{
public:
	//初期化
	void Initialize(ID3D11Device* device);
	//更新
	void Update(float elapsed_time);
	//描画
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	//シーン遷移関数
	void ChangeScene(Scene*scene);
	//getter
	SceneEditor* GetSceneEditor() { return mEditor.get(); }
	//シングルトン
	static SceneManager& GetInctance()
	{
		static SceneManager manager;
		return manager;
	}
	//シーンのenum
	enum SCENETYPE
	{
		TITLE = 1,
		SELECT,
		GAME,
		RESULT,
	};

private:
	//コンストラクタ
	SceneManager()
	{
		mEditor = std::make_unique<SceneEditor>();
	}
	//今のシーン
	std::unique_ptr<Scene>mScene;
	//次のシーン
	std::unique_ptr<Scene>mNextScene;
	//シーンエディター
	std::unique_ptr<SceneEditor>mEditor;
};
#define pSceneManager (SceneManager::GetInctance())
