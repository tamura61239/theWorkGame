#include "stage_select.h"
#include"texture.h"
#include"misc.h"
#include"key_board.h"
#include"gpu_particle_manager.h"
#ifdef USE_IMGUI
#include"imgui.h"
#endif

//初期化
StageSelect::StageSelect(ID3D11Device* device, const int maxCount) :mSelectSceneFlag(true), mSelectNumber(0)/*,mMaxStage(4), mTextColor(1,1,1,1)*/, mStageBoardCreateFlag(false), mChangeSelect(0),mMoveTimer(1)
{
	HRESULT hr;
	//ステージフォントのテクスチャ
	mStageTexture = std::make_shared<TextureData>();
	hr = load_texture_from_file(device, L"Data/image/stage.png", mStageTexture->mSRV.GetAddressOf(), &mStageTexture->mDesc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mStageTexture->mTextureDrowSize = VECTOR2F(320, 100);
	//数字フォントのテクスチャ
	mNumberTexture = std::make_shared<TextureData>();
	hr = load_texture_from_file(device, L"Data/image/number.png", mNumberTexture->mSRV.GetAddressOf(), &mNumberTexture->mDesc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mNumberTexture->mTextureDrowSize = VECTOR2F(63, 100);
	mBoardSize = VECTOR2F(200, 500);
	//枠のテクスチャ
	mBackTexture = std::make_shared<TextureData>();
	hr = load_texture_from_file(device, L"Data/image/siro.png", mBackTexture->mSRV.GetAddressOf(), &mBackTexture->mDesc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	for (int i = 0; i < maxCount; i++)
	{
		std::wstring fileName = L"Data/image/stage" + std::to_wstring(i) + L"scne_map.dds";

		mStageImageText.push_back(std::make_shared<TextureData>());
		hr = load_texture_from_file(device, fileName.c_str(), mStageImageText.back()->mSRV.GetAddressOf(), &mStageImageText.back()->mDesc);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		mStageImageText.back()->mTextureDrowSize = VECTOR2F(1920, 1080);
	}

	mDrow = std::make_unique<Sprite>(device);
	//LocalDataの一様の初期化
	mLocalDatas.resize(5);
	std::string name[5] = { "front","number","back" ,"tutorial","stageImage" };
	for (int i = 0; i < static_cast<int>(mLocalDatas.size()); i++)
	{
		mLocalDatas[i].mName = name[i];
		mLocalDatas[i].mData = std::make_shared<LocalData>();
		mLocalDatas[i].mData->mAlpha = 1;
		mLocalDatas[i].mData->mPosition = VECTOR2F(0, 0);
		mLocalDatas[i].mData->mScale = VECTOR2F(1, 1);
	}
	mInterval = 10;
	//データのロード
	Load();
	//枠のテクスチャの描画サイズ
	mBackTexture->mTextureDrowSize = mBoardSize;
	//stageboardの初期化
	for (int i = 0; i < maxCount; i++)
	{
		mStageBoards.push_back(std::make_unique<StageBorad>());
		mStageBoards.back()->CreateText(mBackTexture, mLocalDatas[2].mData, VECTOR2F(0, 0));
		mStageBoards.back()->CreateText(mStageTexture, mLocalDatas[0].mData, VECTOR2F(0, 0));
		VECTOR2F uv;
		uv.y = 0;
		uv.x = mNumberTexture->mTextureDrowSize.x * i;
		mStageBoards.back()->CreateText(mNumberTexture, mLocalDatas[1].mData, uv);
		mStageBoards.back()->CreateText(mStageImageText[i], mLocalDatas[4].mData, VECTOR2F(0,0),VECTOR4F(0.8f,0.8f,0.8f,1));
		mStageBoards.back()->SetSize(mBoardSize);
		mStageBoards.back()->SetStageNo(i);
		mStageBoards.back()->SetInterval(mInterval);
	}
}

//エディタ(ImGui)関数
void StageSelect::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("stage select data");
	ImGui::InputFloat("board size x", &mBoardSize.x, 10);
	ImGui::InputFloat("board size y", &mBoardSize.y, 10);
	ImGui::InputFloat("interval", &mInterval, 1);

	for (auto& stage : mStageBoards)
	{
		stage->SetSize(mBoardSize);
		stage->SetInterval(mInterval);
	}
	mBackTexture->mTextureDrowSize = mBoardSize;
	if (ImGui::CollapsingHeader("localData"))
	{
		for (int i = 0; i < static_cast<int>(mLocalDatas.size()); i++)
		{
			auto& local = mLocalDatas[i].mData;
			std::string defName = std::to_string(i);
			defName += mLocalDatas[i].mName;
			if (ImGui::CollapsingHeader(defName.c_str()))
			{
				float* position[2] = { &local->mPosition.x,&local->mPosition.y };
				std::string name = defName + ":position";
				ImGui::SliderFloat2(name.c_str(), *position, 0, 1);
				name = defName + ":alpha";
				ImGui::SliderFloat(name.c_str(), &local->mAlpha, 0, 1);
				name = defName + ":scale.x";
				ImGui::InputFloat(name.c_str(), &local->mScale.x, 0.1f);
				name = defName + ":scale.y";
				ImGui::InputFloat(name.c_str(), &local->mScale.y, 0.1f);
			}
		}
	}
	ImGui::Text("%d", mSelectNumber);
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::End();
#endif
}

void StageSelect::Update(float elapsdTime, StageManager* manager)
{
	if (!mSelectSceneFlag)return;
	float time = elapsdTime*5.f;
	if (mChangeSelect == 0)//選択するステージを変更してない時
	{
		Select(manager);
	}
	else//選択するステージを変更してる時
	{
		mMoveTimer+= time;
		if (mMoveTimer > 1)
		{
			mMoveTimer = 1;
		}
	}
	//ステージボードの更新
	for (auto& stageBoard : mStageBoards)
	{
		stageBoard->Update(mChangeSelect, mMoveTimer, mSelectNumber, VECTOR2F(1920, 1080) * 0.5f);
	}
	//変更時間が一定値を超えた時
	if (mMoveTimer >= 1&& mChangeSelect!=0)
	{
		if (mChangeSelect > 0)
		{
			mSelectNumber++;
		}
		else
		{
			mSelectNumber--;
		}
		mMoveTimer = 1;

		mChangeSelect = 0;
	}
}
//ステージの選択関数
void StageSelect::Select(StageManager* manager)
{

	if (pKeyBoad.PressedState(KeyLabel::RIGHT)&& mSelectNumber < static_cast<int>(mStageBoards.size()) - 1)//右に移動
	{
		mMoveTimer = 0;
		mChangeSelect = 1;
	}
	if (pKeyBoad.PressedState(KeyLabel::LEFT) && mSelectNumber > 0)//左に移動
	{
		mMoveTimer = 0;
		mChangeSelect =- 1;
	}
	if (pKeyBoad.RisingState(KeyLabel::SPACE))//決定
	{
		manager->SetStageNo(mSelectNumber);
		mSelectSceneFlag = false;
	}
}

//描画
void StageSelect::Render(ID3D11DeviceContext* context)
{
	for (auto& stageBoard : mStageBoards)
	{
		stageBoard->Render(context, mDrow.get());
	}
}

void StageSelect::Load()
{
	FILE* fp;
	int fileSize = 0;

	if (fopen_s(&fp, "Data/file/stage_select_scene.bin", "rb") == 0)
	{
		//ファイルサイズの取得
		fseek(fp, 0, SEEK_END);

		fileSize = ftell(fp);
		//ファイルの先頭に戻す
		fseek(fp, 0, SEEK_SET);

		fread(&mBoardSize, sizeof(VECTOR2F), 1, fp);
		fread(&mInterval, sizeof(float), 1, fp);
		fileSize -= sizeof(VECTOR2F) + sizeof(float);
		for (int i = 0; i < static_cast<int>(fileSize/sizeof(LocalData)); i++)
		{
			LocalData data;
			fread(&data, sizeof(LocalData), 1, fp);
			mLocalDatas[i].mData->mAlpha = data.mAlpha;
			mLocalDatas[i].mData->mPosition = data.mPosition;
			mLocalDatas[i].mData->mScale = data.mScale;
		}

		fclose(fp);
	}
}
//ファイル書き込み関数
void StageSelect::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/stage_select_scene.bin", "wb");
	{
		fwrite(&mBoardSize, sizeof(VECTOR2F), 1, fp);
		fwrite(&mInterval, sizeof(float), 1, fp);
		for (int i = 0; i < static_cast<int>(mLocalDatas.size()); i++)
		{

			fwrite(mLocalDatas[i].mData.get(), sizeof(LocalData), 1, fp);
		}
		fclose(fp);
	}

}
