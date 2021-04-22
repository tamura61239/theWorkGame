#include "tutorial_state.h"
#include"key_board.h"
#include"texture.h"
#include"Easing.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

TutorialState::TutorialState(ID3D11Device* device) :mState(0), mBackGroundColor(1, 1, 1, 1), mTextAlpha(1), mCount(0), mTimer(0), mNextTime(0.2f), mTextPosition(0, 0), mTextSize(230, 36), mEasingTimer(3.f), mLockTime(0.5f)
, mKeyFlag(false)
{
	//�`��p
	mRender = std::make_unique<Sprite>(device, L"Data/image/siro.png");
	mTexts.resize(4);
	//�e�N�X�`���̐���
	LoadTextureFromFile(device, L"Data/image/�`���[�g���A���{�^��.png", mTexts[0].GetAddressOf());
	LoadTextureFromFile(device, L"Data/image/����2.png", mTexts[1].GetAddressOf());
	LoadTextureFromFile(device, L"Data/image/����1.png", mTexts[2].GetAddressOf());
	LoadTextureFromFile(device, L"Data/image/�����g.png", mTexts[3].GetAddressOf());
	//�p�����[�^�[��ݒ肷��
	mCbZoom = std::make_unique<ConstantBuffer<CbZoom>>(device);
	{
		FILE* fp;
		if (fopen_s(&fp, "Data/file/tutorial_data.bin", "rb") == 0)
		{
			fread(&mNextTime, sizeof(float), 1, fp);
			fread(&mLockTime, sizeof(float), 1, fp);
			fread(&mTextPosition, sizeof(VECTOR2F), 1, fp);
			fread(&mTextSize, sizeof(VECTOR2F), 1, fp);
			fread(&mCbZoom->data, sizeof(mCbZoom->data), 1, fp);
			fclose(fp);
		}
	}
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�	
/*****************************************************/
/**************************	�S�̗̂���̍X�V***************************/
float TutorialState::Update(float elapsdTime, PlayerCharacter* player)
{
	float timer = 1.0f;
	switch (mState)
	{
	case 0://�W�����v����񂵂����ǂ���
		CheckFirstJump(player);
		break;
	case 1://�ō����B�_�t�߂ɂȂ������V�[�����~�߂�
		timer = StopScene(elapsdTime);
		break;
	case 2://�L�[�����������ǂ���
		CheckNextExplanation(elapsdTime, player);
		break;
	case 3://������
		timer = CheckEndExplanation(elapsdTime);
		break;
	}
	return timer;
}

/************************�W�����v����񂵂����ǂ���**************************/
void TutorialState::CheckFirstJump(PlayerCharacter* player)
{
	if (mCount == 0)
	{//�W�����v�������ǂ���
		mCount += player->GetMoveState() == PlayerCharacter::MOVESTATE::JUMP ? 1 : 0;
	}
	else
	{//�ō����B�_�t�߂ɂȂ������ǂ���
		if (player->GetVelocity().y < 10)
		{
			mState++;
			mCount = 0;
			mKeyFlag = true;
		}
	}
}
/***************************�V�[�����~�߂Ă���Ƃ�*******************************/
float TutorialState::StopScene(float elapsdTime)
{
	if (pKeyBoad.RisingState(KeyLabel::SPACE))
	{//space�L�[�������ƃV�[���������o��
		mState++;
		mBackGroundColor = VECTOR4F(1, 1, 1, 1);
		mTextAlpha = 1.f;
		mTimer = 0;
		mKeyFlag = false;
		return 1.0f;
	}
	mTimer += elapsdTime;
	if (mTimer >= mLockTime)
	{//���ȏソ�Ɠ����Ȃ��Ȃ�
		mTimer = mLockTime;
	}
	//�������~�܂��Ă���
	return (1 - (mTimer / mLockTime)) * 0.05f;
}
/************************�W�����v���čō����B�_�t�߂܂ŗ����Ƃ�**********************/
void TutorialState::CheckNextExplanation(float elapsdTime, PlayerCharacter* player)
{
	mTimer += elapsdTime;
	if (mTimer >= mNextTime || player->GetGroundFlag())
	{
		mState++;
		mTimer = 0;
	}
}
/**********************************�W�����v���Ē��n������*******************************/
float TutorialState::CheckEndExplanation(float elapsdTime)
{
	if (pKeyBoad.RisingState(KeyLabel::RIGHT))
	{
		mCount = mCount + 1 == 2 ? 0 : 1;
	}
	if (pKeyBoad.RisingState(KeyLabel::LEFT))
	{
		mCount = mCount - 1 == -1 ? 1 : 0;
	}
	if (pKeyBoad.RisingState(KeyLabel::SPACE) && mCount == 1)
	{
		mTextAlpha = 1;
		mTimer = 0;
		mBackGroundColor = VECTOR4F(1, 1, 1, 1);
		mState++;
		mKeyFlag = true;
		return 1.0f;
	}
	static float s = 1;
	mTextAlpha = Easing::OutCirc(mTimer, mEasingTimer);
	mTimer += elapsdTime * s;
	if (mTimer > mEasingTimer || mTimer <= 0.f)
	{
		s *= -1.f;
	}
	mBackGroundColor = VECTOR4F(0.6f, 0.6f, 0.6f, 1);

	return 0.0f;
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void TutorialState::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("tutorial");
	ImGui::InputFloat(u8"���������o���܂ł̍ő厞��", &mNextTime, 0.1f);
	ImGui::InputFloat(u8"�~�߂�܂ł̎���", &mLockTime, 0.1f);
	//�\���ʒu�̒���
	float* position[2] = { &mTextPosition.x,&mTextPosition.y };
	ImGui::DragFloat2("position", *position);
	float* size[2] = { &mTextSize.x,&mTextSize.y };
	ImGui::DragFloat2("size", *size);
	//�p�����[�^�[�̕\��
	ImGui::Text("alpha:%f", mTextAlpha);
	ImGui::Text("zoom blur");
	ImGui::InputFloat("length", &mCbZoom->data.lenght, 0.1f);
	ImGui::InputInt("division", &mCbZoom->data.division, 1);
	//�Z�[�u
	if (ImGui::Button("save"))
	{
		FILE* fp;
		fopen_s(&fp, "Data/file/tutorial_data.bin", "wb");
		{
			fwrite(&mNextTime, sizeof(float), 1, fp);
			fwrite(&mLockTime, sizeof(float), 1, fp);
			fwrite(&mTextPosition, sizeof(VECTOR2F), 1, fp);
			fwrite(&mTextSize, sizeof(VECTOR2F), 1, fp);
			fwrite(&mCbZoom->data, sizeof(mCbZoom->data), 1, fp);
			fclose(fp);
		}

	}
	ImGui::End();
#endif
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
/***************************�{�^��(�{�^���������^�C�~���O)�̕\��**************************/
void TutorialState::RenderButton(ID3D11DeviceContext* context)
{
	if (mState == 1)
	{//�W�����v���ċ󒆂ɂ���Ƃ�
		mRender->Render(context, mTexts[0].Get(), mTextPosition, mTextSize, VECTOR2F(0, 0), VECTOR2F(500, 100), 0, VECTOR4F(0.6f, 0.6f, 0.6f, mTextAlpha));
	}

}
/****************************�������̕\��************************/
void TutorialState::RenderText(ID3D11DeviceContext* context)
{
	if (mState == 3)
	{//���n������
		mRender->Render(context, mTexts[3].Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0, VECTOR4F(1, 1, 1, mTextAlpha));
		mRender->Render(context, mTexts[mCount + 1].Get(), VECTOR2F(1920, 1080) * 0.1f, VECTOR2F(1920, 1080) * 0.8f, VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		if (mCount == 1)mRender->Render(context, mTexts[0].Get(), VECTOR2F(1920, 1080) - VECTOR2F(500 * 1.02f, 100 * 1.2f), VECTOR2F(500, 100), VECTOR2F(0, 0), VECTOR2F(500, 100), 0, VECTOR4F(1, 1, 1, mTextAlpha));

	}
}
