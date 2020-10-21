#include "stage_borad.h"

void StageBorad::CreateText(std::shared_ptr<TextureData> textureData, std::shared_ptr<LocalData> localData, const VECTOR2F& uv)
{
	mTexts.push_back(std::make_shared<SelectText>());
	mTexts.back()->SetLocalData(localData);
	mTexts.back()->SetTextureData(textureData);
	mTexts.back()->SetUV(uv);
}


void StageBorad::Update(int changeSelect, float timer, int nowStageNo, const VECTOR2F& centerPosition)
{
	int stageNoDifference = mStageNo - nowStageNo;
	mPosition = centerPosition;
	if (changeSelect == 0)
	{
		if (stageNoDifference != 0)
		{
			mPosition.x += stageNoDifference * (mSize.x + mInterval);
			mColor.w = 1 - (0.25f * abs(stageNoDifference));
			mScale.x = 1 - (0.15f * abs(stageNoDifference));
			mScale.y = mScale.x;
		}
		else
		{
			mColor.w = 1;
		}
		mPosition -= mSize* mScale * 0.5f;
	}
	else
	{
		int nextNo = nowStageNo + changeSelect;
		int nextStageNoDifference = mStageNo - nextNo;
		int difference = abs(nextStageNoDifference) - abs(stageNoDifference);
		if (difference > 0)
		{
			mColor.w =1 - (0.25f * abs(stageNoDifference))- 0.25f * timer;
			mScale.x =1 - (0.15f * abs(stageNoDifference))- 0.15f * timer;
		}
		else
		{
			mColor.w = 1 - (0.25f * abs(stageNoDifference)) + 0.25f * timer;
			mScale.x = 1 - (0.15f * abs(stageNoDifference)) + 0.15f * timer;
		}
		mScale.y = mScale.x;

		mPosition.x += stageNoDifference * (mSize.x + mInterval) + (mSize.x + mInterval) * (-changeSelect) * timer;
		mPosition -= mSize* mScale * 0.5f ;

	}
}

void StageBorad::Render(ID3D11DeviceContext* context, Sprite* sprite)
{
	for (auto& text : mTexts)
	{
		text->Render(context, sprite, mPosition, mScale, mSize, mColor);
	}
}
