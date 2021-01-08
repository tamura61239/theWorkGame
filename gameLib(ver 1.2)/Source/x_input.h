#pragma once
#include "vector.h"

#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0.lib")

class XInput
{
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
	int mId;

	State mCurrentState;
	State mPreviousState;

	float mDeadzoneX;
	float mDeadzoneY;

	void StickState();
	void TriggerState();
	float ApplyDeadZone(const float value, const float max_value, const float deadzone);
public:
	XInput(const int id, const float deadzone_x, const float deadzone_y);
	~XInput() {};

	void Update();
	bool ButtonPressedState(int pad) { if (this->mCurrentState.state.Gamepad.wButtons & pad)return true; return false; }
	bool ButtonRisingState(int pad) { if ((this->mCurrentState.state.Gamepad.wButtons & pad) && !(this->mPreviousState.state.Gamepad.wButtons & pad))return true; return false; }
	bool ButtonFallingState(int pad) { if (!(this->mCurrentState.state.Gamepad.wButtons & pad) && (this->mPreviousState.state.Gamepad.wButtons & pad))return true; return false; }

	float StickStateX(bool right) { if (right) return mCurrentState.rStick.x; else return mCurrentState.lStick.x; }
	float StickStateY(bool right) { if (right) return mCurrentState.rStick.y; else return mCurrentState.lStick.y; }

	bool TriggerPressedState(bool right) { if (right) return mCurrentState.rTrigger > 0.0f; else return mCurrentState.lTrigger > 0.0f; }
	bool TriggerRisingState(bool right) { if (right) return mCurrentState.rTrigger > 0.0f  && mPreviousState.rTrigger == 0.0f; else return mCurrentState.lTrigger > 0.0f  && mPreviousState.lTrigger == 0.0f; }
	bool TriggerFallingState(bool right) { if (right) return mCurrentState.rTrigger == 0.0f && mPreviousState.rTrigger > 0.0f;  else return mCurrentState.lTrigger == 0.0f && mPreviousState.lTrigger > 0.0f; }
};