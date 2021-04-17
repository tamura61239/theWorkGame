#pragma once
#include"ui.h"
#include<vector>

class TitleUIMove
{
public:
	//コンストラクタ
	TitleUIMove(const int UICount);
	//更新
	void Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis);
	//リセット
	void ResetTimer() 
	{ 
		mTimer = 0; 
		mMoveChangeFlag = false;
	}
	//エディタのデータ
	struct TitleUIMoveData
	{
		float startTime;
		float startAlpha;
		float alphaAmount;
		float endAlpha;
	};
	//getter
	const bool GetMoveChangeFlag() { return mMoveChangeFlag; }
	std::vector<TitleUIMoveData>GetTitleUIMove() { return mDatas; }
	//setter
	void SetMoveChangeFlag(const bool flag) { mMoveChangeFlag = flag; }
	void SetTitleUIMove(std::vector<TitleUIMoveData>datas) { mDatas = datas; }
	//テストプレイ開始
	void TestPlay(std::vector<std::shared_ptr<UI>>uis)
	{
		mTimer = 0;
		mMoveChangeFlag = false;
		for (int i = 0; i < static_cast<int>(uis.size()); i++)
		{
			auto&uiData = uis[i]->GetUIData();
			auto& data = mDatas[i];
			uiData.mColor.w = data.startAlpha;
			uis[i]->SetUIData(uiData);
		}
	}
private:
	//データ
	std::vector<TitleUIMoveData>mDatas;
	//時間
	float mTimer;
	bool mMoveChangeFlag;
};