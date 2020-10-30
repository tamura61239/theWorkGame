#pragma once
#include"camera.h"
#include<memory>

class StageEditorCameraOperation
{
public:
	StageEditorCameraOperation(std::shared_ptr<Camera>camera);
	//çXêV
	void ImGuiUpdate();
	void Update(float elapsedTime);
	//setter
	void SetNewPosition(const VECTOR3F& position) 
	{ 
		mNewPosition = position; 
		mMoveFlag = true;
	}
	//getter
	const bool GetMoveFlag() { return mMoveFlag; }
private:
	struct CameraData
	{
		float mAngleX;
		float mAngleY;
		float r;
	};
	CameraData mData;
	std::weak_ptr<Camera>mCamera;
	VECTOR3F mNewPosition;
	bool mMoveFlag;
};