#include "stage_operation.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
#include"key_board.h"
//�R���X�g���N�^
StageOperation::StageOperation() :mChangFlag(true), mColorType(0)
{
	mColor[0] = VECTOR4F(1, 0, 0, 1);
	mColor[1] = VECTOR4F(0, 0, 1, 0.3f);

	FileFunction::LoadArray(&mColor[0], "Data/file/stageColor.txt", "r");
}
/*****************�G�f�B�^�֐�********************/
void StageOperation::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("stageColor");
	float* color0[4] = { &mColor[0].x,&mColor[0].y ,&mColor[0].z ,&mColor[0].w };
	if (ImGui::ColorEdit4("color0", *color0))
	{
		mChangFlag = true;
	}
	float* color1[4] = { &mColor[1].x,&mColor[1].y ,&mColor[1].z ,&mColor[1].w };
	if (ImGui::ColorEdit4("color1", *color1))
	{
		mChangFlag = true;
	}
	if (ImGui::Button("save"))FileFunction::SaveArray(&mColor[0], 2, "Data/file/stageColor.txt", "w");
	ImGui::End();
#endif
}
/********************�X�V�֐�***********************/
void StageOperation::Update(float elapsd_time, StageManager* manager, const bool playFlag)
{
	if (!playFlag)
	{//�v���C���ĂȂ���
	}
	else
	{//�v���C��
		if (pKeyBoad.RisingState(KeyLabel::SPACE) && !mChangFlag)//space�L�[���������Ƃ�
		{
			mChangFlag = true;
			mColorType++;
			if (mColorType >= 2)mColorType = 0;
		}
	}
	SetStageColor(manager);
}
//�I�u�W�F�N�g�ɐF��ݒ肷��
void StageOperation::SetStageColor(StageManager* manager)
{
	for (auto& stage : manager->GetStages())
	{
		switch (mColorType)//�F��ݒ肷��
		{
		case 0:
			stage->SetColor(mColor[stage->GetStageData().mColorType]);
			break;
		case 1:
			stage->SetColor(mColor[1 - stage->GetStageData().mColorType]);
			break;
		}
	}
	mChangFlag = false;
}
