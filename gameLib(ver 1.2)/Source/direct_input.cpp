#include "direct_input.h"
#include <vector>

LPDIRECTINPUT8			g_lpDI = nullptr;
std::vector<LPDIRECTINPUTDEVICE8> g_lpJoysticks;

BOOL PASCAL EnumJoyDeviceProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	LPDIRECTINPUTDEVICE8 g_lpJoystick;
	// ジョイスティックデバイスの作成
	HRESULT ret = g_lpDI->CreateDevice(lpddi->guidInstance, &g_lpJoystick, NULL);
	if (FAILED(ret)) {

		return DIENUM_STOP;
	}

	// 入力データ形式のセット
	ret = g_lpJoystick->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(ret)) {
		g_lpJoystick->Release();
		return DIENUM_STOP;
	}

	// 入力範囲のセット
	DIPROPRANGE	diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMax = 32767L;
	diprg.lMin = -32767L;

	// X axis
	diprg.diph.dwObj = DIJOFS_X;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// Y axis
	diprg.diph.dwObj = DIJOFS_Y;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// Z axis
	diprg.diph.dwObj = DIJOFS_Z;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// RZ axis
	diprg.diph.dwObj = DIJOFS_RZ;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	diprg.lMax = 255L;
	diprg.lMin = 0L;
	// RX axis
	diprg.diph.dwObj = DIJOFS_RX;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// RY axis
	diprg.diph.dwObj = DIJOFS_RY;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	g_lpJoystick->Poll();
	g_lpJoysticks.emplace_back(g_lpJoystick);
	// 最初の1つのみで終わる
	return DIENUM_CONTINUE;			// 次のデバイスを列挙するにはDIENUM_CONTINUEを返す
}


DirectInput::DirectInput(const int id, const float deadzone_x, const float deadzone_y, HINSTANCE instance)
{
	m_id = id;

	HRESULT hr = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&g_lpDI, NULL);
	if (FAILED(hr))
	{
		assert(!"Could not Create DirectInput8");
	}

	hr = g_lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyDeviceProc, NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr)) {
		g_lpDI->Release();
		assert(!"Could not Create EnumDevices");
	}
	if (g_lpJoysticks.empty())
	{
		return;
	}
	if (!g_lpJoysticks.at(m_id)) {
		// The gamepad was not find
		m_current_state.connected = false;
		return;
	}

	// Device information
	DIDEVCAPS dc;
	dc.dwSize = sizeof(dc);
	g_lpJoysticks.at(m_id)->GetCapabilities(&dc);

	// Start of operation
	g_lpJoysticks.at(m_id)->Acquire();
	m_current_state.connected = true;


	m_deadzone_y = deadzone_y;
	m_deadzone_x = deadzone_x;
}

DirectInput::~DirectInput()
{
	if (g_lpDI != nullptr)
		g_lpDI->Release();
	if (g_lpJoysticks.empty())
	{
		return;
	};
	if (g_lpJoysticks.at(m_id) != nullptr)
		g_lpJoysticks.at(m_id)->Release();

	
}

void DirectInput::Update()
{
	if (!m_current_state.connected)
		return;

	m_previous_state = m_current_state;

	HRESULT hr = g_lpJoysticks.at(m_id)->GetDeviceState(sizeof(m_current_state.state), &m_current_state.state);
	//if (FAILED(hr)) {
	//	// Start again
	//	g_lpJoystick->Acquire();
	//}

	DirectionButtonState();
	StickState();
	TriggerState();
}

bool DirectInput::ButtonPressedState(int button)
{
	if (!m_current_state.connected)
		return false;

	if (m_current_state.state.rgbButtons[button] & 0x80)
	{
		return true;
	}
	return false;
}
bool DirectInput::ButtonRisingState(int button)
{
	if ((m_current_state.state.rgbButtons[button] & 0x80) &&
		!(m_previous_state.state.rgbButtons[button] & 0x80))
	{
		return true;
	}
	return false;
}
bool DirectInput::ButtonFallingState(int button)
{
	if (!(m_current_state.state.rgbButtons[button] & 0x80) &&
		(m_previous_state.state.rgbButtons[button] & 0x80))
	{
		return true;
	}
	return false;
}

bool DirectInput::DirectionButtonPressedState(int button)
{
	return m_current_state.direction_button[button] > 0;
}
bool DirectInput::DirectionButtonRisingState(int button)
{
	return m_current_state.direction_button[button] > 0 && m_previous_state.direction_button[button] == 0;
}
bool DirectInput::DirectionButtonFallingState(int button)
{
	return m_current_state.direction_button[button] == 0 && m_previous_state.direction_button[button] > 0;
}

void DirectInput::DirectionButtonState()
{
	if (m_current_state.state.rgdwPOV[0] == 0 ||
		m_current_state.state.rgdwPOV[0] == 4500 ||
		m_current_state.state.rgdwPOV[0] == 31500)
		++m_current_state.direction_button[State::UP];
	else
	{
		m_current_state.direction_button[State::UP] = 0;
	}

	if (m_current_state.state.rgdwPOV[0] == 13500 ||
		m_current_state.state.rgdwPOV[0] == 18000 ||
		m_current_state.state.rgdwPOV[0] == 22500)
		++m_current_state.direction_button[State::DOWN];
	else
	{
		m_current_state.direction_button[State::DOWN] = 0;
	}

	if (m_current_state.state.rgdwPOV[0] == 22500 ||
		m_current_state.state.rgdwPOV[0] == 27000 ||
		m_current_state.state.rgdwPOV[0] == 31500)
		++m_current_state.direction_button[State::LEFT];
	else
	{
		m_current_state.direction_button[State::LEFT] = 0;
	}

	if (m_current_state.state.rgdwPOV[0] == 4500 ||
		m_current_state.state.rgdwPOV[0] == 9000 ||
		m_current_state.state.rgdwPOV[0] == 13500)
		++m_current_state.direction_button[State::RIGHT];
	else
	{
		m_current_state.direction_button[State::RIGHT] = 0;
	}
}
void DirectInput::StickState()
{
	m_current_state.l_stick.x = ApplyDeadZone(static_cast<float>(m_current_state.state.lX), MAX_STICKTILT, m_deadzone_x);

	m_current_state.l_stick.y = ApplyDeadZone(static_cast<float>(m_current_state.state.lY), MAX_STICKTILT, m_deadzone_y);

	m_current_state.r_stick.x = ApplyDeadZone(static_cast<float>(m_current_state.state.lZ), MAX_STICKTILT, m_deadzone_x);

	m_current_state.r_stick.y = ApplyDeadZone(static_cast<float>(m_current_state.state.lRz), MAX_STICKTILT, m_deadzone_y);
}

void DirectInput::TriggerState()
{
	m_current_state.l_trigger = ApplyDeadZone(static_cast<float>(m_current_state.state.lRx), MAX_TRRIGERTILT, 0.0f);
	m_current_state.r_trigger = ApplyDeadZone(static_cast<float>(m_current_state.state.lRy), MAX_TRRIGERTILT, 0.0f);
}

float DirectInput::ApplyDeadZone(const float value, const float max_value, const float deadzone)
{
	float normalize_value = value / max_value;

	if (normalize_value > -deadzone && normalize_value < deadzone)
	{
		return 0.0f;
	}

	return (std::max)((std::min)(normalize_value, 1.0f), -1.0f);
}