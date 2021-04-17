#pragma once
#include<memory>
#include"sprite.h"

class Fade
{
public:
	//�V�[��
	enum class FADE_SCENE
	{
		TITLE,
		SELECT,
		GAME,
		RESULT,
		MAX,
	};
	//�����̏��
	enum class FADE_MODO
	{
		NONE,
		FADEIN,
		FADEOUT
	};
	//�t�F�[�h�̃f�[�^�̃��[�h
	void StartLoad();
	Fade(ID3D11Device* device, FADE_SCENE scene);
	//setter
	void SetFadeScene(FADE_SCENE scene);
	//getter
	const bool GetEndFlag() { return mNowFadeData.mEndFlag; }
	const FADE_MODO GetFadeScene() { return mState; }
	//start�֐�
	void StartFadeIn()
	{
		//�t�F�[�g�C���J�n
		mState = FADE_MODO::FADEIN;
		mNowFadeData.mData.mColor.w = 1;

	}
	void StartFadeOut()
	{
		//�t�F�[�g�A�E�g�J�n
		{
			mState = FADE_MODO::FADEOUT;
			mNowFadeData.mData.mColor.w = 0;
		}
	}
	//clear�֐�
	void Clear()
	{
		mState = FADE_MODO::NONE;
		mNowFadeData.mEndFlag = false;
	}
	//�G�f�B�^
	void Editor();
	//�X�V
	void Update(float elapstTime);
	//�`��
	void Render(ID3D11DeviceContext* context);
private:
	struct FadeData
	{
		VECTOR4F mColor;
		float mInEndTime;
		float mOutEndTime;
	};
	struct FadeScene
	{
		FADE_SCENE mScene;
		FadeData mData;
		bool mEndFlag;
	};
	//����
	void Move(float elapsdTime, FadeScene* scene);
	//���̃V�[���̃f�[�^
	FadeScene mNowFadeData;
	//�G�f�B�^�ő��삷��V�[���̃f�[�^
	FadeScene mCheckFadeData;
	bool mTestFlag;
	//���
	FADE_MODO mState;
	//�G�f�B�^�ő��삷��t�F�[�h�̃V�[��
	int mEditorScene;
	//�S�V�[���̃t�F�[�h�f�[�^
	FadeScene mFadeDatas[static_cast<int>(FADE_SCENE::MAX)];
	//�`��p
	std::unique_ptr<Sprite>mSprite;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
};