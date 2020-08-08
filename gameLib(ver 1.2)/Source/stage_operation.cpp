#include "stage_operation.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
#include"key_board.h"

StageOperation::StageOperation():mChangFlag(true),mColorType(0)
{
	Load();
}

void StageOperation::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/stageColor.txt", "r") == 0)
	{
		fread(&mColor[0], sizeof(VECTOR4F), 1, fp);
		fread(&mColor[1], sizeof(VECTOR4F), 1, fp);
		fclose(fp);
		mChangFlag = false;
		return;
	}
	mColor[0] = VECTOR4F(1, 0, 0, 1);
	mColor[1] = VECTOR4F(0, 0, 1, 0.3f);
}

void StageOperation::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/stageColor.txt", "w");
	fwrite(&mColor[0], sizeof(VECTOR4F), 1, fp);
	fwrite(&mColor[1], sizeof(VECTOR4F), 1, fp);
	fclose(fp);
}

void StageOperation::ImGuiUpdate()
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
	if (ImGui::Button("save"))Save();
	ImGui::End();
#endif
}

void StageOperation::Update(float elapsd_time, StageManager* manager)
{
	ImGuiUpdate();
	if (pKeyBoad.RisingState(KeyLabel::SPACE)&&!mChangFlag)
	{
		mChangFlag = true;
		mColorType++;
		if (mColorType >= 2)mColorType = 0;
	}
	if (mChangFlag)
	{
		for (auto& stage : manager->GetStages())
		{
			switch (mColorType)
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
}
