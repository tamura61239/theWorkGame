#pragma once
#include"ui.h"
#include<vector>

class GameUiMove
{
public:
	//コンストラクタ
	GameUiMove();
	//更新
	void Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis);
	//UIのUV値の設定
	void SetUI(std::vector<std::shared_ptr<UI>>uis);
	//開始
	void Start()
	{
		mTime = mGameUIData.mMaxTime;
		mCount = mGameUIData.mMaxCount;
		mStartFlag = true;
		mState = 0;
	}
	//テストプレイ開始
	void TestStart()
	{
		mTime = mGameUIData.mMaxTime;
		mCount = mGameUIData.mMaxCount;
		mTestFlag = true;
		mState = 0;
	}
	//エディタ用データ
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
private:
	//カウントの動き
	void CountMove(float elapsdTime, std::shared_ptr<UI>ui);
	//タイムの動き
	void TimeMove(float elapsdTime, std::shared_ptr<UI>ui);
	//データ
	GameUIData mGameUIData;
	//時間
	float mTime;
	//開始フラグ
	bool mStartFlag;
	//テストプレイ開始フラグ
	bool mTestFlag;
	//状態
	int mState;
	//カウント
	float mCount;
};