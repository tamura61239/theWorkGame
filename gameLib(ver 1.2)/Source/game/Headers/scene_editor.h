#pragma once
class SceneEditor
{
public:
	//�R���X�g���N�^
	SceneEditor():mStageNo(0),mSceneNo(0), mEditorFlag(false){}
	//�G�f�B�^�[
	int Editor(bool* editorFlag,const int maxStageNumber);
	//getter
	const int GetStageNo() { return mStageNo; }
	const int GetSceneNo() { return mSceneNo; }
private:
	//�I�񂾃X�e�[�WNumber
	int mStageNo;
	//�I�񂾃V�[��
	int mSceneNo;
	//�G�f�B�^�[��ON��OFF���̃t���O
	bool mEditorFlag;
};