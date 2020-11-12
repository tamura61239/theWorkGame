#include "camera_operation.h"
#include<Windows.h>
#include<string>
#ifdef USE_IMGUI
#include <imgui.h>
#endif

CameraOperation::CameraOperation(std::shared_ptr<Camera> camera, int scene) :mType(CAMERA_TYPE::NORMAL), mScene(scene)
{
	mCamera = camera;
	VECTOR3F focusF = camera->GetFocus();
	VECTOR3F eyeF = camera->GetEye();
	VECTOR3F l = focusF - eyeF;
	distance = sqrtf(l.x * l.x + l.y * l.y + l.z * l.z);
	switch (mScene)
	{
	case 0:
		mTitleCamera = std::make_unique<TitleCameraOperation>(camera);
		break;
	case 1:
		mPlayCamera = std::make_unique<PlayCameraOperation>(camera);
		mStageEditorCamera = std::make_unique<StageEditorCameraOperation>(camera);
		break;
	}
}
//エディタ関数
void CameraOperation::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("camera");
	int type = static_cast<int>(mType);
	ImGui::RadioButton("normal", &type, 0);
	ImGui::RadioButton("debug", &type, 1);
	switch (mScene)
	{
	case 0:
		ImGui::RadioButton("title camera", &type, 2);
		break;
	case 1:
		ImGui::RadioButton("play camera", &type, 3);
		ImGui::RadioButton("stage editor camera", &type, 4);
		break;
	}
	if (mType != static_cast<CAMERA_TYPE>(type))
	{
		if (mType == CAMERA_TYPE::DEBUG)
		{
			mCamera.lock()->SetUp(VECTOR3F(0, 1, 0));
		}
	}
	mType = static_cast<CAMERA_TYPE>(type);
	ImGui::Separator();
	VECTOR3F eye = mCamera.lock()->GetEye();
	VECTOR3F focus = mCamera.lock()->GetFocus();
	VECTOR3F up = mCamera.lock()->GetUp();
	ImGui::Text("[eye] x:%f y:%f z:%f", eye.x, eye.y, eye.z);
	ImGui::Text("[focus] x:%f y:%f z:%f", focus.x, focus.y, focus.z);
	ImGui::Text("[up] x:%f y:%f z:%f", up.x, up.y, up.z);
	ImGui::Separator();
	switch (mType)
	{
	case CAMERA_TYPE::NORMAL:
		break;
	case CAMERA_TYPE::DEBUG:
		break;
	case CAMERA_TYPE::TITLE_CAMERA:
		mTitleCamera->ImGuiUpdate();
		break;
	case CAMERA_TYPE::PLAY:
		mPlayCamera->ImGuiUpdate();
		break;
	case CAMERA_TYPE::STAGE_EDITOR:
		mStageEditorCamera->ImGuiUpdate();
		break;
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
		mTitleCamera->Update(elapsedTime);
		break;
	case CAMERA_TYPE::PLAY:
		mPlayCamera->Update(elapsedTime);
		break;
	case CAMERA_TYPE::STAGE_EDITOR:
		mStageEditorCamera->Update(elapsedTime);
		break;
	}
}
//デバックカメラ
void CameraOperation::DebugCamera()
{
	VECTOR3F focusF = mCamera.lock()->GetFocus();
	VECTOR3F upF = mCamera.lock()->GetUp();
	VECTOR3F rightF = mCamera.lock()->GetRight();
	VECTOR3F eyeF = mCamera.lock()->GetEye();
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
		mCamera.lock()->SetEye(eyeF);
		mCamera.lock()->SetFocus(focusF);
		mCamera.lock()->SetUp(upF);
	}
}
