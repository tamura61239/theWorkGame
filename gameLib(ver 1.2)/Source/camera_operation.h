#pragma once
#include"title_camera_operation.h"
#include"play_camera_operation.h"
#include"stage_editor_camera_operation.h"

class CameraOperation
{
public:
	//�R���X�g���N�^
	CameraOperation(Camera*camera,int scene);
	//�G�f�B�^
	void Editor(Camera* camera);
	//�X�V
	void Update(Camera* camera, float elapsedTime);
	//�f�o�b�N�J����
	void DebugCamera(Camera* camera);
	//�J�����̎��
	enum class CAMERA_TYPE
	{
		NORMAL,
		DEBUG,
		TITLE_CAMERA,
		PLAY,
		STAGE_EDITOR
	};
	//setter
	void SetCameraType(CAMERA_TYPE type) { mType = type; }
	//getter
	CAMERA_TYPE GetCameraType() { return mType; }
	TitleCameraOperation* GetTitleCamera() { return mTitleCamera.get(); }
	PlayCameraOperation* GetPlayCamera() { return mPlayCamera.get(); }
	StageEditorCameraOperation* GetStageEditorCamera() { return mStageEditorCamera.get(); }

private:
	//�J��������N���X�ϐ�
	std::unique_ptr<TitleCameraOperation>mTitleCamera;
	std::unique_ptr<PlayCameraOperation>mPlayCamera;
	std::unique_ptr<StageEditorCameraOperation>mStageEditorCamera;
	//�G�f�B�^�ϐ�
	CAMERA_TYPE mType;
	//DebugCamera
	VECTOR2F oldCursor;
	VECTOR2F newCursor;
	float distance;
	VECTOR2F rotate;
	int mScene;
};