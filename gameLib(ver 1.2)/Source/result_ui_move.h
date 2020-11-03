#pragma once
#include"ui.h"
#include<vector>

class ResultUIMove
{
public:
	ResultUIMove();
	void Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis);
	//setter
	void SetMoveFlag(const bool flag) { mMoveFlag = flag; }
	//getter
	const int GetType() { return mType; }
	const bool GetDecisionFlag() { return mDecisionFlag; }
private:
	int mType;
	bool mMoveFlag;
	bool mDecisionFlag;
};