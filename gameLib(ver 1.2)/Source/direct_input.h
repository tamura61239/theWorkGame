#pragma once
#define DIRECTINPUT_VERSION     0x0800
#include <windows.h>

#include "vector.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>

//DirectInputクラス
class DirectInput
{
public:
	//コンストラクタ
	DirectInput(const int id, const float deadzone_x, const float deadzone_y, HINSTANCE instance);
	~DirectInput();
		
	//更新
	void Update();
	//ボタンを押してる間
	bool ButtonPressedState(int pad);
	//ボタンを押した瞬間
	bool ButtonRisingState(int pad);
	//ボタンを離した瞬間
	bool ButtonFallingState(int pad);

	//十字ボタンを押してる間
	bool DirectionButtonPressedState(int pad);
	//十字ボタンを押した瞬間
	bool DirectionButtonRisingState(int pad);
	//十字ボタンを離した瞬間
	bool DirectionButtonFallingState(int pad);

	//スティックのｘ軸の傾き具合
	float StickStateX(bool right) { if (right) return mCurrentState.rStick.x; else return mCurrentState.lStick.x; }
	//スティックのｙ軸の傾き具合
	float StickStateY(bool right) { if (right) return mCurrentState.rStick.y; else return mCurrentState.lStick.y; }

	//スティックの押し込み中
	bool TriggerPressedState(bool right) { if (right) return mCurrentState.rTrigger > 0.0f; else return mCurrentState.lTrigger > 0.0f; }
	//スティックの押し込み開始
	bool TriggerRisingState(bool right) { if (right) return mCurrentState.rTrigger > 0.0f  && mPreviousState.rTrigger == 0.0f; else return mCurrentState.lTrigger > 0.0f  && mPreviousState.lTrigger == 0.0f; }
	//スティックの押し込み終了
	bool TriggerFallingState(bool right) { if (right) return mCurrentState.rTrigger == 0.0f && mPreviousState.rTrigger > 0.0f;  else return mCurrentState.lTrigger == 0.0f && mPreviousState.lTrigger > 0.0f; }
private:
	//状態
	struct State
	{
		DIJOYSTATE state;

		enum { UP, DOWN, LEFT, RIGHT };
		int directionButton[4];
		float rTrigger;
		float lTrigger;

		VECTOR2F rStick;
		VECTOR2F lStick;

		bool connected = false;
	};

	static constexpr float MAX_STICKTILT = 32767.0f;
	static constexpr float MAX_TRRIGERTILT = 255.0f;
	//ゲームパッドのID
	int mId;
	//今のフレームのパッドの状態
	State mCurrentState;
	//前のフレームのパッドの状態
	State mPreviousState;

	float mDeadzoneX;
	float mDeadzoneY;

	void DirectionButtonState();
	void StickState();
	void TriggerState();
	float ApplyDeadZone(const float value, const float max_value, const float deadzone);

};