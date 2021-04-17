#include "fade.h"
#include<string>
#include"texture.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
//�t�@�C�����[�h�֐�
void Fade::StartLoad()
{
	for (int i = 0; i < static_cast<int>(FADE_SCENE::MAX); i++)
	{
		std::string fileName = { "Data/file/fade_out" };
		fileName += std::to_string(i) + ".bin";
		mFadeDatas[i].mData.mColor = VECTOR4F(0, 0, 0, 0);
		mFadeDatas[i].mData.mInEndTime = 0;
		mFadeDatas[i].mData.mOutEndTime = 0;

		FileFunction::Load(mFadeDatas[i].mData, fileName.c_str(), "rb");
		mFadeDatas[i].mScene = static_cast<FADE_SCENE>(i);
		mFadeDatas[i].mEndFlag = false;
	}
}
//�R���X�g���N�^
Fade::Fade(ID3D11Device* device, FADE_SCENE scene)
	:mTestFlag(false), mState(FADE_MODO::NONE), mEditorScene(static_cast<int>(scene))
{
	StartLoad();
	mNowFadeData = mFadeDatas[mEditorScene];
	mSprite = std::make_unique<Sprite>(device, L"Data/image/siro.png");
	LoadTextureFromFile(device, L"Data/image/siro.png", mSRV.GetAddressOf());
}
//�ǂ̃V�[������ݒ�
void Fade::SetFadeScene(FADE_SCENE scene)
{
	mEditorScene = static_cast<int>(scene);
	mNowFadeData = mFadeDatas[mEditorScene];
}

/****************�G�f�B�^�֐�*******************/
void Fade::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("fade out");
	if (!mTestFlag)
	{//�e�X�g�v���C���łȂ���
		static const int sceneMax = static_cast<int>(FADE_SCENE::MAX);
		static const char* fadeNames[sceneMax] = { "TITLE","SELECT","GAME","RESULT" };
		//�ǂ̃V�[���̃t�F�[�h�f�[�^�𑀍삷�邩���߂�
		ImGui::Combo("scene", &mEditorScene, fadeNames, sceneMax);
		//�f�[�^�̑���
		float* color[3] = { &mFadeDatas[mEditorScene].mData.mColor.x,&mFadeDatas[mEditorScene].mData.mColor.y ,&mFadeDatas[mEditorScene].mData.mColor.z };
		ImGui::ColorEdit3("color", *color);
		ImGui::InputFloat("fade in time", &mFadeDatas[mEditorScene].mData.mInEndTime, 0.1f);
		ImGui::InputFloat("fade out time", &mFadeDatas[mEditorScene].mData.mOutEndTime, 0.1f);
		//�Z�[�u
		if (ImGui::Button("save"))
		{
			std::string fileName = { "Data/file/fade_out" };
			fileName += std::to_string(mEditorScene) + ".bin";
			FileFunction::Save(mFadeDatas[mEditorScene].mData, fileName.c_str(), "rb");
		}
		//���[�h
		if (ImGui::Button("load"))
		{
			std::string fileName = { "Data/file/fade_out" };
			fileName += std::to_string(mEditorScene) + ".bin";
			FileFunction::Load(mFadeDatas[mEditorScene].mData, fileName.c_str(), "rb");

		}
	}
	else
	{//�e�X�g�v���C��
		int state = static_cast<int>(mState);
		if (ImGui::RadioButton("in", &state, 1))
		{//�t�F�[�h�C���J�n
			mState = FADE_MODO::FADEIN;
			mCheckFadeData.mData.mColor.w = 1;
		}
		if (ImGui::RadioButton("out", &state, 2))
		{//�t�F�[�h�A�E�g�J�n
			mState = FADE_MODO::FADEOUT;
			mCheckFadeData.mData.mColor.w = 0;
		}
		ImGui::Text("state:%d", state);
	}
	int scene = static_cast<int>(mNowFadeData.mScene);
	//�e�X�g�v���C�̊J�n�ƏI��
	if (ImGui::Checkbox("test move", &mTestFlag))
	{
		mCheckFadeData = mFadeDatas[scene];
		mState = FADE_MODO::NONE;
	}
	ImGui::End();
	if (mState == FADE_MODO::NONE)
	{	//�t�F�[�h�C���ł��A�E�g�ł��Ȃ���
		mNowFadeData = mFadeDatas[scene];
	}
#endif
}

void Fade::Update(float elapsdTime)
{
	if (mTestFlag)
	{
		Move(elapsdTime, &mCheckFadeData);
	}
	else
	{
		Move(elapsdTime, &mNowFadeData);
	}
}

void Fade::Render(ID3D11DeviceContext* context)
{
	
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);
	if (mTestFlag)
	{
		mSprite->Render(context, mSRV.Get(), VECTOR2F(0, 0), VECTOR2F(1920,1080), VECTOR2F(0, 0), VECTOR2F(1024, 1024), 0, mCheckFadeData.mData.mColor);
	}
	else if(mState!=FADE_MODO::NONE)
	{
		mSprite->Render(context, mSRV.Get(),VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1024, 1024), 0, mNowFadeData.mData.mColor);
	}
}


void Fade::Move(float elapsdTime, FadeScene* scene)
{
	switch (mState)
	{
	case FADE_MODO::NONE:
		break;
	case FADE_MODO::FADEIN:
		scene->mData.mColor.w -= elapsdTime * (1 / scene->mData.mInEndTime);
		if (scene->mData.mColor.w <= 0)
		{
			scene->mEndFlag = true;
		}
		break;
	case FADE_MODO::FADEOUT:
		scene->mData.mColor.w += elapsdTime * (1 / scene->mData.mOutEndTime);
		if (scene->mData.mColor.w >= 1)
		{
			scene->mEndFlag = true;
		}
		break;

	}

}
