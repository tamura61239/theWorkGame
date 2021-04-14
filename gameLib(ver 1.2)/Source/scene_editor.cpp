#include "scene_editor.h"
#include"key_board.h"
#include<string>
#ifdef USE_IMGUI
#include <imgui.h>
#endif
/************************�G�f�B�^�[�֐�(ImGui���g���Ď��R�ɃV�[����J�ڂ����肷��)***************************/
int SceneEditor::Editor(bool* editorFlag, const int maxStageNumber)
{

	int num = 0;
	//F1�L�[�ŃG�f�B�^�[��ON�ɂ���
	if (pKeyBoad.RisingState(KeyLabel::F1))mEditorFlag = true;
	//�G�f�B�^�[��OFF�̎�
	if (!mEditorFlag)
	{
		*editorFlag = mEditorFlag;
		return num;
	}
#ifdef USE_IMGUI
	if (ImGui::BeginMainMenuBar())
	{
		//�J�ڐ�̃V�[����I������
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
				//�Q�[���V�[���łǂ̃X�e�[�W�ɑJ�ڂ��邩�I������
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
		//�G�f�B�^�[��OFF�ɂ���
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
	//���삵�����ʂ𑗂�
	*editorFlag = mEditorFlag;
	mSceneNo = num;
	return num;
}
