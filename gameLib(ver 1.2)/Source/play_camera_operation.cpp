#include "play_camera_operation.h"
#include"static_obj.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

PlayCameraOperation::PlayCameraOperation()
{
	mParameter.angle = 3.14f * 0.75f;
	mParameter.length = 60;
	mParameter.y = 25;
	FileFunction::Load(mParameter, "Data/file/playerCameraParameter.bin", "rb");
}

void PlayCameraOperation::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("player camera");
	ImGui::SliderFloat("camera angle", &mParameter.angle, -3.14f, 3.14f);
	ImGui::InputFloat("camera length", &mParameter.length, 10);
	ImGui::InputFloat("camera y", &mParameter.y, 5);
	ImGui::Text("start parameter");
	if (ImGui::Button("save"))
	{
		FileFunction::Save(mParameter, "Data/file/playerCameraParameter.bin", "wb");

	}
	ImGui::End();
#endif
}

void PlayCameraOperation::Update(Camera* camera, float elapsedTime)
{
	camera->SetFocus(mPlayerPosition);
	camera->SetEye(mPlayerPosition + VECTOR3F(sinf(mParameter.angle) * mParameter.length, mParameter.y, cosf(mParameter.angle) * mParameter.length) * gameObjScale);
}

