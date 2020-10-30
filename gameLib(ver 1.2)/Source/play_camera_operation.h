#pragma once
#include"camera.h"
#include<memory>


class PlayCameraOperation
{
public:
	PlayCameraOperation(std::shared_ptr<Camera>camera);
	//�X�V
	void ImGuiUpdate();
	void Update(float elapsedTime);
	//�t�@�C������
	void Load();
	void Save();
	//setter
	void SetPlayerPosition(const VECTOR3F& position) { mPlayerPosition = position; }
	//getter
	const bool GetStartProductionFlag() { return mStartProduction; }
private:
	struct CameraParameter
	{
		float angle;
		float length;
		float y;
	};
	CameraParameter mParameter;
	CameraParameter mStartParameter;
	std::weak_ptr<Camera>mCamera;
	bool mStartProduction;
	VECTOR3F mPlayerPosition;
};