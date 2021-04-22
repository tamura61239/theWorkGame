#pragma once
#include<memory>
#include"sprite.h"

class Fade
{
public:
	//シーン
	enum class FADE_SCENE
	{
		TITLE,
		SELECT,
		GAME,
		RESULT,
		MAX,
	};
	//動きの状態
	enum class FADE_MODO
	{
		NONE,
		FADEIN,
		FADEOUT
	};
	//フェードのデータのロード
	void StartLoad();
	Fade(ID3D11Device* device, FADE_SCENE scene);
	//setter
	void SetFadeScene(FADE_SCENE scene);
	//getter
	const bool GetEndFlag() { return mNowFadeData.mEndFlag; }
	const FADE_MODO GetFadeScene() { return mState; }
	//start関数
	void StartFadeIn()
	{
		//フェートイン開始
		mState = FADE_MODO::FADEIN;
		mNowFadeData.mData.mColor.w = 1;

	}
	void StartFadeOut()
	{
		//フェートアウト開始
		{
			mState = FADE_MODO::FADEOUT;
			mNowFadeData.mData.mColor.w = 0;
		}
	}
	//clear関数
	void Clear()
	{
		mState = FADE_MODO::NONE;
		mNowFadeData.mEndFlag = false;
	}
	//エディタ
	void Editor();
	//更新
	void Update(float elapstTime);
	//描画
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
	//動き
	void Move(float elapsdTime, FadeScene* scene);
	//今のシーンのデータ
	FadeScene mNowFadeData;
	//エディタで操作するシーンのデータ
	FadeScene mCheckFadeData;
	bool mTestFlag;
	//状態
	FADE_MODO mState;
	//エディタで操作するフェードのシーン
	int mEditorScene;
	//全シーンのフェードデータ
	FadeScene mFadeDatas[static_cast<int>(FADE_SCENE::MAX)];
	//描画用
	std::unique_ptr<Sprite>mSprite;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
};