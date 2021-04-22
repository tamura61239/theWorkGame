#pragma once
#include "vector.h"

#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0.lib")
//XInputクラス
class XInput
{
public:
	//コンストラクタ
	XInput(const int id, const float deadzone_x, const float deadzone_y);
	~XInput() {};
	//更新
	void Update();
	//ボタンを押してる間
	bool ButtonPressedState(int pad) { if (this->mCurrentState.state.Gamepad.wButtons & pad)return true; return false; }
	//ボタンを押した瞬間
	bool ButtonRisingState(int pad) { if ((this->mCurrentState.state.Gamepad.wButtons & pad) && !(this->mPreviousState.state.Gamepad.wButtons & pad))return true; return false; }
	//ボタンを離した瞬間
	bool ButtonFallingState(int pad) { if (!(this->mCurrentState.state.Gamepad.wButtons & pad) && (this->mPreviousState.state.Gamepad.wButtons & pad))return true; return false; }

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
	//入力状態
	struct State {
		XINPUT_STATE state;

		float rTrigger;
		float lTrigger;

		VECTOR2F rStick;
		VECTOR2F lStick;

		bool connected = false;
	};

	static constexpr float MAX_STICKTILT = 32767.0f;
	static constexpr float MAX_TRRIGERTILT = 255.0f;

	//今のフレームのパッドの状態
	State mCurrentState;
	//前のフレームのパッドの状態
	State mPreviousState;
	//コントローラーのID
	int mId;

	float mDeadzoneX;
	float mDeadzoneY;

	void StickState();
	void TriggerState();
	float ApplyDeadZone(const float value, const float max_value, const float deadzone);

};