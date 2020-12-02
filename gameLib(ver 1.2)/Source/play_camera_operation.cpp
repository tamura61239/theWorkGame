#include "play_camera_operation.h"
#include"static_obj.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

PlayCameraOperation::PlayCameraOperation()
{
	Load();
}

void PlayCameraOperation::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("player camera");
	ImGui::SliderFloat("camera angle", &mParameter.angle, -3.14f, 3.14f);
	ImGui::InputFloat("camera length", &mParameter.length, 10);
	ImGui::InputFloat("camera y", &mParameter.y, 5);
	ImGui::Text("start parameter");
	ImGui::Checkbox("start production", &mStartProduction);
	ImGui::SliderFloat("start camera angle", &mStartParameter.angle, -3.14f, 3.14f);
	ImGui::InputFloat("start camera length", &mStartParameter.length, 10);
	ImGui::InputFloat("start camera y", &mStartParameter.y, 5);

	ImGui::End();
#endif
}

void PlayCameraOperation::Update(Camera* camera,float elapsedTime)
{
	camera->SetFocus(mPlayerPosition);
	camera->SetEye(mPlayerPosition+VECTOR3F(sinf(mParameter.angle) * mParameter.length,mParameter.y,cosf(mParameter.angle) * mParameter.length)*gameObjScale);
}

void PlayCameraOperation::Load()
{
	FILE* fp;

	if (fopen_s(&fp, "Data/file/playerCameraParameter.bin", "rb") == 0)
	{
		fread(&mParameter, sizeof(CameraParameter), 1, fp);
		fclose(fp);
	}
	else
	{
		mParameter.angle = 3.14f * 0.75f;
		mParameter.length = 60;
		mParameter.y = 25;
		mStartParameter.angle = 3.14f * 0.75f;
		mStartParameter.length = 60;
		mStartParameter.y = 25;

	}

}

void PlayCameraOperation::Save()
{
	FILE* fp;

	fopen_s(&fp, "Data/file/playerCameraParameter.bin", "wb");
	CameraParameter parameter[] =
	{
		mParameter,
		mStartParameter
	};
	fwrite(parameter, sizeof(CameraParameter), 2, fp);
	fclose(fp);

}
