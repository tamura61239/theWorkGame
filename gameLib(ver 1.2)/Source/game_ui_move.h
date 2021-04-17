#pragma once
#include"ui.h"
#include<vector>

class GameUiMove
{
public:
	//�R���X�g���N�^
	GameUiMove();
	//�X�V
	void Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis);
	//UI��UV�l�̐ݒ�
	void SetUI(std::vector<std::shared_ptr<UI>>uis);
	//�J�n
	void Start()
	{
		mTime = mGameUIData.mMaxTime;
		mCount = mGameUIData.mMaxCount;
		mStartFlag = true;
		mState = 0;
	}
	//�e�X�g�v���C�J�n
	void TestStart()
	{
		mTime = mGameUIData.mMaxTime;
		mCount = mGameUIData.mMaxCount;
		mTestFlag = true;
		mState = 0;
	}
	//�G�f�B�^�p�f�[�^
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
	//�J�E���g�̓���
	void CountMove(float elapsdTime, std::shared_ptr<UI>ui);
	//�^�C���̓���
	void TimeMove(float elapsdTime, std::shared_ptr<UI>ui);
	//�f�[�^
	GameUIData mGameUIData;
	//����
	float mTime;
	//�J�n�t���O
	bool mStartFlag;
	//�e�X�g�v���C�J�n�t���O
	bool mTestFlag;
	//���
	int mState;
	//�J�E���g
	float mCount;
};