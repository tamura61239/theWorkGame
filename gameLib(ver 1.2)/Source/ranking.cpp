#include "ranking.h"
#include"ui_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/**********************Time�p�̃e�N�X�`��****************************/
TimeText::TimeText(float time) :mTime(static_cast<int>(time * 100)), mTextSize(0, 0), mLeftTop(0, 0), mSizeX(0)
{
}
//�`��
void TimeText::Render(ID3D11DeviceContext* context, Sprite* sprite)
{
	int number = mTime / 1000;
	int time = mTime % 1000;
	VECTOR2F leftTop = mLeftTop + VECTOR2F(mTextSize.x, 0) * 0;
	VECTOR2F size = VECTOR2F(mSizeX, mTextSize.y);
	VECTOR2F texLeftTop = VECTOR2F(62 * number, 0);
	sprite->Render(context, leftTop, size, texLeftTop, VECTOR2F(62, 100), 0);
	number = time / 100;
	time %= 100;
	leftTop = mLeftTop + VECTOR2F(mTextSize.x, 0) * 0.2f;
	texLeftTop = VECTOR2F(62 * number, 0);
	sprite->Render(context, leftTop, size, texLeftTop, VECTOR2F(62, 100), 0);
	number = time / 10;
	time %= 10;
	leftTop = mLeftTop + VECTOR2F(mTextSize.x, 0) * 0.6f;
	texLeftTop = VECTOR2F(62 * number, 0);
	sprite->Render(context, leftTop, size, texLeftTop, VECTOR2F(62, 100), 0);
	number = time;
	leftTop = mLeftTop + VECTOR2F(mTextSize.x, 0) * 0.8f;
	texLeftTop = VECTOR2F(62 * number, 0);
	sprite->Render(context, leftTop, size, texLeftTop, VECTOR2F(62, 100), 0);

}
/****************************�����L���O*****************************/
int Ranking::mStageNo = 0;
Ranking::Ranking(ID3D11Device* device, float time) :mState(0), mNowPlayTime(time), mTimer(0), mTestFlag(false), mNowPlayRank(0), mNewPlayMove(0), mNoRankTextColor(1,1,1,1)
{
	mNowPlayTimeText = std::make_unique<TimeText>(0);
	for (int i = 0; i < 5; i++)
	{
		mRankingTexts.push_back(std::make_unique<TimeText>(0));
	}
	mNumberTest = std::make_unique<Sprite>(device, L"Data/image/number.png");
	mRankTest = std::make_unique<Sprite>(device, L"Data/image/rank.png");
	Load();
}
//�X�V
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
	case 0:
		ScereMove(elapsdTime);
		break;
	case 1:
		RankingMove(elapsdTime);
		break;
	case 2:
		NewRankingMove(elapsdTime);
		break;
	}
}
//�`��
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
		mNumberTest->Render(context, leftTop, VECTOR2F(62, 100), VECTOR2F(62 * (i + 1), 0), VECTOR2F(62, 100), 0);
	}
	VECTOR2F leftTop = VECTOR2F(mRankingData.mLeftTop.x - 370, mRankingData.mLeftTop.y + mRankingData.mIntervalY * 5 - 20);

	mRankTest->Render(context,leftTop,VECTOR2F(330,100.f),VECTOR2F(0,0),VECTOR2F(330,100),0, mNoRankTextColor);
}
//1�b�Ԃ�0���獡��̃X�R�A(�^�C��)�܂ő��₷
void Ranking::ScereMove(float elapsdTime)
{
	float time = mTimer;
	if (time > 1)
	{
		time = time - (time - 1);
		mState++;
		RankingTimeSort();
	}
	mNowPlayTimeText->SetTime(time * mNowPlayTime);
}
//�����L���O�̃X�R�A(�^�C��)�����ɕ\��
void Ranking::RankingMove(float elapsdTime)
{
	for (int i = 0; i < mRankingTexts.size(); i++)
	{
		auto& text = mRankingTexts.at(i);
		if (mRankingData.mStartTime * i + 1.f <= mTimer)
		{
			VECTOR2F leftTop = VECTOR2F(0, mRankingData.mLeftTop.y+mRankingData.mIntervalY * i);
			VECTOR2F start = leftTop + VECTOR2F(mRankingData.mStartX, 0);
			VECTOR2F end = leftTop + VECTOR2F(mRankingData.mLeftTop.x, 0);
			float time = mTimer - (mRankingData.mStartTime * i + 1.f);
			time = min(time / mRankingData.mMoveTime, 1.f);
			DirectX::XMStoreFloat2(&leftTop, DirectX::XMVectorLerp(DirectX::XMLoadFloat2(&start), DirectX::XMLoadFloat2(&end), time));
			text->SetLeftTop(leftTop);
			if (mRankingTexts.size() * mRankingData.mStartTime + 1.f + mRankingData.mMoveTime <= mTimer)
			{
				mState++;
				mTimer = 0;
				mNewPlayMove = 0;
			}
		}

	}
}
//����̃X�R�A(�^�C��)�������L���O�ɑg�ݍ��ޏ���
void Ranking::NewRankingMove(float elapsdTime)
{
	if (mNewPlayMove == 0)
	{
		float time = min(mTimer / mRankingData.mMoveTime, 1.f);
		if (mNowPlayRank < 5)
		{
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
		{
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
		float time = min((mTimer- mRankingData.mMoveTime) / mRankingData.mMoveTime, 1.f);

		VECTOR2F leftTop = VECTOR2F(0, 0),
			start = VECTOR2F(mRankingData.mStartX, mRankingData.mLeftTop.y + mRankingData.mIntervalY * mNowPlayRank),
			end = VECTOR2F(mRankingData.mLeftTop.x, mRankingData.mLeftTop.y + mRankingData.mIntervalY * mNowPlayRank);
		DirectX::XMStoreFloat2(&leftTop, DirectX::XMVectorLerp(DirectX::XMLoadFloat2(&start), DirectX::XMLoadFloat2(&end), time));
		mRankingTexts.back()->SetLeftTop(leftTop);
		if (time >= 1.f)
		{
			if(!mTestFlag)UIManager::GetInctance().GetResultUIMove()->SetMoveFlag(true);
			mState++;
		}
	}
}
//editor�֐�
void Ranking::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("rank text");
	ImGui::Checkbox("testFlag", &mTestFlag);
	float* color[4] = { &mNoRankTextColor.x,&mNoRankTextColor.y,&mNoRankTextColor.z,&mNoRankTextColor.w };
	ImGui::ColorEdit4("no rank text color", *color);
	if (!mTestFlag&& mState<3)
	{
		UIManager::GetInctance().GetResultUIMove()->SetMoveFlag(false);
		mRankingTexts.erase(mRankingTexts.begin() + 5);

	}
	if (ImGui::CollapsingHeader("now play data"))
	{
		float*leftTop[2] = { &mNowPlayScoreTime.mLeftTop.x,&mNowPlayScoreTime.mLeftTop.y };
		ImGui::DragFloat2("now play left top", *leftTop);
		mNowPlayTimeText->SetLeftTop(mNowPlayScoreTime.mLeftTop);
		float*textSize[2] = { &mNowPlayScoreTime.mTextSize.x,&mNowPlayScoreTime.mTextSize.y };
		ImGui::DragFloat2("now play text size", *textSize);
		mNowPlayTimeText->SetTextSize(mNowPlayScoreTime.mTextSize);
	}
	if (ImGui::CollapsingHeader("ranking data"))
	{
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
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::End();
#endif
}
//�����L���O�̃\�[�g
void Ranking::RankingTimeSort()
{
	int time[6] = { 0 };
	int nowPlayTime = static_cast<int>(mNowPlayTime * 100);
	time[5] = nowPlayTime;
	std::string fileName = "Data/file/ranking" + std::to_string(mStageNo) + ".txt";
	FILE* fp;
	if (fopen_s(&fp, fileName.c_str(), "rt") == 0)
	{
		fread(&time[0], sizeof(int), 5, fp);
		fclose(fp);
	}
	QuickSort(time, 0, 5);
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
	int count = 0;
	for (int i = 0; i < 6; i++)
	{
		if (i == mNowPlayRank)continue;
		auto& text = mRankingTexts[count];
		text->SetTime(static_cast<float>(time[i])*0.01f);
		count++;
	}
	fopen_s(&fp, fileName.c_str(), "wt");
	fwrite(&time[0], sizeof(int), 5, fp);
	fclose(fp);

}
/******************�\�[�g�֐�**********************/
void Ranking::SetRankingData()
{
	mNowPlayTimeText->SetLeftTop(mNowPlayScoreTime.mLeftTop);
	mNowPlayTimeText->SetTextSize(mNowPlayScoreTime.mTextSize);
	for (int i = 0; i < mRankingTexts.size(); i++)
	{
		auto& text = mRankingTexts.at(i);
		VECTOR2F leftTop = VECTOR2F(mRankingData.mStartX, mRankingData.mLeftTop.y);
		leftTop.y += mRankingData.mIntervalY * i;
		text->SetLeftTop(leftTop);
		text->SetTextSize(mRankingData.mTextSize);
	}
}

void Ranking::QuickSort(int array[], int left, int right)
{
	int pivot;

	if (left < right) {
		pivot = Partition(array, left, right);
		QuickSort(array, left, pivot - 1);   // pivot�����ɍċA�I�ɃN�C�b�N�\�[�g
		QuickSort(array, pivot + 1, right);
	}
}

int Ranking::Partition(int array[], int left, int right)
{
	int i, j, pivot;
	i = left;
	j = right + 1;
	pivot = left;   // �擪�v�f��pivot�Ƃ���

	do {
		do { i++; } while (array[i] > array[pivot]);
		do { j--; } while (array[pivot] > array[j]);
		// pivot��菬�������̂����ցA�傫�����̂��E��
		if (i < j) { Swap(&array[i], &array[j]); }
	} while (i < j);

	Swap(&array[pivot], &array[j]);   //pivot���X�V

	return j;
}

void Ranking::Swap(int* x, int* y)
{
	int temp;    // �l���ꎞ�ۑ�����ϐ�

	temp = *x;
	*x = *y;
	*y = temp;
}
/*********************�t�@�C���֐�*******************/
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

void Ranking::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/rankingData.bin", "wb");
	fwrite(&mNowPlayScoreTime, sizeof(ScoreData), 1, fp);
	fwrite(&mRankingData, sizeof(RankingData), 1, fp);
	fwrite(&mNoRankTextColor, sizeof(VECTOR4F), 1, fp);
	fclose(fp);

}
