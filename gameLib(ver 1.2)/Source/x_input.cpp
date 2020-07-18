#include "x_input.h"
#include <Windows.h>
#include <algorithm>

XInput::XInput(const int id, const float deadzone_x, const float deadzone_y)
{
	m_id = id;

	DWORD result = XInputGetState(m_id, &m_current_state.state);
	if (result != ERROR_DEVICE_NOT_CONNECTED)
	{
		m_current_state.connected = true;
	}

	m_deadzone_y = deadzone_y;
	m_deadzone_x = deadzone_x;
}

void XInput::Update()
{
	if (!m_current_state.connected)
		return;

	m_previous_state = m_current_state;

	DWORD result = XInputGetState(m_id, &m_current_state.state);


	StickState();
	TriggerState();
}

void XInput::StickState()
{
	m_current_state.l_stick.x = ApplyDeadZone(static_cast<float>(m_current_state.state.Gamepad.sThumbLX), MAX_STICKTILT, m_deadzone_x);

	m_current_state.l_stick.y = ApplyDeadZone(static_cast<float>(m_current_state.state.Gamepad.sThumbLY), MAX_STICKTILT, m_deadzone_y);

	m_current_state.r_stick.x = ApplyDeadZone(static_cast<float>(m_current_state.state.Gamepad.sThumbRX), MAX_STICKTILT, m_deadzone_x);

	m_current_state.r_stick.y = ApplyDeadZone(static_cast<float>(m_current_state.state.Gamepad.sThumbRY), MAX_STICKTILT, m_deadzone_y);
}

void XInput::TriggerState()
{
	m_current_state.l_trigger = ApplyDeadZone(static_cast<float>(m_current_state.state.Gamepad.bLeftTrigger), MAX_TRRIGERTILT, 0.0f);
	m_current_state.r_trigger = ApplyDeadZone(static_cast<float>(m_current_state.state.Gamepad.bRightTrigger), MAX_TRRIGERTILT, 0.0f);
}

float XInput::ApplyDeadZone(const float value, const float max_value, const float deadzone)
{
	float normalize_value = value / max_value;

	if (normalize_value > -deadzone && normalize_value < deadzone)
	{
		return 0.0f;
	}

	return (std::max)((std::min)(normalize_value, 1.0f), -1.0f);
}