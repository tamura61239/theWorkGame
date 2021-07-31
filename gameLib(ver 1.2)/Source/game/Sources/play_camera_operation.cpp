#include "play_camera_operation.h"
#include"static_obj.h"
#include"file_function.h"
#include"Easing.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
PlayCameraOperation::PlayCameraOperation() :mRespond(false), mRespondTimer(0.0f)
{
	mParameter.angle = 3.14f * 0.75f;
	mParameter.length = 60;
	mParameter.y = 25;
	mParameter.respondTime = 0.3f;
	FileFunction::Load(mParameter, "Data/file/playerCameraParameter.bin", "rb");
}

/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void PlayCameraOperation::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("player camera");
	ImGui::SliderFloat("camera angle", &mParameter.angle, -3.14f, 3.14f);
	ImGui::InputFloat("camera length", &mParameter.length, 10);
	ImGui::InputFloat("camera y", &mParameter.y, 5);
	ImGui::SliderFloat("respond time", &mParameter.respondTime, 0,2);
	ImGui::Text("start parameter");
	if (ImGui::Button("save"))
	{
		FileFunction::Save(mParameter, "Data/file/playerCameraParameter.bin", "wb");

	}
	ImGui::End();
#endif
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void PlayCameraOperation::Update(Camera* camera, float elapsedTime)
{
	if (mRespond)
	{
		VECTOR3F focus;
		mRespondTimer += elapsedTime;
		DirectX::XMStoreFloat3(&focus, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&mPlayerPosition), DirectX::XMLoadFloat3(&mStartPosition), std::min(mRespondTimer/mParameter.respondTime,1.0f)));
		camera->SetFocus(focus);
		camera->SetEye(focus + VECTOR3F(sinf(mParameter.angle) * mParameter.length, mParameter.y, cosf(mParameter.angle) * mParameter.length) * gameObjScale);

	}
	else
	{
		//カメラがプレイヤー座標からの位置に調整
		camera->SetFocus(mPlayerPosition);
		camera->SetEye(mPlayerPosition + VECTOR3F(sinf(mParameter.angle) * mParameter.length, mParameter.y, cosf(mParameter.angle) * mParameter.length) * gameObjScale);
	}
}

