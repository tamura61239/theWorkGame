#include "ui_manager.h"
#include"gpu_particle_manager.h"
#include"vector_combo.h"
#ifdef USE_IMGUI
#include"imgui.h"
#endif

void UIManager::TitleInitialize(ID3D11Device* device)
{
	mDebugUIFrame = std::make_unique<UI>(device, L"Data/image/枠.png", VECTOR2F(500, 500), "");
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/タイトル.png", VECTOR2F(1000, 200), "title"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/startKey.png", VECTOR2F(500, 100), "key"));
	mTitleMove = std::make_unique<TitleUIMove>(mUIs.size());
	mSceneName = "title";
	Load(mSceneName.c_str());

	for (int i=0;i< static_cast<int>(mUIs.size());i++)
	{
		auto& ui = mUIs[i];
		
		mNames.push_back(ui->GetName());
		UI::UIData data = ui->GetUIData();
		TitleUIMove::TitleUIMoveData moveData = mTitleMove->GetTitleUIMove()[i];
		data.mColor.w = moveData.startAlpha;
		ui->SetUIData(data);
	}
	pGpuParticleManager->GetTitleTextureParticle()->LoadTexture(device, L"Data/image/タイトル.png", mUIs[0]->GetUIData().mLeftPosition, mUIs[0]->GetUIData().mDrowSize, mUIs[0]->GetUIData().mTextureLeftTop, mUIs[0]->GetUIData().mTextureSize);
	pGpuParticleManager->GetTitleTextureParticle()->LoadTexture(device, L"Data/image/startKey.png", mUIs[1]->GetUIData().mLeftPosition, mUIs[1]->GetUIData().mDrowSize, mUIs[1]->GetUIData().mTextureLeftTop, mUIs[1]->GetUIData().mTextureSize);

	mUINumber = 0;
	mDebugUIFrameFlag = false;
#ifdef USE_IMGUI
	mMoveStartFlag = false;
#else
	mMoveStartFlag = true;
#endif

}

void UIManager::GameInitialize(ID3D11Device* device)
{
	mDebugUIFrame = std::make_unique<UI>(device, L"Data/image/枠.png", VECTOR2F(500, 500), "");

	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "count"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time1000"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time100"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time10"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time1"));
	mGameMove = std::make_unique<GameUiMove>();
	mGameMove->SetUI(mUIs);
	for (auto& ui : mUIs)
	{
		mNames.push_back(ui->GetName());
	}
	mUINumber = 0;
	mSceneName = "game";
	Load(mSceneName.c_str());
	mDebugUIFrameFlag = false;

}

void UIManager::ResultInitialize(ID3D11Device* device)
{
	mDebugUIFrame = std::make_unique<UI>(device, L"Data/image/枠.png", VECTOR2F(500, 500), "");

	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/○.png", VECTOR2F(800, 800), "frame"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/retry.png", VECTOR2F(250, 80), "retry"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/title.png", VECTOR2F(250, 80), "title"));

	mResultMove = std::make_unique<ResultUIMove>();
	for (auto& ui : mUIs)
	{
		mNames.push_back(ui->GetName());
	}
	mUINumber = 0;
	mSceneName = "result";
	Load(mSceneName.c_str());
	mDebugUIFrameFlag = false;

}

void UIManager::Clear()
{
	mUIs.clear();
	mNames.clear();
	mTitleMove.reset(nullptr);
	mGameMove.reset(nullptr);
	mResultMove.reset(nullptr);
}
void UIManager::ClearUI()
{
	mUIs.clear();
	mNames.clear();
}

void UIManager::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("ui");
	ImGui::Checkbox("debug ui frame", &mDebugUIFrameFlag);
	if (mUIs.size()!=0)
	{
		ImGui::Combo("name", &mUINumber, vector_getter, static_cast<void*>(&mNames), mNames.size());
		{
			UI::UIData data = mUIs[mUINumber]->GetUIData();
			ImGui::Text("drow data");
			float* color[3] = { &data.mColor.x,&data.mColor.y ,&data.mColor.z };
			ImGui::ColorEdit3("color", *color);
			ImGui::InputFloat("alpha", &data.mColor.w);
			ImGui::SliderFloat("left position x", &data.mLeftPosition.x, 0, 1920);
			ImGui::SliderFloat("left position y", &data.mLeftPosition.y, 0, 1080);
			float* leftPosition[2] = { &data.mLeftPosition.x ,&data.mLeftPosition.y };
			ImGui::InputFloat2("left position", *leftPosition);
			ImGui::SliderFloat("size x", &data.mDrowSize.x, 0, 1920);
			ImGui::SliderFloat("size y", &data.mDrowSize.y, 0, 1080);
			float* drowSize[2] = { &data.mDrowSize.x ,&data.mDrowSize.y };
			ImGui::InputFloat2("size", *drowSize);

			ImGui::Text("texture data");
			if (mSceneName._Equal("title"))
			{
				ImGui::SliderFloat("texture life top x", &data.mTextureLeftTop.x, 0, 1920);
				ImGui::SliderFloat("texture life top y", &data.mTextureLeftTop.y, 0, 1080);
				ImGui::InputFloat("texture size x", &data.mTextureSize.x, 10);
				ImGui::InputFloat("texture size y", &data.mTextureSize.y, 10);
				ImGui::Separator();
				mUIs[mUINumber]->SetUIData(data);

				ImGui::Text("title data");
				std::vector<TitleUIMove::TitleUIMoveData> titleData = mTitleMove->GetTitleUIMove();
				ImGui::InputFloat("start time", &titleData[mUINumber].startTime, 0.1f);
				ImGui::SliderFloat("start alpha", &titleData[mUINumber].startAlpha, 0, 1);
				ImGui::SliderFloat("end alpha", &titleData[mUINumber].endAlpha, 0, 1);
				ImGui::InputFloat("alpha amount", &titleData[mUINumber].alphaAmount, 0.1f);
				if (ImGui::Button("title move data save"))
				{
					mTitleMove->Save();
				}
				mTitleMove->SetTitleUIMove(titleData);
				if (ImGui::Button("testMove"))
				{
					mTitleMove->TextMove(mUIs);
				}
			}
			else if (mSceneName._Equal("game"))
			{
				ImGui::SliderFloat("texture life top x", &data.mTextureLeftTop.x, 0, 1920);
				ImGui::SliderFloat("texture life top y", &data.mTextureLeftTop.y, 0, 1080);
				ImGui::Separator();
				mUIs[mUINumber]->SetUIData(data);

				ImGui::Text("game data");
				GameUiMove::GameUIData gameData = mGameMove->GetGameUIData();
				ImGui::InputFloat("max time", &gameData.mMaxTime, 1);
				ImGui::InputFloat("max count", &gameData.mMaxCount, 1);
				mGameMove->SetGameUIData(gameData);
				if (ImGui::Button("game move data save"))
				{
					mGameMove->Save();
				}

				if (!mGameMove->GetTestFlag()&&!mGameMove->GetStartFlag())
				{
					mGameMove->SetUI(mUIs);
					if (ImGui::Button("testMove"))
					{
						mGameMove->TestStart();
					}
				}
				else
				{
					if (ImGui::Button("reset"))
					{
						ResetGameUI();
					}
				}
			}
			else
			{
				
				mUIs[mUINumber]->SetUIData(data);
				auto& resultData = mResultMove->GetResultUIData();
				ImGui::SliderFloat("frame alpth", &resultData.frameAlpth, 0, 1);
				ImGui::SliderFloat("alpth difference", &resultData.alpthDifference, 0, 1);
				if (ImGui::Button("save"))
				{
					mResultMove->Save();
				}
				mResultMove->SetResultUIData(resultData);
			}

		}

	}
	if (ImGui::Button("ui data save"))
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
		mTitleMove->Update(elapsdTime, mUIs);
	}
	else if (mSceneName._Equal("game"))
	{
		mGameMove->Update(elapsdTime, mUIs);
	}
	else if (mSceneName._Equal("result"))
	{
		mResultMove->Update(elapsdTime, mUIs);
	}
}

void UIManager::Render(ID3D11DeviceContext* context)
{
	for (auto& ui : mUIs)
	{
		ui->Render(context);
#ifdef USE_IMGUI
		if (!mDebugUIFrameFlag)continue;
		auto& data = ui->GetUIData();
		auto& data2 = mDebugUIFrame->GetUIData();
		data2.mLeftPosition = data.mLeftPosition;
		data2.mDrowSize = data.mDrowSize;
		mDebugUIFrame->SetUIData(data2);
		mDebugUIFrame->Render(context);
#endif
	}
}

void UIManager::Load(const char* scene)
{
	FILE* fp;
	std::string fileName = { "Data/file/" };
	fileName += scene;
	fileName += "UI";
	fileName += ".bin";
	int fileSize = 0;
	if (fopen_s(&fp, fileName.c_str(), "rb") == 0)
	{
		//ファイルサイズの取得
		fseek(fp, 0, SEEK_END);

		fileSize = ftell(fp);
		//ファイルの先頭に戻す
		fseek(fp, 0, SEEK_SET);

		UI::UIData data;
		for (int i=0;i< static_cast<int>(fileSize/sizeof(UI::UIData));i++)
		{
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
		for (auto&ui:mUIs)
		{
			UI::UIData data = ui->GetUIData();
			fwrite(&data, sizeof(UI::UIData), 1, fp);
		}
		fclose(fp);
	}
}
