#pragma once
#include"camera.h"
#include"camera_operation.h"
#include"singleton_class.h"

class CameraManager:public Singleton<CameraManager>
{
public:
	void Initialize(ID3D11Device*device, const int scene);
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
private:
	std::shared_ptr<Camera>mCamera;
	std::unique_ptr<CameraOperation>mCameraOperation;
	float defaultFov = 0;
	float defaultAspect = 0;
	float defaultNearZ = 0;
	float defaultFarZ = 0;
};
#define pCameraManager (CameraManager::GetInctance())