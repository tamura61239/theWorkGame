#pragma once
#include"ui.h"
#include<vector>
#include"title_ui_move.h"
#include"game_ui_move.h"
#include"result_ui_move.h"
#include"singleton_class.h"

class UIManager:public Singleton<UIManager>
{
public:
	//������
	void TitleInitialize(ID3D11Device* device);
	void GameInitialize(ID3D11Device* device);
	void ResultInitialize(ID3D11Device* device);
	//�N���A
	void Clear();
	void ClearUI();
	void ResetGameUI()
	{
		mGameMove->SetTestFlag(false);
		mGameMove->SetUI(mUIs);
	}
	//�X�V
	void ImGuiUpdate();
	void Update(float elapsdTime);
	void Render(ID3D11DeviceContext* context);
	//getter
	TitleUIMove* GetTitleUIMove() { return mTitleMove.get(); }
	GameUiMove* GetGameUIMove() { return mGameMove.get(); }
	ResultUIMove* GetResultUIMove() { return mResultMove.get(); }
private:
	std::vector<std::shared_ptr<UI>>mUIs;
	std::vector<std::string>mNames;
	std::unique_ptr<TitleUIMove>mTitleMove;
	std::unique_ptr<GameUiMove>mGameMove;
	std::unique_ptr<ResultUIMove>mResultMove;
	std::unique_ptr<UI>mDebugUIFrame;
	int mUINumber;
	std::string mSceneName;
	bool mMoveStartFlag;
	bool mDebugUIFrameFlag;
	void Load(const char* scene);
	void Save(const char* scene);
};