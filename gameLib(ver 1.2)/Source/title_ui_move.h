#pragma once
#include"ui.h"
#include<vector>

class TitleUIMove
{
public:
	//�R���X�g���N�^
	TitleUIMove(const int UICount);
	//�X�V
	void Update(float elapsdTime, std::vector<std::shared_ptr<UI>>uis);
	//���Z�b�g
	void ResetTimer() 
	{ 
		mTimer = 0; 
		mMoveChangeFlag = false;
	}
	//�G�f�B�^�̃f�[�^
	struct TitleUIMoveData
	{
		float startTime;
		float startAlpha;
		float alphaAmount;
		float endAlpha;
	};
	//getter
	const bool GetMoveChangeFlag() { return mMoveChangeFlag; }
	std::vector<TitleUIMoveData>GetTitleUIMove() { return mDatas; }
	//setter
	void SetMoveChangeFlag(const bool flag) { mMoveChangeFlag = flag; }
	void SetTitleUIMove(std::vector<TitleUIMoveData>datas) { mDatas = datas; }
	//�e�X�g�v���C�J�n
	void TestPlay(std::vector<std::shared_ptr<UI>>uis)
	{
		mTimer = 0;
		mMoveChangeFlag = false;
		for (int i = 0; i < static_cast<int>(uis.size()); i++)
		{
			auto&uiData = uis[i]->GetUIData();
			auto& data = mDatas[i];
			uiData.mColor.w = data.startAlpha;
			uis[i]->SetUIData(uiData);
		}
	}
private:
	//�f�[�^
	std::vector<TitleUIMoveData>mDatas;
	//����
	float mTimer;
	bool mMoveChangeFlag;
};