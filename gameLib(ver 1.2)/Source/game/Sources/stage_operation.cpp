#include "stage_operation.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
#include"key_board.h"
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
StageOperation::StageOperation() : mColorType(0)
{
	mColor[0] = VECTOR4F(1, 0, 0, 1);
	mColor[1] = VECTOR4F(0, 0, 1, 0.3f);

	FileFunction::LoadArray(&mColor[0], "Data/file/stageColor.txt", "r");
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void StageOperation::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("stageColor");
	float* color0[4] = { &mColor[0].x,&mColor[0].y ,&mColor[0].z ,&mColor[0].w };
	if (ImGui::ColorEdit4("color0", *color0))
	{
	}
	float* color1[4] = { &mColor[1].x,&mColor[1].y ,&mColor[1].z ,&mColor[1].w };
	if (ImGui::ColorEdit4("color1", *color1))
	{
	}
	if (ImGui::Button("save"))FileFunction::SaveArray(&mColor[0], 2, "Data/file/stageColor.txt", "w");
	ImGui::End();
#endif
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
/**************************キーを押したかどうかの更新***************************/
void StageOperation::Update(const bool playFlag)
{
	if (playFlag)
	{//プレイ中
		if (pKeyBoad.RisingState(KeyLabel::SPACE))//spaceキーを押したとき
		{
			mColorType++;
			if (mColorType >= 2)mColorType = 0;
		}
	}
}
/*************************オブジェクトに色を設定する*************************/
void StageOperation::SetStageColor(StageObj* obj)
{
	switch (mColorType)//色を設定する
	{
	case 0:
		obj->SetColor(mColor[obj->GetStageData().mColorType]);
		break;
	case 1:
		obj->SetColor(mColor[1 - obj->GetStageData().mColorType]);
		break;
	}
}
