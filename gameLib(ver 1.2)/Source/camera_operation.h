#pragma once
#include"title_camera_operation.h"
#include"play_camera_operation.h"
#include"stage_editor_camera_operation.h"

class CameraOperation
{
public:
	CameraOperation(std::shared_ptr<Camera>camera,int scene);
	void ImGuiUpdate();
	void Update(float elapsedTime);
	void DebugCamera();
	TitleCameraOperation* GetTitleCamera() { return mTitleCamera.get(); }
	PlayCameraOperation* GetPlayCamera() { return mPlayCamera.get(); }
	StageEditorCameraOperation* GetStageEditorCamera(){ return mStageEditorCamera.get(); }
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
private:
	std::unique_ptr<TitleCameraOperation>mTitleCamera;
	std::unique_ptr<PlayCameraOperation>mPlayCamera;
	std::unique_ptr<StageEditorCameraOperation>mStageEditorCamera;
	std::weak_ptr<Camera>mCamera;
	CAMERA_TYPE mType;
	//DebugCamera
	VECTOR2F oldCursor;
	VECTOR2F newCursor;
	float distance;
	VECTOR2F rotate;
	int mScene;
};