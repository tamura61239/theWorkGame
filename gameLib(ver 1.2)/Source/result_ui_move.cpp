#include "result_ui_move.h"
#include"key_board.h"

ResultUIMove::ResultUIMove():mMoveFlag(false),mType(0), mDecisionFlag(false)
{
}

void ResultUIMove::Update(float elapsdTime, std::vector<std::shared_ptr<UI>> uis)
{
	if (!mMoveFlag)
	{
		mType = 0;
		mDecisionFlag = false;
		for (auto& ui : uis)
		{
			auto& data = ui->GetUIData();
			data.mColor.w = 0.5f;
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
		mType = 1;
	}
	if (pKeyBoad.RisingState(KeyLabel::LEFT))
	{
		mType = 0;
	}
	if (pKeyBoad.RisingState(KeyLabel::SPACE))
	{
		mDecisionFlag = true;
	}
	for (int i = 0; i < uis.size(); i++)
	{
		auto& ui = uis[i];
		auto& data = ui->GetUIData();
		if (mType == i)
		{
			data.mColor.w = 1;
		}
		else
		{
			data.mColor.w = 0.6f;
		}
		ui->SetUIData(data);
	}

}
