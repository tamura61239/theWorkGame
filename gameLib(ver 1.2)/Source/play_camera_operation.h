#pragma once
#include"camera.h"
#include<memory>


class PlayCameraOperation
{
public:
	PlayCameraOperation();
	//更新
	void ImGuiUpdate();
	void Update(Camera* camera,float elapsedTime);
	//ファイル操作
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
	bool mStartProduction;
	VECTOR3F mPlayerPosition;
};