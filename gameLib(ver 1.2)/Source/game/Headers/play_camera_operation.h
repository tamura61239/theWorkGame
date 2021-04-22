#pragma once
#include"camera.h"
#include<memory>


class PlayCameraOperation
{
public:
	//�R���X�g���N�^
	PlayCameraOperation();
	//�G�f�B�^
	void Editor();
	//�X�V
	void Update(Camera* camera,float elapsedTime);
	//setter
	void SetPlayerPosition(const VECTOR3F& position) { mPlayerPosition = position; }
private:
	//�G�f�B�^�ϐ�
	struct CameraParameter
	{
		float angle;
		float length;
		float y;
	};
	CameraParameter mParameter;
	//�v���C���[���W
	VECTOR3F mPlayerPosition;
};