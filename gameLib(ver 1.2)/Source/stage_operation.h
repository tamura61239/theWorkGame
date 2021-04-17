#pragma once
#include"stage_manager.h"

class StageOperation
{
public:
	//�R���X�g���N�^
	StageOperation();
	//�G�f�B�^
	void Editor();
	//�X�V
	void Update(float elapsd_time, StageManager* manager,const bool playFlag);
	//�F�̐ݒ�
	void SetStageColor(StageManager* manager);
	//���Z�b�g
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