#include "scene_editor.h"
#include"key_board.h"
#include<string>
#ifdef USE_IMGUI
#include <imgui.h>
#endif

int SceneEditor::Editor(bool* editorFlag, const int maxStageNumber)
{

	int num = 0;
#ifdef USE_IMGUI
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("scene"))
		{
			if (ImGui::MenuItem("title"))
			{
				num = 1;
			}
			if (ImGui::MenuItem("select"))
			{
				num = 2;
			}
			if (ImGui::BeginMenu("game"))
			{
				for (int i = 0; i < maxStageNumber; i++)
				{
					std::string name = "stage" + std::to_string(i);
					if (ImGui::MenuItem(name.c_str()))
					{
						mStageNo = i;
						num = 3;
					}
				}
				ImGui::EndMenu();

			}
			if (ImGui::MenuItem("result"))
			{
				num = 4;
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("editor"))
		{
			if (ImGui::MenuItem("ON"))
			{
				mEditorFlag = true;
			}
			if (ImGui::MenuItem("OFF"))
			{
				mEditorFlag = false;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
#endif
	*editorFlag = mEditorFlag;
	mSceneNo = num;
	return num;
}
