#include "camera_operation.h"
#include<Windows.h>
#include"key_board.h"
#include<string>
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

CameraOperation::CameraOperation(std::shared_ptr<Camera> camera) :mType(CAMERA_TYPE::NORMAL), mTitleSceneChangeFlag(false), time(0), mEndTitleFlag(false), mLerpMovement(0)
{
	mCamera = camera;
	VECTOR3F focusF = mCamera->GetFocus();
	VECTOR3F eyeF = mCamera->GetEye();
	VECTOR3F l = focusF - eyeF;
	distance = sqrtf(l.x * l.x + l.y * l.y + l.z * l.z);
}
//エディタ関数
void CameraOperation::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("camera");
	int type = static_cast<int>(mType);
	ImGui::RadioButton("normal", &type, 0);
	ImGui::RadioButton("debug", &type, 1);
	ImGui::RadioButton("title camera", &type, 2);
	mType = static_cast<CAMERA_TYPE>(type);
	ImGui::Separator();
	VECTOR3F eye = mCamera->GetEye();
	VECTOR3F focus = mCamera->GetFocus();
	VECTOR3F up = mCamera->GetUp();
	ImGui::Text("[eye] x:%f y:%f z:%f", eye.x, eye.y, eye.z);
	ImGui::Text("[focus] x:%f y:%f z:%f", focus.x, focus.y, focus.z);
	ImGui::Text("[up] x:%f y:%f z:%f", up.x, up.y, up.z);
	ImGui::Separator();
	if (mType == CAMERA_TYPE::NORMAL)
	{

	}
	else if (mType == CAMERA_TYPE::DEBUG)
	{

	}
	else if (mType == CAMERA_TYPE::TITLE_CAMERA)
	{
		ImGui::InputFloat("startEye.x", &mTitleData.mEye.x, 1);
		ImGui::InputFloat("startEye.y", &mTitleData.mEye.y, 1);
		ImGui::InputFloat("startEye.z", &mTitleData.mEye.z, 1);
		ImGui::SliderFloat("startFront.x", &mTitleData.mFront.x, -1, 1);
		ImGui::SliderFloat("startFront.y", &mTitleData.mFront.y, -1, 1);
		ImGui::SliderFloat("startFront.z", &mTitleData.mFront.z, -1, 1);
		ImGui::SliderFloat("lerpMin", &mTitleData.mMinLerp, 0, 1);
		ImGui::SliderFloat("lerpMax", &mTitleData.mMaxLerp, 0, 1);
		ImGui::InputFloat("startTime", &mTitleData.startTime, 0.1f);
		ImGui::SliderFloat("lerp change amount", &mTitleData.mLerpChangeAmount, 0, 1);
		ImGui::Checkbox("start flag", &mTitleSceneChangeFlag);
		if (!mTitleSceneChangeFlag)
		{
			mCamera->SetEye(mTitleData.mEye);
			mCamera->SetFocus(mTitleData.mEye + mTitleData.mFront);
			time = 0;
		}
		if (ImGui::Button("save"))
		{
			SaveTitleData();
		}
	}

	ImGui::End();
#endif

}

void CameraOperation::Update(float elapsedTime)
{

	switch (mType)
	{
	case CAMERA_TYPE::DEBUG:
		DebugCamera();
		break;
	case CAMERA_TYPE::TITLE_CAMERA:
		TitleCamera(elapsedTime);
		break;
	}
}
//デバックカメラ
void CameraOperation::DebugCamera()
{
	VECTOR3F focusF = mCamera->GetFocus();
	VECTOR3F upF = mCamera->GetUp();
	VECTOR3F rightF = mCamera->GetRight();
	VECTOR3F eyeF = mCamera->GetEye();
	POINT cursor;
	::GetCursorPos(&cursor);

	oldCursor = newCursor;
	newCursor = VECTOR2F(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

	float move_x = (newCursor.x - oldCursor.x) * 0.02f;
	float move_y = (newCursor.y - oldCursor.y) * 0.02f;

	// Altキー
	if (::GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			// Y軸回転
			rotate.y += move_x * 0.5f;
			if (rotate.y > DirectX::XM_PI)
			{
				rotate.y -= DirectX::XM_2PI;
			}
			else if (rotate.y < -DirectX::XM_PI)
			{
				rotate.y += DirectX::XM_2PI;
			}
			// X軸回転
			rotate.x += move_y * 0.5f;
			if (rotate.x > DirectX::XM_PI)
			{
				rotate.x -= DirectX::XM_2PI;
			}
			else if (rotate.x < -DirectX::XM_PI)
			{
				rotate.x += DirectX::XM_2PI;
			}
		}
		else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
		{
			// 平行移動
			float s = distance * 0.035f;
			float x = -move_x * s;
			float y = move_y * s;

			focusF.x += rightF.x * x;
			focusF.y += rightF.y * x;
			focusF.z += rightF.z * x;

			focusF.x += upF.x * y;
			focusF.y += upF.y * y;
			focusF.z += upF.z * y;
		}
		else if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			// ズーム
			distance += (-move_y - move_x) * distance * 0.1f;
		}


		float sx = ::sinf(rotate.x);
		float cx = ::cosf(rotate.x);
		float sy = ::sinf(rotate.y);
		float cy = ::cosf(rotate.y);

		DirectX::XMVECTOR front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
		DirectX::XMVECTOR right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVector3Cross(right, front);

		DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&focusF);
		DirectX::XMVECTOR distance = DirectX::XMVectorSet(this->distance, this->distance, this->distance, 0.0f);
		DirectX::XMVECTOR eye = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));
		DirectX::XMStoreFloat3(&eyeF, eye);
		DirectX::XMStoreFloat3(&upF, up);
		mCamera->SetEye(eyeF);
		mCamera->SetFocus(focusF);
		mCamera->SetUp(upF);
	}
}
//タイトルシーンのカメラ
void CameraOperation::TitleCamera(float elapsedTime)
{
	if (!mTitleSceneChangeFlag)return;//spaceキーが押されたかどうか
	time += elapsedTime;
	//押されてから一定時間がたったかどうか
	if (time >= mTitleData.startTime)
	{
		VECTOR3F eye = mCamera->GetEye();
		DirectX::XMVECTOR eyeVecc = DirectX::XMLoadFloat3(&eye);
		DirectX::XMVECTOR endPosVecc = DirectX::XMLoadFloat3(&mTitleData.endPosition);
		float length = 0;
		DirectX::XMStoreFloat(&mLerpMovement, DirectX::XMVectorLerp(DirectX::XMLoadFloat(&mTitleData.mMinLerp), DirectX::XMLoadFloat(&mTitleData.mMaxLerp), mTitleData.mLerpChangeAmount*60*elapsedTime));
		//カメラのeye座標からendPosition(最終座標)までlerp関数で移動する
		eyeVecc = DirectX::XMVectorLerp(eyeVecc, endPosVecc, mLerpMovement);
		//カメラのeye座標からendPosition(最終座標)までの距離を測る
		DirectX::XMStoreFloat(&length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(eyeVecc, endPosVecc)));
		DirectX::XMStoreFloat3(&eye, eyeVecc);
		mCamera->SetEye(eye);
		mCamera->SetFocus(eye + mTitleData.mFront);
		//距離が一定以下かどうか
		if (length <= 20)
		{
			mEndTitleFlag = true;
		}
	}
}

//titleDataのロード
void CameraOperation::LoadTitleData()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/Title_camera.bin", "rb") == 0)
	{
		fread(&mTitleData, sizeof(TitleCameraData), 1, fp);
		fclose(fp);
	}
	else
	{
		mTitleData.mEye = mCamera->GetEye();
		VECTOR3F front = mCamera->GetFocus() - mTitleData.mEye;
		DirectX::XMStoreFloat3(&mTitleData.mFront, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
		mTitleData.endPosition = VECTOR3F(0, 0, 100);
	}
	mCamera->SetEye(mTitleData.mEye);
	mCamera->SetFocus(mTitleData.mEye + mTitleData.mFront);
}
//titleDataのセーブ
void CameraOperation::SaveTitleData()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/Title_camera.bin", "wb");

	fwrite(&mTitleData, sizeof(TitleCameraData), 1, fp);
	fclose(fp);


}

