#pragma once
#include"stage_manager.h"

class StageOperation
{
public:
	//コンストラクタ
	StageOperation();
	//エディタ
	void Editor();
	//更新
	void Update(float elapsd_time, StageManager* manager,const bool playFlag);
	//色の設定
	void SetStageColor(StageManager* manager);
	//リセット
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