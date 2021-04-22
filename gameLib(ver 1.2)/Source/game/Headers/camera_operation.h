#pragma once
#include"title_camera_operation.h"
#include"play_camera_operation.h"
#include"stage_editor_camera_operation.h"

class CameraOperation
{
public:
	//コンストラクタ
	CameraOperation(Camera*camera,int scene);
	//エディタ
	void Editor(Camera* camera);
	//更新
	void Update(Camera* camera, float elapsedTime);
	//デバックカメラ
	void DebugCamera(Camera* camera);
	//カメラの種類
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
	//カメラ操作クラス変数
	std::unique_ptr<TitleCameraOperation>mTitleCamera;
	std::unique_ptr<PlayCameraOperation>mPlayCamera;
	std::unique_ptr<StageEditorCameraOperation>mStageEditorCamera;
	//エディタ変数
	CAMERA_TYPE mType;
	//DebugCamera
	VECTOR2F oldCursor;
	VECTOR2F newCursor;
	float distance;
	VECTOR2F rotate;
	int mScene;
};