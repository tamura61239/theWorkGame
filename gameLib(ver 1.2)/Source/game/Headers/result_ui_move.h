#pragma once
#include"ui.h"
#include<vector>

class ResultUIMove
{
public:
	//�R���X�g���N�^
	ResultUIMove();
	//�X�V
	void Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis,size_t uiCount);
	//�G�f�B�^�̃f�[�^
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

private:
	//
	int mType;
	//�t���t
	bool mMoveFlag;
	bool mDecisionFlag;
	//�f�[�^
	ResultUIData mData;
};