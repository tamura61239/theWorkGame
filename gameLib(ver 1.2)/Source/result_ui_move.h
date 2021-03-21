#pragma once
#include"ui.h"
#include<vector>

class ResultUIMove
{
public:
	ResultUIMove();
	void Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis,size_t uiCount);
	struct ResultUIData
	{
		float frameAlpth;
		float alpthDifference;
	};
	//setter
	void SetMoveFlag(const bool flag) { mMoveFlag = flag; }
	void SetResultUIData(const ResultUIData data) { mData = data; }
	//getter
	const int GetType() { return mType; }
	const bool GetDecisionFlag() { return mDecisionFlag; }
	ResultUIData GetResultUIData(){ return mData; }

	void Save();
private:
	void Load();
	int mType;
	bool mMoveFlag;
	bool mDecisionFlag;
	ResultUIData mData;
};