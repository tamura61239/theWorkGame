#include "camera_manager.h"
#include<d3d11.h>

#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/
void CameraManager::Initialize(ID3D11Device* device,const int scene)
{
	//カメラクラスの生成
	mCamera = std::make_unique<Camera>(device);
	//カメラ操作クラスの生成
	mCameraOperation = std::make_unique<CameraOperation>(mCamera.get(),scene);
	//カメラにprojection行列に必要なパラメーターを設定する
	mCamera->SetPerspective(defaultFov, defaultAspect, defaultNearZ, defaultFarZ);
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void CameraManager::Update(float elapsed_time)
{
	//カメラ操作クラスの更新
	mCameraOperation->Update(mCamera.get(),elapsed_time);
	//カメラクラスの更新
	mCamera->CalculateMatrix();
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void CameraManager::Editor()
{
	//カメラ操作クラスのエディタ関数を呼ぶ
	mCameraOperation->Editor(mCamera.get());
}
/*****************************************************/
//　　　　　　　　　　解放関数
/*****************************************************/
void CameraManager::DestroyCamera()
{
	mCameraOperation.reset();
	mCamera.reset();
}
