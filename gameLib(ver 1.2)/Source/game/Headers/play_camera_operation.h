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
	void SetStartPosition(const VECTOR3F& position) { mStartPosition = position; }
	//getter
	const bool& GetRespondFlag()const { return mRespond; }
	const bool& Respond() { return mRespondTimer > mParameter.respondTime; }
	//���Z�b�g
	void Reset()
	{
		mRespond = false;
		mRespondTimer = 0.0f;
	}
	//���X�|���J�n
	void StartRespond()
	{
		mRespond = true;
	}
private:
	//�G�f�B�^�ϐ�
	struct CameraParameter
	{
		float angle;
		float length;
		float y;
		float respondTime;
	};
	CameraParameter mParameter;
	//�v���C���[���W
	VECTOR3F mPlayerPosition;
	VECTOR3F mStartPosition;
	bool mRespond;
	float mRespondTimer;
};