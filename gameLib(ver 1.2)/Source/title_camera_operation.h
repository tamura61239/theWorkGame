#pragma once
#include"camera.h"
#include<memory>


class TitleCameraOperation
{
public:
	TitleCameraOperation(std::shared_ptr<Camera>camera);
	//更新
	void ImGuiUpdate();
	void Update(float elapsedTime);
	//ファイル操作
	void Load();
	void Save();
	//setter
	void SetEndPosition(const VECTOR3F& end) { mTitleData.endPosition = end; }
	void SetTitleSceneChangeFlag(const bool flag) { mTitleSceneChangeFlag = flag; }
	//getter
	const bool GetTitleSceneChangeFlag() { return mTitleSceneChangeFlag; }
	const bool GetEndTitleFlag() { return mEndTitleFlag; }

private:
	std::weak_ptr<Camera>mCamera;
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