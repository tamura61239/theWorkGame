#pragma once
#include"camera.h"
#include<memory>


class PlayCameraOperation
{
public:
	//コンストラクタ
	PlayCameraOperation();
	//エディタ
	void Editor();
	//更新
	void Update(Camera* camera,float elapsedTime);
	//setter
	void SetPlayerPosition(const VECTOR3F& position) { mPlayerPosition = position; }
private:
	//エディタ変数
	struct CameraParameter
	{
		float angle;
		float length;
		float y;
	};
	CameraParameter mParameter;
	//プレイヤー座標
	VECTOR3F mPlayerPosition;
};