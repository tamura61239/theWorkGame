#pragma once
#include"ui.h"
#include<vector>

class TitleUIMove
{
public:
	TitleUIMove(const int UICount);
	void Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis);
	void ResetTimer() 
	{ 
		mTimer = 0; 
		mMoveChangeFlag = false;
	}
	void Load();
	void Save();
	struct TitleUIMoveData
	{
		float startTime;
		float startAlpha;
		float alphaAmount;
		float endAlpha;
	};
	const bool GetMoveChangeFlag() { return mMoveChangeFlag; }
	void SetMoveChangeFlag(const bool flag) { mMoveChangeFlag = flag; }
	void TextMove(std::vector<std::shared_ptr<UI>>uis)
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
	std::vector<TitleUIMoveData>GetTitleUIMove() { return mDatas; }
	void SetTitleUIMove(std::vector<TitleUIMoveData>datas) { mDatas = datas; }
private:
	std::vector<TitleUIMoveData>mDatas;
	float mTimer;
	bool mMoveChangeFlag;
};