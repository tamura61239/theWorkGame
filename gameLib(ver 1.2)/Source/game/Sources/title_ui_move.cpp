#include "title_ui_move.h"
#include"file_function.h"
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
TitleUIMove::TitleUIMove(const int UICount):mMoveChangeFlag(false)
{
	mDatas.resize(UICount);
	FileFunction::LoadArray(&mDatas[0], "Data/file/titleUIMove.bin", "rb");

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
void TitleUIMove::Update(float elapsdTime, std::vector<std::shared_ptr<UI>> uis)
{
	//�V�[����؂�ւ��鏈���ɓ��������ǂ���
	if (mMoveChangeFlag)return;
	mTimer += elapsdTime;
	for (int i = 0; i < static_cast<int>(uis.size()); i++)
	{
		auto& ui = uis[i];
		auto& data = mDatas[i];
		std::string name = "key";
			if (mTimer > data.startTime)
			{
				UI::UIData uiData = ui->GetUIData();
				uiData.mColor.w += data.alphaAmount * elapsdTime;
				//UI�̃A���t�@�l�����ȏ�ɂȂ������ǂ���
				if ((data.startAlpha-uiData.mColor.w)* (data.startAlpha - uiData.mColor.w)>= (data.startAlpha - data.endAlpha)* (data.startAlpha - data.endAlpha))
				{
					uiData.mColor.w = data.endAlpha;
					if (ui->GetName() == name)
					{//key�����̏����𖞂�������
						mMoveChangeFlag = true;
						break;
					}
					
				}
				ui->SetUIData(uiData);
			}

	}
}
