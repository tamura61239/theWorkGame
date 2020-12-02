#pragma once
#include"camera.h"
#include<memory>


class TitleCameraOperation
{
public:
	TitleCameraOperation();
	//更新
	void ImGuiUpdate(Camera* camera);
	void Update(Camera*camera,float elapsedTime);
	//ファイル操作
	void Load(Camera* camera);
	void Save();
	//setter
	void SetEndPosition(const VECTOR3F& end) { mTitleData.endPosition = end; }
	void SetTitleSceneChangeFlag(const bool flag) { mTitleSceneChangeFlag = flag; }
	//getter
	const bool GetTitleSceneChangeFlag() { return mTitleSceneChangeFlag; }
	const bool GetEndTitleFlag() { return mEndTitleFlag; }
	const bool GetMoveFlag() { return mTime >= mTitleData.startTime ? true : false; }

private:
	struct TitleCameraData
	{
		VECTOR3F mEye;
		VECTOR3F mFront;
		float mMinLerp;
		float mMaxLerp;
		float mLerpChangeAmount;
		float startTime;
		VECTOR3F endPosition;
	};
	TitleCameraData mTitleData;
	bool mTitleSceneChangeFlag;
	bool mEndTitleFlag;
	float mLerpMovement;
	float mTime;
};