#pragma once
#include"ui.h"
#include<vector>
#include"title_ui_move.h"
#include"game_ui_move.h"

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
	void ResetGameUI()
	{
		mGameMove->SetTestFlag(false);
		mGameMove->SetUI(mUIs);
	}
	//更新
	void ImGuiUpdate();
	void Update(float elapsdTime);
	void Render(ID3D11DeviceContext* context);
	//getter
	const bool GetTitleMoveChangeFlag() { return mTitleMove->GetMoveChangeFlag(); }
	GameUiMove* GetGameUIMove() { return mGameMove.get(); }
private:
	UIManager(){}
	std::vector<std::shared_ptr<UI>>mUIs;
	std::vector<std::string>mNames;
	std::unique_ptr<TitleUIMove>mTitleMove;
	std::unique_ptr<GameUiMove>mGameMove;
	int mUINumber;
	std::string mSceneName;
	bool mMoveStartFlag;
	void Load(const char* scene);
	void Save(const char* scene);
};