#pragma once
#include"camera.h"
#include<memory>


class PlayCameraOperation
{
public:
	PlayCameraOperation();
	//çXêV
	void ImGuiUpdate();
	void Update(Camera* camera,float elapsedTime);
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
	bool mStartProduction;
	VECTOR3F mPlayerPosition;
};