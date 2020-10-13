#include "ui_manager.h"
#ifdef USE_IMGUI
#include"imgui.h"
#endif

void UIManager::TitleInitialize(ID3D11Device* device)
{
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/siro.png", VECTOR2F(1024, 1024), "scene"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/ƒ^ƒCƒgƒ‹.png", VECTOR2F(1000, 200), "title"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/startKey.png", VECTOR2F(500, 100), "key"));
	mTitleMove = std::make_unique<TitleUIMove>(mUIs.size());
	for (int i=0;i< mUIs.size();i++)
	{
		auto& ui = mUIs[i];
		
		mNames.push_back(ui->GetName());
		UI::UIData data = ui->GetUIData();
		TitleUIMove::TitleUIMoveData moveData = mTitleMove->GetTitleUIMove()[i];
		data.mColor.w = moveData.startAlpha;
		ui->SetUIData(data);
	}
	mUINumber = 0;
	mSceneName = "title";
#ifdef USE_IMGUI
	mMoveStartFlag = false;
#else
	mMoveStartFlag = true;
#endif
	Load(mSceneName.c_str());

}

void UIManager::GameInitialize(ID3D11Device* device)
{
	for (auto& ui : mUIs)
	{
		mNames.push_back(ui->GetName());
	}
	mUINumber = 0;
	mSceneName = "game";
	Load(mSceneName.c_str());

}

void UIManager::Clear()
{
	mUIs.clear();
	mNames.clear();
	mTitleMove.reset(nullptr);
}
void UIManager::ClearUI()
{
	mUIs.clear();
	mNames.clear();
}
static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

void UIManager::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("ui");
	if (!mMoveStartFlag)
	{
		ImGui::Combo("name", &mUINumber, vector_getter, static_cast<void*>(&mNames), mNames.size());
		{
			UI::UIData data = mUIs[mUINumber]->GetUIData();
			ImGui::Text("drow data");
			float* color[3] = { &data.mColor.x,&data.mColor.y ,&data.mColor.z };
			ImGui::ColorEdit3("color", *color);
			ImGui::SliderFloat("left position x", &data.mLeftPosition.x, 0, 1920);
			ImGui::SliderFloat("left position y", &data.mLeftPosition.y, 0, 1080);
			float* leftPosition[2] = { &data.mLeftPosition.x ,&data.mLeftPosition.y };
			ImGui::InputFloat2("left position", *leftPosition);
			ImGui::SliderFloat("size x", &data.mDrowSize.x, 0, 1920);
			ImGui::SliderFloat("size y", &data.mDrowSize.y, 0, 1080);
			float* drowSize[2] = { &data.mDrowSize.x ,&data.mDrowSize.y };
			ImGui::InputFloat2("size", *drowSize);
			ImGui::Text("texture data");
			ImGui::SliderFloat("texture life top x", &data.mTextureLeftTop.x, 0, 1920);
			ImGui::SliderFloat("texture life top y", &data.mTextureLeftTop.y, 0, 1080);
			ImGui::InputFloat("texture size x", &data.mTextureSize.x, 10);
			ImGui::InputFloat("texture size y", &data.mTextureSize.y, 10);
			if (mSceneName._Equal("title"))
			{
				ImGui::Text("title data");
				std::vector<TitleUIMove::TitleUIMoveData> titleData = mTitleMove->GetTitleUIMove();
				ImGui::InputFloat("start time", &titleData[mUINumber].startTime, 0.1f);
				ImGui::SliderFloat("start alpha", &titleData[mUINumber].startAlpha, 0, 1);
				ImGui::SliderFloat("end alpha", &titleData[mUINumber].endAlpha, 0, 1);
				ImGui::InputFloat("alpha amount", &titleData[mUINumber].alphaAmount, 0.1f);
				if (ImGui::Button("title save"))
				{
					mTitleMove->Save();
				}
				mTitleMove->SetTitleUIMove(titleData);
				mUIs[mUINumber]->SetUIData(data);
				for (int i = 0; i < 3; i++)
				{
					auto& ui = mUIs[i];

					UI::UIData uiData = ui->GetUIData();
					TitleUIMove::TitleUIMoveData moveData = mTitleMove->GetTitleUIMove()[i];
					uiData.mColor.w = titleData[i].startAlpha;
					ui->SetUIData(uiData);
				}
			}

		}

	}
	ImGui::Checkbox("move start", &mMoveStartFlag);
	if (ImGui::Button("save"))
	{
		Save(mSceneName.c_str());

	}
	ImGui::End();
#endif
}

void UIManager::Update(float elapsdTime)
{

	if (mSceneName._Equal("title"))
	{
		if (!mMoveStartFlag)
		{
			mTitleMove->ResetTimer();
			return;
		}
		mTitleMove->Update(elapsdTime, mUIs);
	}
	else if (mSceneName._Equal("game"))
	{
		if (!mMoveStartFlag)
		{
			return;
		}
	}
}

void UIManager::Render(ID3D11DeviceContext* context)
{
	for (auto& ui : mUIs)
	{
		ui->Render(context);
	}
}

void UIManager::Load(const char* scene)
{
	FILE* fp;
	std::string fileName = { "Data/file/" };
	fileName += scene;
	fileName += "UI";
	fileName += ".bin";
	if (fopen_s(&fp, fileName.c_str(), "rb") == 0)
	{
		UI::UIData data;
		for (int i=0;i<3;i++)
		{
			data = mUIs[i]->GetUIData();
			fread(&data, sizeof(UI::UIData), 1, fp);
			mUIs[i]->SetUIData(data);
		}
		fclose(fp);
	}
}

void UIManager::Save(const char* scene)
{
	FILE* fp;
	std::string fileName = { "Data/file/" };
	fileName += scene;
	fileName += "UI";
	fileName += ".bin";
	fopen_s(&fp, fileName.c_str(), "wb");
	{
		for (int i = 0; i < 3; i++)
		{
			UI::UIData data = mUIs[i]->GetUIData();
			fwrite(&data, sizeof(UI::UIData), 1, fp);
		}
		fclose(fp);
	}
}
