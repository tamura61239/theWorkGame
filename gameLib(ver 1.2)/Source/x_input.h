#pragma once
#include "vector.h"

#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0.lib")

class XInput
{
	struct State {
		XINPUT_STATE state;

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

	void StickState();
	void TriggerState();
	float ApplyDeadZone(const float value, const float max_value, const float deadzone);
public:
	XInput(const int id, const float deadzone_x, const float deadzone_y);
	~XInput() {};

	void Update();
	bool ButtonPressedState(int pad) { if (this->m_current_state.state.Gamepad.wButtons & pad)return true; return false; }
	bool ButtonRisingState(int pad) { if ((this->m_current_state.state.Gamepad.wButtons & pad) && !(this->m_previous_state.state.Gamepad.wButtons & pad))return true; return false; }
	bool ButtonFallingState(int pad) { if (!(this->m_current_state.state.Gamepad.wButtons & pad) && (this->m_previous_state.state.Gamepad.wButtons & pad))return true; return false; }

	float StickStateX(bool right) { if (right) return m_current_state.r_stick.x; else return m_current_state.l_stick.x; }
	float StickStateY(bool right) { if (right) return m_current_state.r_stick.y; else return m_current_state.l_stick.y; }

	bool TriggerPressedState(bool right) { if (right) return m_current_state.r_trigger > 0.0f; else return m_current_state.l_trigger > 0.0f; }
	bool TriggerRisingState(bool right) { if (right) return m_current_state.r_trigger > 0.0f  && m_previous_state.r_trigger == 0.0f; else return m_current_state.l_trigger > 0.0f  && m_previous_state.l_trigger == 0.0f; }
	bool TriggerFallingState(bool right) { if (right) return m_current_state.r_trigger == 0.0f && m_previous_state.r_trigger > 0.0f;  else return m_current_state.l_trigger == 0.0f && m_previous_state.l_trigger > 0.0f; }
};