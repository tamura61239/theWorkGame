#pragma once
#include"camera.h"
#include<memory>

class StageEditorCameraOperation
{
public:
	StageEditorCameraOperation(Camera* camera);
	//çXêV
	void ImGuiUpdate(Camera* camera);
	void Update(Camera* camera,float elapsedTime);
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
	VECTOR3F mNewPosition;
	bool mMoveFlag;
};