#include "game_ui_move.h"

GameUiMove::GameUiMove() :mStartFlag(false), mState(0), mTime(0),mCount(0),mGameUIData(),mTestFlag(false)
{
	Load();
}

void GameUiMove::Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis)
{
	if (!mStartFlag&&!mTestFlag)return;
	for (auto& ui : uis)
	{
		if (mState == 0)
		{
			CountMove(elapsdTime, ui);
		}
		else if (mState == 1)
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
		else if (ui->GetName()._Equal("time"))
		{

		}
		ui->SetUIData(data);
	}
}

void GameUiMove::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/gameUIData.bin", "wb");
	fwrite(&mGameUIData, sizeof(GameUIData), 1, fp);
	fclose(fp);
}

void GameUiMove::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/gameUIData.bin", "rb") == 0)
	{
		fread(&mGameUIData, sizeof(GameUIData), 1, fp);
		fclose(fp);
	}
}

void GameUiMove::CountMove(float elapsdTime, std::shared_ptr<UI> ui)
{
	if (!ui->GetName()._Equal("count"))return;
	auto& data = ui->GetUIData();
	mCount -= elapsdTime;
	int number = static_cast<int>(mCount + 1);
	data.mTextureLeftTop.x = data.mTextureSize.x * number;
	if (mCount <= 0)
	{
		mCount = 0;
		data.mColor.w = 0;
		mState++;
	}
	ui->SetUIData(data);
}

void GameUiMove::TimeMove(float elapsdTime, std::shared_ptr<UI> ui)
{
	if (!ui->GetName()._Equal("time"))return;
	auto& data = ui->GetUIData();
}
