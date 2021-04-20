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
//�@�@�@�@�@�@�@�@�@�@�������֐�
/*****************************************************/
/**************************�^�C�g���V�[����UI�̏�����***************************/
void UIManager::TitleInitialize(ID3D11Device* device)
{
	//�g�̐���
	mDebugUIFrame = std::make_unique<UI>(device, L"Data/image/�g.png", VECTOR2F(500, 500), "");
	//UI�̐���
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/�^�C�g��.png", VECTOR2F(1000, 200), "title"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/startKey.png", VECTOR2F(500, 100), "key"));
	//�������N���X�̐���
	mTitleMove = std::make_unique<TitleUIMove>(mUIs.size());
	//�t�@�C���̃��[�h
	mSceneName = "title";
	Load(mSceneName.c_str());
	//���O�̎擾
	for (int i=0;i< static_cast<int>(mUIs.size());i++)
	{
		auto& ui = mUIs[i];
		
		mNames.push_back(ui->GetName());
		UI::UIData data = ui->GetUIData();
		TitleUIMove::TitleUIMoveData moveData = mTitleMove->GetTitleUIMove()[i];
		data.mColor.w = moveData.startAlpha;
		ui->SetUIData(data);
	}
	//�^�C�g���V�[���̃p�[�e�B�N���̐���
	pGpuParticleManager->GetTitleTextureParticle()->LoadTexture(device, L"Data/image/�^�C�g��.png", mUIs[0]->GetUIData().mLeftPosition, mUIs[0]->GetUIData().mDrowSize, mUIs[0]->GetUIData().mTextureLeftTop, mUIs[0]->GetUIData().mTextureSize);
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
/**************************�Q�[���V�[����UI�̏�����***************************/

void UIManager::GameInitialize(ID3D11Device* device)
{
	//�g�̐���
	mDebugUIFrame = std::make_unique<UI>(device, L"Data/image/�g.png", VECTOR2F(500, 500), "");
	//UI�̐���
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "count"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time1000"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time100"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time10"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/number.png", VECTOR2F(62.f, 100), "time1"));
	//�������N���X�̐���
	mGameMove = std::make_unique<GameUiMove>();
	//UI�̏�����
	mGameMove->SetUI(mUIs);
	//���O�̎擾
	for (auto& ui : mUIs)
	{
		mNames.push_back(ui->GetName());
	}
	mUINumber = 0;
	//�t�@�C���̃��[�h
	mSceneName = "game";
	Load(mSceneName.c_str());
	mDebugUIFrameFlag = false;
	mUIMaxCount = mUIs.size();

}
/**************************���U���g�V�[����UI�̏�����***************************/

void UIManager::ResultInitialize(ID3D11Device* device)
{
	//�g�̐���
	mDebugUIFrame = std::make_unique<UI>(device, L"Data/image/�g.png", VECTOR2F(500, 500), "");
	//UI�̐���
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/��.png", VECTOR2F(800, 800), "frame"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/retry.png", VECTOR2F(300, 80), "retry"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/title.png", VECTOR2F(300, 80), "title"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/stageselect.png", VECTOR2F(300, 80), "stage select"));
	mUIs.push_back(std::make_shared<UI>(device, L"Data/image/nextstage.png", VECTOR2F(300, 80), "next stage"));
	mUIMaxCount = mUIs.size();
	//�������N���X�̐���
	mResultMove = std::make_unique<ResultUIMove>();
	mUINumber = 0;
	//�t�@�C���̃��[�h
	mSceneName = "result";
	Load(mSceneName.c_str());
	mDebugUIFrameFlag = false;
	if (Ranking::GetStageNo() == StageManager::GetMaxStageCount() - 1)mUIMaxCount--;
	//���O�̎擾
	for (int i=0;i<mUIMaxCount;i++)
	{
		mNames.push_back(mUIs[i]->GetName());
	}

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@����֐�
/*****************************************************/
/***************************�S���̉��**************************/
void UIManager::Clear()
{
	mUIs.clear();
	mNames.clear();
	mTitleMove.reset(nullptr);
	mGameMove.reset(nullptr);
	mResultMove.reset(nullptr);
}
/***************************UI�݂̂̉��**************************/

void UIManager::ClearUI()
{
	mUIs.clear();
	mNames.clear();
	mUIMaxCount = 0;
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void UIManager::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("ui");
	//�t���[�����o�����ǂ�����I������
	ImGui::Checkbox("debug ui frame", &mDebugUIFrameFlag);
	if (mUIs.size()!=0)
	{
		//UI��I��
		ImGui::Combo("name", &mUINumber, vector_getter, static_cast<void*>(&mNames), mNames.size());
		{
			//�I��UI�̃p�����[�^�[�𑀍�
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
			{//�V�[�����^�C�g���̎�
				//�p�����[�^�[�𑀍�
				ImGui::SliderFloat("texture life top x", &data.mTextureLeftTop.x, 0, 1920);
				ImGui::SliderFloat("texture life top y", &data.mTextureLeftTop.y, 0, 1080);
				ImGui::InputFloat("texture size x", &data.mTextureSize.x, 10);
				ImGui::InputFloat("texture size y", &data.mTextureSize.y, 10);
				ImGui::Separator();
				//���������p�����[�^�[��ݒ肷��
				mUIs[mUINumber]->SetUIData(data);
				//UI�̓����̃p�����[�^�[�𑀍삷��
				ImGui::Text("title data");
				std::vector<TitleUIMove::TitleUIMoveData> titleData = mTitleMove->GetTitleUIMove();
				ImGui::InputFloat("start time", &titleData[mUINumber].startTime, 0.1f);
				ImGui::SliderFloat("start alpha", &titleData[mUINumber].startAlpha, 0, 1);
				ImGui::SliderFloat("end alpha", &titleData[mUINumber].endAlpha, 0, 1);
				ImGui::InputFloat("alpha amount", &titleData[mUINumber].alphaAmount, 0.1f);
				//�Z�[�u
				if (ImGui::Button("title move data save"))
				{
					FileFunction::SaveArray(&titleData[0], titleData.size(), "Data/file/titleUIMove.bin", "wb");
				}
				//���삵���p�����[�^�[��ݒ肷��
				mTitleMove->SetTitleUIMove(titleData);
				//�e�X�g�v���C
				if (ImGui::Button("testMove"))
				{
					mTitleMove->TestPlay(mUIs);
				}
			}
			else if (mSceneName._Equal("game"))
			{//�V�[�����Q�[���̎�
				//�p�����[�^�[�𑀍삷��
				ImGui::SliderFloat("texture life top x", &data.mTextureLeftTop.x, 0, 1920);
				ImGui::SliderFloat("texture life top y", &data.mTextureLeftTop.y, 0, 1080);
				ImGui::Separator();
				//���������p�����[�^�[��ݒ肷��
				mUIs[mUINumber]->SetUIData(data);

				//UI�̓����̃p�����[�^�[�𑀍삷��
				ImGui::Text("game data");
				GameUiMove::GameUIData gameData = mGameMove->GetGameUIData();
				ImGui::InputFloat("max time", &gameData.mMaxTime, 1);
				ImGui::InputFloat("max count", &gameData.mMaxCount, 1);
				//���삵���p�����[�^�[��ݒ肷��
				mGameMove->SetGameUIData(gameData);
				//�Z�[�u
				if (ImGui::Button("game move data save"))
				{
					FileFunction::Save(gameData, "Data/file/gameUIData.bin", "wb");
				}

				if (!mGameMove->GetTestFlag()&&!mGameMove->GetStartFlag())
				{
					//�e�X�g�v���C
					mGameMove->SetUI(mUIs);
					if (ImGui::Button("testMove"))
					{
						mGameMove->TestStart();
					}
				}
				else
				{
					//�e�X�g�v���C�I��
					if (ImGui::Button("reset"))
					{
						ResetGameUI();
					}
				}
			}
			else
			{//�V�[�������U���g�̎�
				//�p�����[�^�[�𑀍삷��
				mUIs[mUINumber]->SetUIData(data);
				auto& resultData = mResultMove->GetResultUIData();
				ImGui::SliderFloat("frame alpth", &resultData.frameAlpth, 0, 1);
				ImGui::SliderFloat("alpth difference", &resultData.alpthDifference, 0, 1);
				//�Z�[�u
				if (ImGui::Button("save"))
				{
					FileFunction::Save(resultData, "Data/file/resultUIData.bin", "wb");
				}
				//���삵���p�����[�^�[��ݒ肷��
				mResultMove->SetResultUIData(resultData);
			}

		}

	}
	//UI�̃f�[�^�̃Z�[�u
	if (ImGui::Button("ui data save"))
	{
		Save(mSceneName.c_str());

	}
	ImGui::End();
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
void UIManager::Update(float elapsdTime)
{

	if (mSceneName._Equal("title"))
	{//�^�C�g���̂Ƃ�
		mTitleMove->Update(elapsdTime, mUIs);
	}
	else if (mSceneName._Equal("game"))
	{//�Q�[���̂Ƃ�
		mGameMove->Update(elapsdTime, mUIs);
	}
	else if (mSceneName._Equal("result"))
	{//���U���g�̂Ƃ�
		mResultMove->Update(elapsdTime, mUIs, mUIMaxCount);
	}
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
void UIManager::Render(ID3D11DeviceContext* context)
{
	for (int i=0;i< mUIMaxCount;i++)
	{
		mUIs[i]->Render(context);
#ifdef USE_IMGUI
		//�g��`�悷�鎞
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
//�@�@�@�@�@�@�@�@�@�@�t�@�C������֐�
/*****************************************************/
/***************************���[�h**************************/
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
/*********************�Z�[�u**********************/
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
