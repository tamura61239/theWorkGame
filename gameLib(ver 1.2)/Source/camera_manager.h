#pragma once
#include"camera.h"
#include"camera_operation.h"

class CameraManager
{
public:
	void CreateCamera();
	void Update(float elapsed_time);
	//setter
	void SetDefaultPerspective(float fov, float aspect, float nearZ, float farZ)
	{
		defaultFov = fov;
		defaultAspect = aspect;
		defaultNearZ = nearZ;
		defaultFarZ = farZ;
	}
	//getter
	Camera* GetCamera() { return mCamera.get(); }
	CameraOperation* GetCameraOperation() { return mCameraOperation.get(); }
	void DestroyCamera();
	static CameraManager& GetInctance()
	{
		static CameraManager manager;
		return manager;
	}
private:
	CameraManager(){}
	std::shared_ptr<Camera>mCamera;
	std::unique_ptr<CameraOperation>mCameraOperation;
	float defaultFov = 0;
	float defaultAspect = 0;
	float defaultNearZ = 0;
	float defaultFarZ = 0;
};
#define pCamera (CameraManager::GetInctance())