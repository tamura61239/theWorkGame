#pragma once
#include"stage_obj.h"

class StageOperation
{
public:
	//コンストラクタ
	StageOperation();
	//エディタ
	void Editor();
	//更新
	void Update(const bool playFlag);
	//色の設定
	void SetStageColor(StageObj* obj);
	//リセット
	void Reset()
	{
		mColorType = 0;
	}
	//getter
	const int GetColorType() { return mColorType; }
private:
	int mColorType;
	VECTOR4F mColor[2];
};