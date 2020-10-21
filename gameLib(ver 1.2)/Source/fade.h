#pragma once
#include<memory>
#include"sprite.h"

class Fade
{
public:
	enum class FADE_SCENE
	{
		TITLE,
		SELECT,
		GAME,
		MAX,
	};
	enum class FADE_MODO
	{
		NONE,
		FADEIN,
		FADEOUT
	};
	void StartLoad();
	Fade(ID3D11Device*device, FADE_SCENE scene);
	//setter
	void SetFadeScene(FADE_SCENE scene);
	//getter
	const bool GetEndFlag() { return mNowFadeData.mEndFlag; }
	const FADE_MODO GetFadeScene() { return mState; }
	//startä÷êî
	void StartFadeIn()
	{
		if (mState == FADE_MODO::NONE)
		{
			mState = FADE_MODO::FADEIN;
			mNowFadeData.mData.mColor.w = 1;
		}
	}
	void StartFadeOut()
	{
		if (mState == FADE_MODO::NONE)
		{
			mState = FADE_MODO::FADEOUT;
			mNowFadeData.mData.mColor.w = 0;
		}
	}
	//clearä÷êî
	void Clear()
	{
		mState = FADE_MODO::NONE;
		mNowFadeData.mEndFlag = false;
	}
	void ImGuiUpdate();
	void Update(float elapstTime);
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

	void Load(int scene);
	void Save(int scene);
	void Move(float elapsdTime, FadeScene* scene);
	FadeScene mNowFadeData;
	FadeScene mCheckFadeData;
	bool mTestFlag;
	FADE_MODO mState;
	int mEditorScene;
	FadeScene mFadeDatas[static_cast<int>(FADE_SCENE::MAX)];
	std::unique_ptr<Sprite>mSprite;
};