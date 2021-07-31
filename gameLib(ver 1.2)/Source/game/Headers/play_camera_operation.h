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
	void SetStartPosition(const VECTOR3F& position) { mStartPosition = position; }
	//getter
	const bool& GetRespondFlag()const { return mRespond; }
	const bool& Respond() { return mRespondTimer > mParameter.respondTime; }
	//リセット
	void Reset()
	{
		mRespond = false;
		mRespondTimer = 0.0f;
	}
	//リスポン開始
	void StartRespond()
	{
		mRespond = true;
	}
private:
	//エディタ変数
	struct CameraParameter
	{
		float angle;
		float length;
		float y;
		float respondTime;
	};
	CameraParameter mParameter;
	//プレイヤー座標
	VECTOR3F mPlayerPosition;
	VECTOR3F mStartPosition;
	bool mRespond;
	float mRespondTimer;
};