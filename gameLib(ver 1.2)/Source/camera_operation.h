#pragma once
#include"camera.h"
#include<memory>

class CameraOperation
{
public:
	CameraOperation(std::shared_ptr<Camera>camera);
	void Update(float elapsedTime);
	void DebugCamera();
	enum CAMERA_TYPE
	{
		NORMAL,
		DEBUG
	};
	//setter
	void SetCameraType(CAMERA_TYPE type) { mType = type; }
private:
	std::shared_ptr<Camera>mCamera;
	CAMERA_TYPE mType;
	//DebugCamera
	VECTOR2F oldCursor;
	VECTOR2F newCursor;
	float distance;
	VECTOR2F rotate;
};