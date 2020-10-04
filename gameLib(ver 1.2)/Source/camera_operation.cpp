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

CameraOperation::CameraOperation(std::shared_ptr<Camera> camera) :mType(CAMERA_TYPE::NORMAL),angle1(0),angle2(0)
{
	mCamera = camera;
	VECTOR3F focusF = mCamera->GetFocus();
	VECTOR3F eyeF = mCamera->GetEye();
	VECTOR3F l = focusF - eyeF;
	distance = sqrtf(l.x * l.x + l.y * l.y + l.z * l.z);
}

void CameraOperation::Update(float elapsedTime)
{
	switch (mType)
	{
	case CAMERA_TYPE::DEBUG:
		DebugCamera();
		break;
	case CAMERA_TYPE::TITLE_CAMERA:
		TitleCamera();
		break;
	}
}

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
#ifdef USE_IMGUI
	ImGui::Begin("DebugCamera");
	ImGui::Text("[eye] x:%f y:%f z:%f", eyeF.x, eyeF.y, eyeF.z);
	ImGui::Text("[focus] x:%f y:%f z:%f", focusF.x, focusF.y, focusF.z);
	ImGui::Text("[up] x:%f y:%f z:%f", upF.x, upF.y, upF.z);
	ImGui::End();
#endif
}

void CameraOperation::TitleCamera()
{
#ifdef USE_IMGUI
	ImGui::Begin("camera parameter");
	VECTOR3F eye = mCamera->GetEye();
	VECTOR3F forcs = mCamera->GetFocus();

	static float moveSize = 1;
	ImGui::InputFloat("moveSize", &moveSize, 1);
	ImGui::InputFloat("eye.x", &eye.x, moveSize);
	ImGui::InputFloat("eye.y", &eye.y, moveSize);
	ImGui::InputFloat("eye.z", &eye.z, moveSize);

	//VECTOR3F vec = forcs - eye;
	//float dot1 = 0,dot2 = 0;
	//VECTOR3F cross1, cross2;
	//DirectX::XMVECTOR right= DirectX::XMLoadFloat3(&VECTOR3F(1, 0, 0));
	//DirectX::XMVECTOR v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vec));
	//DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&VECTOR3F(0, 1, 0));

	//DirectX::XMStoreFloat(&dot1, DirectX::XMVector3Dot(right, v));
	//DirectX::XMStoreFloat(&dot2, DirectX::XMVector3Dot(up, v));

	//DirectX::XMStoreFloat3(&cross1, DirectX::XMVector3Cross(right, v));
	//DirectX::XMStoreFloat3(&cross2, DirectX::XMVector3Cross(up, v));

	//angle1 = acosf(dot1);
	//if (cross1.y < 0)angle1 *= -1;
	//angle2 = acosf(dot2);
	//if (cross2.z < 0)angle2 *= -1;

	//float angle1 = 0, dot1 = 0;
	//DirectX::XMVECTOR  right = DirectX::XMLoadFloat3(&VECTOR3F(1, 0, 0));
	//DirectX::XMVECTOR  vec1 = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&VECTOR3F(forcs.x - eye.x, 0, forcs.z - eye.z)));
	//DirectX::XMStoreFloat(&dot1, DirectX::XMVector3Dot(right, vec1));
	//VECTOR3F cross1 = VECTOR3F(0, 0, 0);
	//DirectX::XMStoreFloat3(&cross1, DirectX::XMVector3Cross(right, vec1));

	//if (cross1.y >= 0)angle1 = acos(dot1);
	//else angle1 = -acos(dot1);

	//float angle2 = 0, dot2 = 0;
	//DirectX::XMVECTOR  up = DirectX::XMLoadFloat3(&VECTOR3F(0, 1, 0));
	//DirectX::XMVECTOR  vec2 = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&VECTOR3F(forcs.x - eye.x, forcs.y - eye.y,0)));
	//VECTOR3F cross2 = VECTOR3F(0, 0, 0);
	//DirectX::XMStoreFloat(&dot2, DirectX::XMVector3Dot(up, vec2));
	//DirectX::XMStoreFloat3(&cross2, DirectX::XMVector3Cross(up, vec2));

	//if (cross2.z >= 0)angle2 = acos(dot2);
	//else angle2 = -acos(dot2);

	ImGui::SliderFloat("angle.x", &angle1, -3.14f, 3.14f);
	ImGui::SliderFloat("angle.y", &angle2, -3.14f, 3.14f);
	static float length = 50;
	ImGui::InputFloat("length", &length, 1);
	forcs.x = sinf(angle2) * sinf(angle1);
	forcs.y = cosf(angle2);
	forcs.z = sinf(angle2) * cosf(angle1);
	forcs = forcs * length + eye;
	mCamera->SetEye(eye);
	mCamera->SetFocus(forcs);
	ImGui::Text("eye %f %f %f", eye.x, eye.y, eye.z);
	ImGui::Text("focus %f %f %f", forcs.x, forcs.y, forcs.z);
	if (ImGui::Button("save"))Save();
	if (ImGui::Button("load"))Load();
	ImGui::End();

#endif
}

void CameraOperation::Load()
{
	std::string typeName = { "" };
	switch (mType)
	{
	case CAMERA_TYPE::TITLE_CAMERA:
		typeName = "Title_camera";
		break;
	}
	if (typeName._Equal(""))return;
	std::string fileName = { "Data/file/" };
	fileName += typeName;
	fileName += ".bin";
	FILE* fp;
	VECTOR3F eye = VECTOR3F(0, 0, 0);
	VECTOR3F focus = VECTOR3F(0, 0, 0);
	if (fopen_s(&fp, fileName.c_str(), "rb") == 0)
	{
		fread(&eye, sizeof(VECTOR3F), 1, fp);
		fread(&focus, sizeof(VECTOR3F), 1, fp);
		fread(&angle1, sizeof(float), 1, fp);
		fread(&angle2, sizeof(float), 1, fp);
		fclose(fp);
		mCamera->SetEye(eye);
		mCamera->SetFocus(focus);
	}
}

void CameraOperation::Save()
{
	std::string typeName = { "" };
	switch (mType)
	{
	case CAMERA_TYPE::TITLE_CAMERA:
		typeName = "Title_camera";
		break;
	}
	if (typeName._Equal(""))return;
	std::string fileName = { "Data/file/" };
	fileName += typeName;
	fileName += ".bin";
	FILE* fp;
	VECTOR3F eye = mCamera->GetEye();
	VECTOR3F focus = mCamera->GetFocus();
	fopen_s(&fp, fileName.c_str(), "wb");

	fwrite(&eye, sizeof(VECTOR3F), 1, fp);
	fwrite(&focus, sizeof(VECTOR3F), 1, fp);
	fwrite(&angle1, sizeof(float), 1, fp);
	fwrite(&angle2, sizeof(float), 1, fp);
	fclose(fp);


}

