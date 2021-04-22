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
//�@�@�@�@�@�@�@�@�@�@�������֐�
/*****************************************************/
void CameraManager::Initialize(ID3D11Device* device,const int scene)
{
	//�J�����N���X�̐���
	mCamera = std::make_unique<Camera>(device);
	//�J��������N���X�̐���
	mCameraOperation = std::make_unique<CameraOperation>(mCamera.get(),scene);
	//�J������projection�s��ɕK�v�ȃp�����[�^�[��ݒ肷��
	mCamera->SetPerspective(defaultFov, defaultAspect, defaultNearZ, defaultFarZ);
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
void CameraManager::Update(float elapsed_time)
{
	//�J��������N���X�̍X�V
	mCameraOperation->Update(mCamera.get(),elapsed_time);
	//�J�����N���X�̍X�V
	mCamera->CalculateMatrix();
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void CameraManager::Editor()
{
	//�J��������N���X�̃G�f�B�^�֐����Ă�
	mCameraOperation->Editor(mCamera.get());
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@����֐�
/*****************************************************/
void CameraManager::DestroyCamera()
{
	mCameraOperation.reset();
	mCamera.reset();
}
