#include "game_ui_move.h"
#include"file_function.h"

GameUiMove::GameUiMove() :mStartFlag(false), mState(0), mTime(0), mCount(0), mGameUIData(), mTestFlag(false)
{
	FileFunction::Load(mGameUIData, "Data/file/gameUIData.bin", "rb");
}

void GameUiMove::Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis)
{
	if (!mStartFlag && !mTestFlag)return;

	if (mState == 0)
	{
		mCount -= elapsdTime;
		if (mCount <= 0)
		{
			mCount = 0;
			mState++;
		}

		for (auto& ui : uis)
		{
			CountMove(elapsdTime, ui);
		}
	}
	else if (mState == 1)
	{
		mTime -= elapsdTime;
		if (mTime <= 0)mTime = 0;
		for (auto& ui : uis)
		{
			TimeMove(elapsdTime, ui);
		}
	}
}

void GameUiMove::SetUI(std::vector<std::shared_ptr<UI>> uis)
{
	for (auto& ui : uis)
	{
		auto& data = ui->GetUIData();
		if (ui->GetName()._Equal("count"))
		{
			int number = static_cast<int>(mGameUIData.mMaxCount);
			data.mTextureLeftTop.x = data.mTextureSize.x * number;
			data.mColor.w = 1;
		}
		else if (ui->GetName()._Equal("time1000"))
		{
			int number = static_cast<int>(mGameUIData.mMaxTime * 100) / 1000;
			data.mTextureLeftTop.x = data.mTextureSize.x * number;
			data.mColor.w = 1;
		}
		else if (ui->GetName()._Equal("time100"))
		{
			int number = (static_cast<int>(mGameUIData.mMaxTime * 100) % 1000) / 100;
			data.mTextureLeftTop.x = data.mTextureSize.x * number;
			data.mColor.w = 1;

		}
		else if (ui->GetName()._Equal("time10"))
		{
			int number = (static_cast<int>(mGameUIData.mMaxTime * 100) % 100) / 10;
			data.mTextureLeftTop.x = data.mTextureSize.x * number;
			data.mColor.w = 1;

		}
		else if (ui->GetName()._Equal("time1"))
		{
			int number = static_cast<int>(mGameUIData.mMaxTime * 100) % 10;
			data.mTextureLeftTop.x = data.mTextureSize.x * number;
			data.mColor.w = 1;

		}

		ui->SetUIData(data);
	}
}


void GameUiMove::CountMove(float elapsdTime, std::shared_ptr<UI> ui)
{
	if (!ui->GetName()._Equal("count"))return;
	auto& data = ui->GetUIData();
	int number = static_cast<int>(mCount + 1);
	data.mTextureLeftTop.x = data.mTextureSize.x * number;
	if (mCount <= 0)data.mColor.w = 0;
	ui->SetUIData(data);
}

void GameUiMove::TimeMove(float elapsdTime, std::shared_ptr<UI> ui)
{
	if (ui->GetName()._Equal("count"))return;

	auto& data = ui->GetUIData();
	if (ui->GetName()._Equal("time1000"))
	{
		int number = static_cast<int>(mTime * 100) / 1000;
		data.mTextureLeftTop.x = data.mTextureSize.x * number;
		data.mColor.w = 1;
	}
	else if (ui->GetName()._Equal("time100"))
	{
		int number = (static_cast<int>(mTime * 100) % 1000) / 100;
		data.mTextureLeftTop.x = data.mTextureSize.x * number;
		data.mColor.w = 1;

	}
	else if (ui->GetName()._Equal("time10"))
	{
		int number = (static_cast<int>(mTime * 100) % 100) / 10;
		data.mTextureLeftTop.x = data.mTextureSize.x * number;
		data.mColor.w = 1;

	}
	else if (ui->GetName()._Equal("time1"))
	{
		int number = static_cast<int>(mTime * 100) % 10;
		data.mTextureLeftTop.x = data.mTextureSize.x * number;
		data.mColor.w = 1;
	}
	ui->SetUIData(data);

}
