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
	//初期化
	void TitleInitialize(ID3D11Device* device);
	void GameInitialize(ID3D11Device* device);
	void ResultInitialize(ID3D11Device* device);
	//クリア
	void Clear();
	void ClearUI();
	void ResetGameUI()
	{
		mGameMove->SetTestFlag(false);
		mGameMove->SetUI(mUIs);
	}
	//エディタ
	void Editor();
	//更新
	void Update(float elapsdTime);
	//描画
	void Render(ID3D11DeviceContext* context);
	//getter
	TitleUIMove* GetTitleUIMove() { return mTitleMove.get(); }
	GameUiMove* GetGameUIMove() { return mGameMove.get(); }
	ResultUIMove* GetResultUIMove() { return mResultMove.get(); }
private:
	//UIテキスト
	std::vector<std::shared_ptr<UI>>mUIs;
	//名前リスト
	std::vector<std::string>mNames;
	//動き
	std::unique_ptr<TitleUIMove>mTitleMove;
	std::unique_ptr<GameUiMove>mGameMove;
	std::unique_ptr<ResultUIMove>mResultMove;
	//UIのフレーム
	std::unique_ptr<UI>mDebugUIFrame;
	//エディタ変数
	int mUINumber;
	std::string mSceneName;
	bool mMoveStartFlag;
	bool mDebugUIFrameFlag;
	int mUIMaxCount;
	//ファイル操作
	void Load(const char* scene);
	void Save(const char* scene);
};