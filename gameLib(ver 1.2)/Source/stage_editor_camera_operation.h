#pragma once
#include"camera.h"
#include<memory>

class StageEditorCameraOperation
{
public:
	//�R���X�g���N�^
	StageEditorCameraOperation(Camera* camera);
	//�G�f�B�^
	void Editor(Camera* camera);
	//�X�V
	void Update(Camera* camera,float elapsedTime);
	//setter
	void SetNewPosition(const VECTOR3F& position) 
	{//���W���X�V
		mNewPosition = position; 
		mMoveFlag = true;
	}
	//getter
	const bool GetMoveFlag() { return mMoveFlag; }
private:
	//�G�f�B�^�ϐ�
	struct CameraData
	{
		float mAngleX;
		float mAngleY;
		float r;
	};
	CameraData mData;
	//���̍��W
	VECTOR3F mNewPosition;
	bool mMoveFlag;
};