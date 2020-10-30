#pragma once
#include"ui.h"
#include<vector>

class GameUiMove
{
public:
	GameUiMove();
	void Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis);
	void SetUI(std::vector<std::shared_ptr<UI>>uis);
	void Start()
	{
		mTime = mGameUIData.mMaxTime;
		mCount = mGameUIData.mMaxCount;
		mStartFlag = true;
		mState = 0;
	}
	void TestStart()
	{
		mTime = mGameUIData.mMaxTime;
		mCount = mGameUIData.mMaxCount;
		mTestFlag = true;
		mState = 0;
	}
	struct GameUIData
	{
		GameUIData() :mMaxCount(3), mMaxTime(0), mCountEndScale(0), mCountAlphaTime(0) {}
		float mMaxTime;
		float mMaxCount;
		float mCountEndScale;
		float mCountAlphaTime;
	};

	//setter
	void SetStartFlag(const bool flag)
	{
		mStartFlag = flag;
	}
	void SetTestFlag(const bool flag)
	{
		mTestFlag = flag;
	}
	void SetGameUIData(const GameUIData& data) { mGameUIData = data; }
	//getter
	const float GetCount() { return mCount; }
	GameUIData GetGameUIData() { return mGameUIData; }
	const float GetTime() { return mTime; }
	const bool GetStartFlag() { return mStartFlag; }
	const bool GetTestFlag() { return mTestFlag; }
	void Save();
private:
	void Load();
	void CountMove(float elapsdTime, std::shared_ptr<UI>ui);
	void TimeMove(float elapsdTime, std::shared_ptr<UI>ui);
	GameUIData mGameUIData;
	VECTOR2F mSaveSize;
	float mTime;
	bool mStartFlag;
	bool mTestFlag;
	int mState;
	float mCount;
};