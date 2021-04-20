#include "stage_borad.h"

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�����֐�
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
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
void StageBorad::Update(int changeSelect, float timer, int nowStageNo, const VECTOR2F& centerPosition)
{
	//�{�[�h�̃X�e�[�W�ԍ�
	int stageNoDifference = mStageNo - nowStageNo;
	mPosition = centerPosition;
	//�X���C�h�����ǂ���
	if (changeSelect == 0)
	{//�X���C�h���ĂȂ���
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
		//�ʒu����
		mPosition -= mSize* mScale * 0.5f;
	}
	else
	{//�X���C�h��
		//�X���C�h��̃f�[�^���v�Z����
		int nextNo = nowStageNo + changeSelect;
		int nextStageNoDifference = mStageNo - nextNo;
		//�{�[�h�������ɋ߂Â��̂��ǂ��������߂�
		int difference = abs(nextStageNoDifference) - abs(stageNoDifference);
		if (difference > 0)
		{//��������
			mColor.w =1 - (0.25f * abs(stageNoDifference))- 0.25f * timer;
			mScale.x =1 - (0.15f * abs(stageNoDifference))- 0.15f * timer;
		}
		else
		{//�߂Â�
			mColor.w = 1 - (0.25f * abs(stageNoDifference)) + 0.25f * timer;
			mScale.x = 1 - (0.15f * abs(stageNoDifference)) + 0.15f * timer;
		}
		mScale.y = mScale.x;
		//�X���C�h������
		mPosition.x += stageNoDifference * (mSize.x + mInterval) + (mSize.x + mInterval) * (-changeSelect) * timer;
		mPosition -= mSize* mScale * 0.5f ;

	}
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
void StageBorad::Render(ID3D11DeviceContext* context, Sprite* sprite)
{
	for (auto& text : mTexts)
	{
		text->Render(context, sprite, mPosition, mScale, mSize, mColor);
	}
}
