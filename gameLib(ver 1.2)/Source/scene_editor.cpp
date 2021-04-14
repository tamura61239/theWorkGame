#include "scene_editor.h"
#include"key_board.h"
#include<string>
#ifdef USE_IMGUI
#include <imgui.h>
#endif
/************************エディター関数(ImGuiを使って自由にシーンを遷移したりする)***************************/
int SceneEditor::Editor(bool* editorFlag, const int maxStageNumber)
{

	int num = 0;
	//F1キーでエディターをONにする
	if (pKeyBoad.RisingState(KeyLabel::F1))mEditorFlag = true;
	//エディターがOFFの時
	if (!mEditorFlag)
	{
		*editorFlag = mEditorFlag;
		return num;
	}
#ifdef USE_IMGUI
	if (ImGui::BeginMainMenuBar())
	{
		//遷移先のシーンを選択する
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
				//ゲームシーンでどのステージに遷移するか選択する
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
		//エディターをOFFにする
		if (ImGui::BeginMenu("editor"))
		{
			if (ImGui::MenuItem("OFF"))
			{
				mEditorFlag = false;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
#endif
	//操作した結果を送る
	*editorFlag = mEditorFlag;
	mSceneNo = num;
	return num;
}
