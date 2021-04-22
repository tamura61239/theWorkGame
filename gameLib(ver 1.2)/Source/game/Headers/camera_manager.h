#pragma once
#include"camera.h"
#include"camera_operation.h"
#include"singleton_class.h"

class CameraManager:public Singleton<CameraManager>
{
public:
	//初期化
	void Initialize(ID3D11Device*device, const int scene);
	//更新
	void Update(float elapsed_time);
	//エディター
	void Editor();
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
	//解放
	void DestroyCamera();
private:
	//カメラクラスの変数
	std::unique_ptr<Camera>mCamera;
	//カメラ操作クラスの変数
	std::unique_ptr<CameraOperation>mCameraOperation;
	//projection行列に必要なパラメーター変数
	float defaultFov = 0;
	float defaultAspect = 0;
	float defaultNearZ = 0;
	float defaultFarZ = 0;
};
#define pCameraManager (CameraManager::GetInctance())