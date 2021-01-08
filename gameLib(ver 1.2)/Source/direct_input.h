#pragma once
#define DIRECTINPUT_VERSION     0x0800
#include <windows.h>

#include "vector.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>


class DirectInput
{
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
	int mId;

	State mCurrentState;
	State mPreviousState;

	float mDeadzoneX;
	float mDeadzoneY;

	void DirectionButtonState();
	void StickState();
	void TriggerState();
	float ApplyDeadZone(const float value, const float max_value, const float deadzone);
public:

	DirectInput(const int id, const float deadzone_x, const float deadzone_y, HINSTANCE instance);
	~DirectInput();
		

	void Update();
	bool ButtonPressedState(int pad);
	bool ButtonRisingState(int pad);
	bool ButtonFallingState(int pad);

	bool DirectionButtonPressedState(int pad);
	bool DirectionButtonRisingState(int pad);
	bool DirectionButtonFallingState(int pad);

	float StickStateX(bool right) { if (right) return mCurrentState.rStick.x; else return mCurrentState.lStick.x; }
	float StickStateY(bool right) { if (right) return mCurrentState.rStick.y; else return mCurrentState.lStick.y; }

	bool TriggerPressedState(bool right) { if (right) return mCurrentState.rTrigger > 0.0f; else return mCurrentState.lTrigger > 0.0f; }
	bool TriggerRisingState(bool right) { if (right) return mCurrentState.rTrigger > 0.0f  && mPreviousState.rTrigger == 0.0f; else return mCurrentState.lTrigger > 0.0f  && mPreviousState.lTrigger == 0.0f; }
	bool TriggerFallingState(bool right) { if (right) return mCurrentState.rTrigger == 0.0f && mPreviousState.rTrigger > 0.0f;  else return mCurrentState.lTrigger == 0.0f && mPreviousState.lTrigger > 0.0f; }

};