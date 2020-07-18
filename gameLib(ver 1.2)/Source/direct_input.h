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
		int direction_button[4];
		float r_trigger;
		float l_trigger;

		VECTOR2F r_stick;
		VECTOR2F l_stick;

		bool connected = false;
	};

	static constexpr float MAX_STICKTILT = 32767.0f;
	static constexpr float MAX_TRRIGERTILT = 255.0f;
	int m_id;

	State m_current_state;
	State m_previous_state;

	float m_deadzone_x;
	float m_deadzone_y;

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

	float StickStateX(bool right) { if (right) return m_current_state.r_stick.x; else return m_current_state.l_stick.x; }
	float StickStateY(bool right) { if (right) return m_current_state.r_stick.y; else return m_current_state.l_stick.y; }

	bool TriggerPressedState(bool right) { if (right) return m_current_state.r_trigger > 0.0f; else return m_current_state.l_trigger > 0.0f; }
	bool TriggerRisingState(bool right) { if (right) return m_current_state.r_trigger > 0.0f  && m_previous_state.r_trigger == 0.0f; else return m_current_state.l_trigger > 0.0f  && m_previous_state.l_trigger == 0.0f; }
	bool TriggerFallingState(bool right) { if (right) return m_current_state.r_trigger == 0.0f && m_previous_state.r_trigger > 0.0f;  else return m_current_state.l_trigger == 0.0f && m_previous_state.l_trigger > 0.0f; }

};