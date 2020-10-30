#include "title_ui_move.h"

TitleUIMove::TitleUIMove(const int UICount):mMoveChangeFlag(false)
{
	mDatas.resize(UICount);
	Load();
}

void TitleUIMove::Update(float elapsdTime, std::vector<std::shared_ptr<UI>> uis)
{
	if (mMoveChangeFlag)return;
	mTimer += elapsdTime;
	for (int i = 0; i < uis.size(); i++)
	{
		auto& ui = uis[i];
		auto& data = mDatas[i];
		std::string name = "key";
			if (mTimer > data.startTime)
			{
				UI::UIData uiData = ui->GetUIData();
				uiData.mColor.w += data.alphaAmount * elapsdTime;
				if (sqrtf((uiData.mColor.w - data.endAlpha)*(uiData.mColor.w - data.endAlpha)) <= sqrtf(data.alphaAmount * elapsdTime* data.alphaAmount * elapsdTime))
				{
					if (ui->GetName() == name)
					{
						mMoveChangeFlag = true;
						break;
					}
					
				}
				ui->SetUIData(uiData);
			}

		//else
		//{
		//	if (ui->GetName() != name)continue;

		//}
	}
}

void TitleUIMove::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/titleUIMove.bin", "rb") == 0)
	{
		for(auto & data:mDatas)
		{
			fread(&data, sizeof(TitleUIMoveData), 1, fp);
		}
		fclose(fp);
	}
}

void TitleUIMove::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/titleUIMove.bin", "wb");
	{
		for (auto& data : mDatas)
		{
			fwrite(&data, sizeof(TitleUIMoveData), 1, fp);
		}
		fclose(fp);
	}
}
