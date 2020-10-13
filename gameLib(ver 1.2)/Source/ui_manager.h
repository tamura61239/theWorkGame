#pragma once
#include"ui.h"
#include<vector>
#include"title_ui_move.h"

class UIManager
{
public:
	static UIManager&GetInctance()
	{
		static UIManager uiManager;
		return uiManager;
	}
	//初期化
	void TitleInitialize(ID3D11Device* device);
	void GameInitialize(ID3D11Device* device);
	//クリア
	void Clear();
	void ClearUI();
	//更新
	void ImGuiUpdate();
	void Update(float elapsdTime);
	void Render(ID3D11DeviceContext* context);
	//getter
	const bool GetMoveChangeFlag() { return mTitleMove->GetMoveChangeFlag(); }
private:
	UIManager(){}
	std::vector<std::shared_ptr<UI>>mUIs;
	std::vector<std::string>mNames;
	std::unique_ptr<TitleUIMove>mTitleMove;
	int mUINumber;
	std::string mSceneName;
	bool mMoveStartFlag;
	void Load(const char* scene);
	void Save(const char* scene);
};