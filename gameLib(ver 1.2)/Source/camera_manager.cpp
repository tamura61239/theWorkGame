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
	mCameraOperation->Update(elapsed_time);
	mCamera->CalculateMatrix();
}

void CameraManager::DestroyCamera()
{
	mCamera.reset();
	mCameraOperation.reset();
}
