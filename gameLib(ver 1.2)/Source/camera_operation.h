#pragma once
#include"camera.h"
#include<memory>

class CameraOperation
{
public:
	CameraOperation(std::shared_ptr<Camera>camera);
	void ImGuiUpdate();
	void Update(float elapsedTime);
	void DebugCamera();
	void TitleCamera(float elapsedTime);
	void LoadTitleData();
	void SaveTitleData();
	const bool GetTitleSceneChangeFlag() { return mTitleSceneChangeFlag; }
	void SetTitleSceneChangeFlag(const bool flag) { mTitleSceneChangeFlag = flag; }
	const bool GetEndTitleFlag() { return mEndTitleFlag; }
	void SetEndPosition(const VECTOR3F& end) { mTitleData.endPosition = end; }
	enum CAMERA_TYPE
	{
		NORMAL,
		DEBUG,
		TITLE_CAMERA
	};
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
	//setter
	void SetCameraType(CAMERA_TYPE type) { mType = type; }
	CAMERA_TYPE GetCameraType() { return mType; }
private:
	std::shared_ptr<Camera>mCamera;
	TitleCameraData mTitleData;
	CAMERA_TYPE mType;
	//DebugCamera
	VECTOR2F oldCursor;
	VECTOR2F newCursor;
	float distance;
	VECTOR2F rotate;
	float time;
	bool mTitleSceneChangeFlag;
	bool mEndTitleFlag;
	float mLerpMovement;
};