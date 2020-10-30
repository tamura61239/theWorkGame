#pragma once
class SceneEditor
{
public:
	SceneEditor():mStageNo(0),mSceneNo(0), mEditorFlag(false){}
	int Editor(bool* editorFlag,const int maxStageNumber);
	//getter
	const int GetStageNo() { return mStageNo; }
	const int GetSceneNo() { return mSceneNo; }
private:
	int mStageNo;
	int mSceneNo;
	bool mEditorFlag;
};