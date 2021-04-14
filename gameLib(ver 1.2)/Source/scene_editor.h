#pragma once
class SceneEditor
{
public:
	//コンストラクタ
	SceneEditor():mStageNo(0),mSceneNo(0), mEditorFlag(false){}
	//エディター
	int Editor(bool* editorFlag,const int maxStageNumber);
	//getter
	const int GetStageNo() { return mStageNo; }
	const int GetSceneNo() { return mSceneNo; }
private:
	//選んだステージNumber
	int mStageNo;
	//選んだシーン
	int mSceneNo;
	//エディターがONかOFFかのフラグ
	bool mEditorFlag;
};