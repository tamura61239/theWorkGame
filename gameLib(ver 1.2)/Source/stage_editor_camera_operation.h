#pragma once
#include"camera.h"
#include<memory>

class StageEditorCameraOperation
{
public:
	//コンストラクタ
	StageEditorCameraOperation(Camera* camera);
	//エディタ
	void Editor(Camera* camera);
	//更新
	void Update(Camera* camera,float elapsedTime);
	//setter
	void SetNewPosition(const VECTOR3F& position) 
	{//座標を更新
		mNewPosition = position; 
		mMoveFlag = true;
	}
	//getter
	const bool GetMoveFlag() { return mMoveFlag; }
private:
	//エディタ変数
	struct CameraData
	{
		float mAngleX;
		float mAngleY;
		float r;
	};
	CameraData mData;
	//今の座標
	VECTOR3F mNewPosition;
	bool mMoveFlag;
};