#pragma once
#include"sprite.h"
#include<vector>
class TimeText
{
public:
	TimeText(float time);
	//setter
	void SetLeftTop(const VECTOR2F& leftTop) { mLeftTop = leftTop; }
	void SetTextSize(const VECTOR2F& textSize) 
	{ 
		mTextSize = textSize; 
		mSizeX = mTextSize.y * 0.01f * 62.f;
	}
	void SetTime(float time)
	{
		mTime = static_cast<int>(time * 100);
	}
	//getter
	const VECTOR2F& GetLeftTop() { return mLeftTop; }
	const VECTOR2F& GetTextSize() { return mTextSize; }
	void Render(ID3D11DeviceContext* context, Sprite* sprite);
private:
	int mTime;
	VECTOR2F mLeftTop;
	VECTOR2F mTextSize;
	float mSizeX;
};
class Ranking
{
public:
	Ranking(ID3D11Device* device, float time);
	void Update(float elapsdTime,bool play);
	void Render(ID3D11DeviceContext* context);
	void ImGuiUpdate();
	//setter
	static void SetStageNo(const int stageNo) { mStageNo = stageNo; }
	static int GetStageNo() { return mStageNo; }
private:
	void ScereMove(float elapsdTime);
	void RankingMove(float elapsdTime);
	void NewRankingMove(float elapsdTime);
	void RankingTimeSort();
	void SetRankingData();
	void QuickSort(int array[], int left, int right);
	int Partition(int array[], int left, int right);
	void Swap(int* x, int* y);
	void Load();
	void Save();
private:
	static int mStageNo;
	std::unique_ptr<Sprite>mNumberTest;
	std::unique_ptr<Sprite>mRankTest;
	std::unique_ptr<TimeText>mNowPlayTimeText;
	std::vector<std::unique_ptr<TimeText>>mRankingTexts;
	int mState;
	float mTimer;
	float mNowPlayTime;
	bool mTestFlag;
	int mNowPlayRank;
	int mNewPlayMove;
	VECTOR4F mNoRankTextColor;
	struct ScoreData
	{
		VECTOR2F mLeftTop;
		VECTOR2F mTextSize;
	};
	struct RankingData
	{
		VECTOR2F mLeftTop;
		VECTOR2F mTextSize;
		float mIntervalY;
		float mStartX;
		float mStartTime;
		float mMoveTime;
	};
	ScoreData mNowPlayScoreTime;
	RankingData mRankingData;
};