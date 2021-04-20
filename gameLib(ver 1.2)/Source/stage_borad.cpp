#include "stage_borad.h"

/*****************************************************/
//　　　　　　　　　　生成関数
/*****************************************************/
void StageBorad::CreateText(std::shared_ptr<TextureData> textureData, std::shared_ptr<LocalData> localData, const VECTOR2F& uv, const VECTOR4F& color)
{
	mTexts.push_back(std::make_shared<SelectText>());
	mTexts.back()->SetLocalData(localData);
	mTexts.back()->SetTextureData(textureData);
	mTexts.back()->SetUV(uv);
	mTexts.back()->SetColor(color);
}

/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void StageBorad::Update(int changeSelect, float timer, int nowStageNo, const VECTOR2F& centerPosition)
{
	//ボードのステージ番号
	int stageNoDifference = mStageNo - nowStageNo;
	mPosition = centerPosition;
	//スライド中かどうか
	if (changeSelect == 0)
	{//スライドしてない時
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
		//位置調整
		mPosition -= mSize* mScale * 0.5f;
	}
	else
	{//スライド中
		//スライド後のデータを計算する
		int nextNo = nowStageNo + changeSelect;
		int nextStageNoDifference = mStageNo - nextNo;
		//ボードが中央に近づくのかどうかを求める
		int difference = abs(nextStageNoDifference) - abs(stageNoDifference);
		if (difference > 0)
		{//遠ざかる
			mColor.w =1 - (0.25f * abs(stageNoDifference))- 0.25f * timer;
			mScale.x =1 - (0.15f * abs(stageNoDifference))- 0.15f * timer;
		}
		else
		{//近づく
			mColor.w = 1 - (0.25f * abs(stageNoDifference)) + 0.25f * timer;
			mScale.x = 1 - (0.15f * abs(stageNoDifference)) + 0.15f * timer;
		}
		mScale.y = mScale.x;
		//スライドさせる
		mPosition.x += stageNoDifference * (mSize.x + mInterval) + (mSize.x + mInterval) * (-changeSelect) * timer;
		mPosition -= mSize* mScale * 0.5f ;

	}
}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
void StageBorad::Render(ID3D11DeviceContext* context, Sprite* sprite)
{
	for (auto& text : mTexts)
	{
		text->Render(context, sprite, mPosition, mScale, mSize, mColor);
	}
}
