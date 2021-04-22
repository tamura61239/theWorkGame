#include "ui_manager.h"
#include"gpu_particle_manager.h"
#include"vector_combo.h"
#include"stage_manager.h"
#include"ranking.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include"imgui.h"
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/
/**************************タイトルシーンのUIの初期化***************************/
void UIManager::TitleInitialize(ID3D11Device* device)
{
	//枠の生成
	mDebugUIFrame = std::make_unique<UI>(device, L"Data/image/枠.png", VECTOR2F(500, 500), "");
	//UIの生成
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/タイトル.png", VECTOR2F(1000, 200), "title"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/startKey.png", VECTOR2F(500, 100), "key"));
	//動かすクラスの生成
	mTitleMove = std::make_unique<TitleUIMove>(mUIs.size());
	//ファイルのロード
	mSceneName = "title";
	Load(mSceneName.c_str());
	//名前の取得
	for (int i=0;i< static_cast<int>(mUIs.size());i++)
	{
		auto& ui = mUIs[i];
		
		mNames.push_back(ui->GetName());
		UI::UIData data = ui->GetUIData();
		TitleUIMove::TitleUIMoveData moveData = mTitleMove->GetTitleUIMove()[i];
		data.mColor.w = moveData.startAlpha;
		ui->SetUIData(data);
	}
	//タイトルシーンのパーティクルの生成
	pGpuParticleManager->GetTitleTextureParticle()->LoadTexture(device, L"Data/image/タイトル.png", mUIs[0]->GetUIData().mLeftPosition, mUIs[0]->GetUIData().mDrowSize, mUIs[0]->GetUIData().mTextureLeftTop, mUIs[0]->GetUIData().mTextureSize);
	pGpuParticleManager->GetTitleTextureParticle()->LoadTexture(device, L"Data/image/startKey.png", mUIs[1]->GetUIData().mLeftPosition, mUIs[1]->GetUIData().mDrowSize, mUIs[1]->GetUIData().mTextureLeftTop, mUIs[1]->GetUIData().mTextureSize);
	
	mUINumber = 0;
	mDebugUIFrameFlag = false;
#ifdef USE_IMGUI
	mMoveStartFlag = false;
#else
	mMoveStartFlag = true;
#endif
	mUIMaxCount = mUIs.size();

}
/**************************ゲームシーンのUIの初期化***************************/

void UIManager::GameInitialize(ID3D11Device* device)
{
	//枠の生成
	mDebugUIFrame = std::make_unique<UI>(device, L"Data/image/枠.png", VECTOR2F(500, 500), "");
	//UIの生成
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "count"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time1000"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time100"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time10"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time1"));
	//動かすクラスの生成
	mGameMove = std::make_unique<GameUiMove>();
	//UIの初期化
	mGameMove->SetUI(mUIs);
	//名前の取得
	for (auto& ui : mUIs)
	{
		mNames.push_back(ui->GetName());
	}
	mUINumber = 0;
	//ファイルのロード
	mSceneName = "game";
	Load(mSceneName.c_str());
	mDebugUIFrameFlag = false;
	mUIMaxCount = mUIs.size();

}
/**************************リザルトシーンのUIの初期化***************************/

void UIManager::ResultInitialize(ID3D11Device* device)
{
	//枠の生成
	mDebugUIFrame = std::make_unique<UI>(device, L"Data/image/枠.png", VECTOR2F(500, 500), "");
	//UIの生成
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/○.png", VECTOR2F(800, 800), "frame"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/retry.png", VECTOR2F(300, 80), "retry"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/title.png", VECTOR2F(300, 80), "title"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/stageselect.png", VECTOR2F(300, 80), "stage select"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/nextstage.png", VECTOR2F(300, 80), "next stage"));
	mUIMaxCount = mUIs.size();
	//動かすクラスの生成
	mResultMove = std::make_unique<ResultUIMove>();
	mUINumber = 0;
	//ファイルのロード
	mSceneName = "result";
	Load(mSceneName.c_str());
	mDebugUIFrameFlag = false;
	if (Ranking::GetStageNo() == StageManager::GetMaxStageCount() - 1)mUIMaxCount--;
	//名前の取得
	for (int i=0;i<mUIMaxCount;i++)
	{
		mNames.push_back(mUIs[i]->GetName());
	}

}
/*****************************************************/
//　　　　　　　　　　解放関数
/*****************************************************/
/***************************全部の解放**************************/
void UIManager::Clear()
{
	mUIs.clear();
	mNames.clear();
	mTitleMove.reset(nullptr);
	mGameMove.reset(nullptr);
	mResultMove.reset(nullptr);
}
/***************************UIのみの解放**************************/

void UIManager::ClearUI()
{
	mUIs.clear();
	mNames.clear();
	mUIMaxCount = 0;
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void UIManager::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("ui");
	//フレームを出すかどうかを選択する
	ImGui::Checkbox("debug ui frame", &mDebugUIFrameFlag);
	if (mUIs.size()!=0)
	{
		//UIを選ぶ
		ImGui::Combo("name", &mUINumber, vector_getter, static_cast<void*>(&mNames), mNames.size());
		{
			//選んだUIのパラメーターを操作
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
			{//シーンがタイトルの時
				//パラメーターを操作
				ImGui::SliderFloat("texture life top x", &data.mTextureLeftTop.x, 0, 1920);
				ImGui::SliderFloat("texture life top y", &data.mTextureLeftTop.y, 0, 1080);
				ImGui::InputFloat("texture size x", &data.mTextureSize.x, 10);
				ImGui::InputFloat("texture size y", &data.mTextureSize.y, 10);
				ImGui::Separator();
				//動かしたパラメーターを設定する
				mUIs[mUINumber]->SetUIData(data);
				//UIの動きのパラメーターを操作する
				ImGui::Text("title data");
				std::vector<TitleUIMove::TitleUIMoveData> titleData = mTitleMove->GetTitleUIMove();
				ImGui::InputFloat("start time", &titleData[mUINumber].startTime, 0.1f);
				ImGui::SliderFloat("start alpha", &titleData[mUINumber].startAlpha, 0, 1);
				ImGui::SliderFloat("end alpha", &titleData[mUINumber].endAlpha, 0, 1);
				ImGui::InputFloat("alpha amount", &titleData[mUINumber].alphaAmount, 0.1f);
				//セーブ
				if (ImGui::Button("title move data save"))
				{
					FileFunction::SaveArray(&titleData[0], titleData.size(), "Data/file/titleUIMove.bin", "wb");
				}
				//操作したパラメーターを設定する
				mTitleMove->SetTitleUIMove(titleData);
				//テストプレイ
				if (ImGui::Button("testMove"))
				{
					mTitleMove->TestPlay(mUIs);
				}
			}
			else if (mSceneName._Equal("game"))
			{//シーンがゲームの時
				//パラメーターを操作する
				ImGui::SliderFloat("texture life top x", &data.mTextureLeftTop.x, 0, 1920);
				ImGui::SliderFloat("texture life top y", &data.mTextureLeftTop.y, 0, 1080);
				ImGui::Separator();
				//動かしたパラメーターを設定する
				mUIs[mUINumber]->SetUIData(data);

				//UIの動きのパラメーターを操作する
				ImGui::Text("game data");
				GameUiMove::GameUIData gameData = mGameMove->GetGameUIData();
				ImGui::InputFloat("max time", &gameData.mMaxTime, 1);
				ImGui::InputFloat("max count", &gameData.mMaxCount, 1);
				//操作したパラメーターを設定する
				mGameMove->SetGameUIData(gameData);
				//セーブ
				if (ImGui::Button("game move data save"))
				{
					FileFunction::Save(gameData, "Data/file/gameUIData.bin", "wb");
				}

				if (!mGameMove->GetTestFlag()&&!mGameMove->GetStartFlag())
				{
					//テストプレイ
					mGameMove->SetUI(mUIs);
					if (ImGui::Button("testMove"))
					{
						mGameMove->TestStart();
					}
				}
				else
				{
					//テストプレイ終了
					if (ImGui::Button("reset"))
					{
						ResetGameUI();
					}
				}
			}
			else
			{//シーンがリザルトの時
				//パラメーターを操作する
				mUIs[mUINumber]->SetUIData(data);
				auto& resultData = mResultMove->GetResultUIData();
				ImGui::SliderFloat("frame alpth", &resultData.frameAlpth, 0, 1);
				ImGui::SliderFloat("alpth difference", &resultData.alpthDifference, 0, 1);
				//セーブ
				if (ImGui::Button("save"))
				{
					FileFunction::Save(resultData, "Data/file/resultUIData.bin", "wb");
				}
				//操作したパラメーターを設定する
				mResultMove->SetResultUIData(resultData);
			}

		}

	}
	//UIのデータのセーブ
	if (ImGui::Button("ui data save"))
	{
		Save(mSceneName.c_str());

	}
	ImGui::End();
#endif
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void UIManager::Update(float elapsdTime)
{

	if (mSceneName._Equal("title"))
	{//タイトルのとき
		mTitleMove->Update(elapsdTime, mUIs);
	}
	else if (mSceneName._Equal("game"))
	{//ゲームのとき
		mGameMove->Update(elapsdTime, mUIs);
	}
	else if (mSceneName._Equal("result"))
	{//リザルトのとき
		mResultMove->Update(elapsdTime, mUIs, mUIMaxCount);
	}
}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
void UIManager::Render(ID3D11DeviceContext* context)
{
	for (int i=0;i< mUIMaxCount;i++)
	{
		mUIs[i]->Render(context);
#ifdef USE_IMGUI
		//枠を描画する時
		if (!mDebugUIFrameFlag)continue;
		auto& data = mUIs[i]->GetUIData();
		auto& data2 = mDebugUIFrame->GetUIData();
		data2.mLeftPosition = data.mLeftPosition;
		data2.mDrowSize = data.mDrowSize;
		mDebugUIFrame->SetUIData(data2);
		mDebugUIFrame->Render(context);
#endif
	}
}
/*****************************************************/
//　　　　　　　　　　ファイル操作関数
/*****************************************************/
/***************************ロード**************************/
void UIManager::Load(const char* scene)
{
	FILE* fp;
	std::string fileName = { "Data/file/" };
	fileName += scene;
	fileName += "UI";
	fileName += ".bin";
	std::vector<UI::UIData>datas;
	datas.resize(mUIs.size());
	FileFunction::LoadArray(&datas[0], fileName.c_str(), "rb");
	for (int i = 0; i < datas.size(); i++)
	{
		mUIs[i]->SetUIData(datas[i]);
	}
}
/*********************セーブ**********************/
void UIManager::Save(const char* scene)
{
	FILE* fp;
	std::string fileName = { "Data/file/" };
	fileName += scene;
	fileName += "UI";
	fileName += ".bin";
	std::vector<UI::UIData>datas;
	for (auto& ui : mUIs)
	{
		datas.push_back(ui->GetUIData());
	}
	FileFunction::SaveArray(&datas[0], datas.size(), fileName.c_str(), "wb");
}
