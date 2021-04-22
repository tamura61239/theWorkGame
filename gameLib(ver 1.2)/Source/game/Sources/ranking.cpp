#include "ranking.h"
#include"ui_manager.h"
#include"gpu_particle_manager.h"
#include <cassert>
#include <algorithm>
#include <functional>
#ifdef USE_IMGUI
#include<imgui.h>
#endif
//                        Time用のテクスチャ

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

TimeText::TimeText(float time) :mTime(static_cast<int>(time * 100)), mTextSize(0, 0), mLeftTop(0, 0), mSizeX(0)
{
}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
void TimeText::Render(ID3D11DeviceContext* context, Sprite* sprite)
{
	//10の位
	int number = mTime / 1000;
	int time = mTime % 1000;
	VECTOR2F leftTop = mLeftTop + VECTOR2F(mTextSize.x, 0) * 0;
	VECTOR2F size = VECTOR2F(mSizeX, mTextSize.y);
	VECTOR2F texLeftTop = VECTOR2F(62.f * static_cast<float>(number), 0);
	sprite->Render(context, leftTop, size, texLeftTop, VECTOR2F(62, 100), 0);
	//1の位
	number = time / 100;
	time %= 100;
	leftTop = mLeftTop + VECTOR2F(mTextSize.x, 0) * 0.2f;
	texLeftTop = VECTOR2F(62.f * static_cast<float>(number), 0);
	sprite->Render(context, leftTop, size, texLeftTop, VECTOR2F(62, 100), 0);
	//0.1の位
	number = time / 10;
	time %= 10;
	leftTop = mLeftTop + VECTOR2F(mTextSize.x, 0) * 0.6f;
	texLeftTop = VECTOR2F(62.f * static_cast<float>(number), 0);
	sprite->Render(context, leftTop, size, texLeftTop, VECTOR2F(62, 100), 0);
	//0.01の位
	number = time;
	leftTop = mLeftTop + VECTOR2F(mTextSize.x, 0) * 0.8f;
	texLeftTop = VECTOR2F(62.f * static_cast<float>(number), 0);
	sprite->Render(context, leftTop, size, texLeftTop, VECTOR2F(62, 100), 0);

}
//                                 ランキング
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

int Ranking::mStageNo = 0;
Ranking::Ranking(ID3D11Device* device, float time) :mState(0), mNowPlayTime(time), mTimer(0), mTestFlag(false), mNowPlayRank(0), mNewPlayMove(0), mNoRankTextColor(1,1,1,1)
{
	mNowPlayTimeText = std::make_unique<TimeText>(0.f);
	for (int i = 0; i < 5; i++)
	{
		mRankingTexts.push_back(std::make_unique<TimeText>(0.f));
	}
	mNumberTest = std::make_unique<Sprite>(device, L"Data/image/number.png");
	mRankTest = std::make_unique<Sprite>(device, L"Data/image/rank.png");
	Load();
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void Ranking::Update(float elapsdTime, bool play)
{
	if (!mTestFlag && !play)
	{
		mTimer = 0;
		mState = 0;
		return;
	}
	mTimer += elapsdTime;
	switch (mState)
	{
	case 0://今回のタイムの表示
		ScereMove(elapsdTime);
		break;
	case 1://過去のランキングデータを表示
		RankingMove(elapsdTime);
		break;
	case 2://ランキングデータをの更新
		NewRankingMove(elapsdTime);
		break;
	}
}
/*******************************1秒間に0から今回のスコア(タイム)まで増やす**************************/
void Ranking::ScereMove(float elapsdTime)
{
	float time = mTimer;
	if (time > 1)
	{
		time = time - (time - 1);
		mState++;
		RankingTimeSort();
	}
	//時間に合わせた値を渡す
	mNowPlayTimeText->SetTime(time * mNowPlayTime);
}
/**********************************ランキングのスコア(タイム)を順に表示*****************************/
void Ranking::RankingMove(float elapsdTime)
{
	for (int i = 0; i < static_cast<int>(mRankingTexts.size()); i++)
	{
		auto& text = mRankingTexts.at(i);
		if (mRankingData.mStartTime * i + 1.f <= mTimer)
		{
			//スライドしながら過去のランキングデータが順番に出てくる
			VECTOR2F leftTop = VECTOR2F(0, mRankingData.mLeftTop.y + mRankingData.mIntervalY * i);
			VECTOR2F start = leftTop + VECTOR2F(mRankingData.mStartX, 0);
			VECTOR2F end = leftTop + VECTOR2F(mRankingData.mLeftTop.x, 0);
			float time = mTimer - (mRankingData.mStartTime * i + 1.f);
			time = std::min(time / mRankingData.mMoveTime, 1.f);
			DirectX::XMStoreFloat2(&leftTop, DirectX::XMVectorLerp(DirectX::XMLoadFloat2(&start), DirectX::XMLoadFloat2(&end), time));
			text->SetLeftTop(leftTop);
			if (mRankingTexts.size() * mRankingData.mStartTime + 1.f + mRankingData.mMoveTime <= mTimer)
			{
				mState++;
				mTimer = 0;
				mNewPlayMove = 0;
				pGpuParticleManager->GetFireworksParticle()->CreateEmitor(mNowPlayRank);
			}
		}

	}
}
/*********************************今回のスコア(タイム)をランキングに組み込む処理*************************/
void Ranking::NewRankingMove(float elapsdTime)
{
	if (mNewPlayMove == 0)
	{
		float time = std::min(mTimer / mRankingData.mMoveTime, 1.f);
		if (mNowPlayRank < 5)
		{//今回のタイムが5位より上なら
			for (int i = mNowPlayRank; i < 5; i++)
			{
				auto& text = mRankingTexts.at(i);
				VECTOR2F leftTop = VECTOR2F(0, 0),
					start = VECTOR2F(mRankingData.mLeftTop.x, mRankingData.mLeftTop.y + mRankingData.mIntervalY * i),
					end = VECTOR2F(mRankingData.mLeftTop.x, mRankingData.mLeftTop.y + mRankingData.mIntervalY * (i + 1));
				DirectX::XMStoreFloat2(&leftTop, DirectX::XMVectorLerp(DirectX::XMLoadFloat2(&start), DirectX::XMLoadFloat2(&end), time));
				text->SetLeftTop(leftTop);
			}
		}
		if (mTimer >= mRankingData.mMoveTime)
		{//新しいランキング表示用のテキストを作る
			mNewPlayMove++;
			mRankingTexts.push_back(std::make_unique<TimeText>(mNowPlayTime));
			mRankingTexts.back()->SetTextSize(mRankingData.mTextSize);
			mTimer = mRankingData.mMoveTime;
			VECTOR2F leftTop = VECTOR2F(mRankingData.mStartX, mRankingData.mLeftTop.y + mRankingData.mIntervalY * mNowPlayRank);
			mRankingTexts.back()->SetLeftTop(leftTop);
		}
	}
	else
	{
		//新しいテキストをスライドさせる
		float time = std::min((mTimer - mRankingData.mMoveTime) / mRankingData.mMoveTime, 1.f);

		VECTOR2F leftTop = VECTOR2F(0, 0),
			start = VECTOR2F(mRankingData.mStartX, mRankingData.mLeftTop.y + mRankingData.mIntervalY * mNowPlayRank),
			end = VECTOR2F(mRankingData.mLeftTop.x, mRankingData.mLeftTop.y + mRankingData.mIntervalY * mNowPlayRank);
		DirectX::XMStoreFloat2(&leftTop, DirectX::XMVectorLerp(DirectX::XMLoadFloat2(&start), DirectX::XMLoadFloat2(&end), time));
		mRankingTexts.back()->SetLeftTop(leftTop);
		if (time >= 1.f)
		{//次のシーンを選択できるようになる&UIが表示されるようになる
			if (!mTestFlag)UIManager::GetInctance()->GetResultUIMove()->SetMoveFlag(true);
			mState++;
		}
	}
}

/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
void Ranking::Render(ID3D11DeviceContext* context)
{
	mNowPlayTimeText->Render(context, mNumberTest.get());
	for (auto& text : mRankingTexts)
	{
		text->Render(context, mNumberTest.get());
	}
	for (int i = 0; i < 5; i++)
	{
		VECTOR2F leftTop = VECTOR2F(mRankingData.mLeftTop.x-100.f, mRankingData.mLeftTop.y + mRankingData.mIntervalY * i-20);
		mNumberTest->Render(context, leftTop, VECTOR2F(62, 100), VECTOR2F(62 * static_cast<float>((i + 1)), 0), VECTOR2F(62, 100), 0);
	}
	VECTOR2F leftTop = VECTOR2F(mRankingData.mLeftTop.x - 370, mRankingData.mLeftTop.y + mRankingData.mIntervalY * 5 - 20);

	mRankTest->Render(context,leftTop,VECTOR2F(330,100.f),VECTOR2F(0,0),VECTOR2F(330,100),0, mNoRankTextColor);
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void Ranking::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("rank text");
	ImGui::Checkbox("testFlag", &mTestFlag);
	//ランキングテキストの色を決める
	float* color[4] = { &mNoRankTextColor.x,&mNoRankTextColor.y,&mNoRankTextColor.z,&mNoRankTextColor.w };
	ImGui::ColorEdit4("no rank text color", *color);
	if (!mTestFlag&& mState<3)
	{//テストプレイ中でなくまだ進行してない時
		UIManager::GetInctance()->GetResultUIMove()->SetMoveFlag(false);
		mRankingTexts.erase(mRankingTexts.begin() + 5);

	}
	if (ImGui::CollapsingHeader("now play data"))
	{//今回のタイムを表示するテキストのパラメーターを調整する
		float*leftTop[2] = { &mNowPlayScoreTime.mLeftTop.x,&mNowPlayScoreTime.mLeftTop.y };
		ImGui::DragFloat2("now play left top", *leftTop);
		mNowPlayTimeText->SetLeftTop(mNowPlayScoreTime.mLeftTop);
		float*textSize[2] = { &mNowPlayScoreTime.mTextSize.x,&mNowPlayScoreTime.mTextSize.y };
		ImGui::DragFloat2("now play text size", *textSize);
		mNowPlayTimeText->SetTextSize(mNowPlayScoreTime.mTextSize);
	}
	if (ImGui::CollapsingHeader("ranking data"))
	{//ランキングの表示用テキストのパラメーターを調整する
		float* leftTop[2] = { &mRankingData.mLeftTop.x,&mRankingData.mLeftTop.y };
		ImGui::DragFloat2("ranking left top", *leftTop);
		float* textSize[2] = { &mRankingData.mTextSize.x,&mRankingData.mTextSize.y };
		ImGui::DragFloat2("ranking text size", *textSize);
		ImGui::InputFloat("start time", &mRankingData.mStartTime, 0.1f);
		ImGui::InputFloat("move time", &mRankingData.mMoveTime, 0.1f);
		ImGui::InputFloat("start x", &mRankingData.mStartX, 10);
		ImGui::InputFloat("interval y", &mRankingData.mIntervalY, 10);
		if(!mTestFlag)SetRankingData();
	}
	//セーブ
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::End();
#endif
}
/*****************************************************/
//　　　　　　　　　　ソート関数
/*****************************************************/
/**************************ランキングのソート**************************/
void Ranking::RankingTimeSort()
{
	int time[6] = { 0 };
	//今回の時間をint型にする(小数点第二位までにする)
	int nowPlayTime = static_cast<int>(mNowPlayTime * 100);
	//今回のタイムを5番目に配置する
	time[5] = nowPlayTime;
	//過去のランキングデータを取得
	std::string fileName = "Data/file/ranking" + std::to_string(mStageNo) + ".txt";
	FILE* fp;
	if (fopen_s(&fp, fileName.c_str(), "rb") == 0)
	{
		fscanf_s(fp, "%d,%d,%d,%d,%d", &time[0], &time[1], &time[2], &time[3], &time[4]);
		//fread(&time[0], sizeof(int), 5, fp);
		fclose(fp);
	}
	//クイックソートする
	QuickSort(time, 0, 5);
	//今回のタイムが何位か調べる
	for (int i = 0; i < 6; i++)
	{
		if (time[i] == nowPlayTime)
		{
			mNowPlayRank = i;
			break;
		}
		if (i == 5)
		{
			mNowPlayRank = i;
		}
	}
	//描画するテキストにタイムを渡す
	int count = 0;
	for (int i = 0; i < 6; i++)
	{
		if (i == mNowPlayRank)continue;
		auto& text = mRankingTexts[count];
		text->SetTime(static_cast<float>(time[i])*0.01f);
		count++;
	}
	//ランキングをセーブする
	fopen_s(&fp, fileName.c_str(), "wb");
	//fwrite(&time[0], sizeof(int), 5, fp);
	fprintf_s(fp, "%d,%d,%d,%d,%d", time[0], time[1], time[2], time[3], time[4]);
	fclose(fp);

}
/******************クイックソート**********************/

void Ranking::QuickSort(int array[], int left, int right)
{
	int pivot;

	if (left < right) {
		pivot = Partition(array, left, right);
		QuickSort(array, left, pivot - 1);   // pivotを境に再帰的にクイックソート
		QuickSort(array, pivot + 1, right);
	}
}
/*************************条件を満たしたものを並び替える*********************/
int Ranking::Partition(int array[], int left, int right)
{
	int i, j, pivot;
	i = left;
	j = right + 1;
	pivot = left;   // 先頭要素をpivotとする

	do {
		do { i++; } while (array[i] > array[pivot]);
		do { j--; } while (array[pivot] > array[j]);
		// pivotより小さいものを左へ、大きいものを右へ
		if (i < j) { Swap(&array[i], &array[j]); }
	} while (i < j);

	Swap(&array[pivot], &array[j]);   //pivotを更新

	return j;
}
/***********************二つの値を交換する*********************/
void Ranking::Swap(int* x, int* y)
{
	int temp;    // 値を一時保存する変数

	temp = *x;
	*x = *y;
	*y = temp;
}
/*****************************************************/
//　　　　　　　　　　setter関数
/*****************************************************/
void Ranking::SetRankingData()
{
	mNowPlayTimeText->SetLeftTop(mNowPlayScoreTime.mLeftTop);
	mNowPlayTimeText->SetTextSize(mNowPlayScoreTime.mTextSize);
	for (int i = 0; i < static_cast<int>(mRankingTexts.size()); i++)
	{
		auto& text = mRankingTexts.at(i);
		VECTOR2F leftTop = VECTOR2F(mRankingData.mStartX, mRankingData.mLeftTop.y);
		leftTop.y += mRankingData.mIntervalY * i;
		text->SetLeftTop(leftTop);
		text->SetTextSize(mRankingData.mTextSize);
	}
}

/*****************************************************/
//　　　　　　　　　　ファイル関数
/*****************************************************/
/**************************ロード***************************/
void Ranking::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/rankingData.bin", "rb") == 0)
	{
		fread(&mNowPlayScoreTime, sizeof(ScoreData), 1, fp);
		fread(&mRankingData, sizeof(RankingData), 1, fp);
		fread(&mNoRankTextColor, sizeof(VECTOR4F), 1, fp);
		fclose(fp);
	}
	else
	{
		::memset(&mNowPlayScoreTime, 0, sizeof(ScoreData));
		::memset(&mRankingData, 0, sizeof(RankingData));
	}
	SetRankingData();
}
/**************************セーブ	**************************/
void Ranking::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/rankingData.bin", "wb");
	fwrite(&mNowPlayScoreTime, sizeof(ScoreData), 1, fp);
	fwrite(&mRankingData, sizeof(RankingData), 1, fp);
	fwrite(&mNoRankTextColor, sizeof(VECTOR4F), 1, fp);
	fclose(fp);

}
