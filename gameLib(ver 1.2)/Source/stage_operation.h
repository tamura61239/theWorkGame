#pragma once
#include"stage_manager.h"

class StageOperation
{
public:
	StageOperation();
	void Load();
	void Save();
	void ImGuiUpdate();
	void Update(float elapsd_time, StageManager* manager,const bool playFlag);
	void SetStageColor(StageManager* manager);
	void Reset(StageManager* manager)
	{
		mColorType = 0;
		mChangFlag = false;
		SetStageColor(manager);
	}
	//getter
	const int GetColorType() { return mColorType; }
private:
	int mColorType;
	bool mChangFlag;
	VECTOR4F mColor[2];
};