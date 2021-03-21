#include "result_ui_move.h"
#include"key_board.h"

ResultUIMove::ResultUIMove() :mMoveFlag(false), mType(0), mDecisionFlag(false)
{
	mData.frameAlpth = 0.2f;
	mData.alpthDifference = 0.65f;
	Load();
}

void ResultUIMove::Update(float elapsdTime, std::vector<std::shared_ptr<UI>> uis, size_t uiCount)
{
	if (!mMoveFlag)
	{
		mType = 0;
		mDecisionFlag = false;
		for (auto& ui : uis)
		{
			auto& data = ui->GetUIData();
			data.mColor.w = 0.f;
			ui->SetUIData(data);
		}
		return;
	}
	if (mDecisionFlag)
	{
		return;
	}
	if (pKeyBoad.RisingState(KeyLabel::RIGHT))
	{
		mType = mType < uiCount - 2 ? mType + 1 : mType;
	}
	if (pKeyBoad.RisingState(KeyLabel::LEFT))
	{
		mType = mType > 0 ? mType - 1 : mType;
	}
	if (pKeyBoad.RisingState(KeyLabel::SPACE))
	{
		mDecisionFlag = true;
	}
	VECTOR2F position = VECTOR2F(0, 0);
	int frameNo = 0;
	for (int i = 0; i < static_cast<int>(uiCount); i++)
	{
		auto& ui = uis[i];
		auto& data = ui->GetUIData();
		if (!ui->GetName()._Equal("frame"))
		{
			data.mColor.w = i - 1 == mType ? 1 : 1 - mData.alpthDifference/*1.f - (mType)*mData.alpthDifference*/;
			if (data.mColor.w >= 1.0f)
			{
				position = data.mLeftPosition + data.mDrowSize * 0.5f;
			}
		}
		else
		{
			frameNo = i;
			data.mColor.w = mData.frameAlpth;
		}
		ui->SetUIData(data);
	}
	auto& frameData = uis[frameNo]->GetUIData();
	frameData.mLeftPosition = position - frameData.mDrowSize * 0.5f;
	uis[frameNo]->SetUIData(frameData);
}

void ResultUIMove::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/resultUIData.bin", "wb");
	fwrite(&mData, sizeof(ResultUIData), 1, fp);
	fclose(fp);

}

void ResultUIMove::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/resultUIData.bin", "rb") == 0)
	{
		fread(&mData, sizeof(ResultUIData), 1, fp);
		fclose(fp);
	}

}
