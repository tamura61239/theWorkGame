#include "camera_manager.h"
#include<d3d11.h>

#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

void CameraManager::CreateCamera(ID3D11Device* device)
{
	mCamera = std::make_shared<Camera>(device);
	mCameraOperation = std::make_unique<CameraOperation>(mCamera);
	mCamera->SetPerspective(defaultFov, defaultAspect, defaultNearZ, defaultFarZ);
}

void CameraManager::Update(float elapsed_time)
{
#ifdef USE_IMGUI
	ImGui::Begin("camera");
	int cameraType = static_cast<int>(mCameraOperation->GetCameraType());
	ImGui::RadioButton("normal", &cameraType, 0);
	ImGui::RadioButton("debug", &cameraType, 1);
	ImGui::RadioButton("title camera", &cameraType, 2);
	mCameraOperation->SetCameraType(static_cast<CameraOperation::CAMERA_TYPE>(cameraType));
	ImGui::End();
#endif
	mCameraOperation->Update(elapsed_time);
	mCamera->CalculateMatrix();
}

void CameraManager::DestroyCamera()
{
	mCamera.reset();
	mCameraOperation.reset();
}
