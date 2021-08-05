#pragma once
#include"stage_obj.h"

class StageOperation
{
public:
	//�R���X�g���N�^
	StageOperation();
	//�G�f�B�^
	void Editor();
	//�X�V
	void Update(const bool playFlag);
	//�F�̐ݒ�
	void SetStageColor(StageObj* obj);
	//���Z�b�g
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