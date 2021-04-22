#include "title_ui_move.h"
#include"file_function.h"
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
TitleUIMove::TitleUIMove(const int UICount):mMoveChangeFlag(false)
{
	mDatas.resize(UICount);
	FileFunction::LoadArray(&mDatas[0], "Data/file/titleUIMove.bin", "rb");

}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void TitleUIMove::Update(float elapsdTime, std::vector<std::shared_ptr<UI>> uis)
{
	//シーンを切り替える処理に入ったかどうか
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
				//UIのアルファ値が一定以上になったかどうか
				if ((data.startAlpha-uiData.mColor.w)* (data.startAlpha - uiData.mColor.w)>= (data.startAlpha - data.endAlpha)* (data.startAlpha - data.endAlpha))
				{
					uiData.mColor.w = data.endAlpha;
					if (ui->GetName() == name)
					{//keyがこの条件を満たしたら
						mMoveChangeFlag = true;
						break;
					}
					
				}
				ui->SetUIData(uiData);
			}

	}
}
