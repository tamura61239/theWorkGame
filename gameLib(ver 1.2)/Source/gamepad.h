#pragma once
#include <memory>
#include "x_input.h"
#include "direct_input.h"

class GamePad
{
	bool m_xinput;
	std::unique_ptr<XInput> m_x_pad;
	std::unique_ptr<DirectInput> m_d_pad;
public:
	GamePad(const int id, const float deadzone_x, const float deadzone_y, bool xinput, HINSTANCE m_hinstance);
	~GamePad() {};

	void Update();
	bool ButtonPressedState(int pad) { return m_xinput ? m_x_pad->ButtonPressedState(pad) : m_d_pad->ButtonPressedState(pad); }
	bool ButtonRisingState(int pad) { return m_xinput ? m_x_pad->ButtonRisingState(pad) : m_d_pad->ButtonRisingState(pad); }
	bool ButtonFallingState(int pad) { return m_xinput ? m_x_pad->ButtonFallingState(pad) : m_d_pad->ButtonFallingState(pad); }

	//direction methods are only used for DirectInput
	bool DirectionButtonPressedState(int pad) { return m_d_pad->DirectionButtonPressedState(pad); }
	bool DirectionButtonRisingState(int pad) { return m_d_pad->DirectionButtonRisingState(pad); }
	bool DirectionButtonFallingState(int pad) { return m_d_pad->DirectionButtonFallingState(pad); }

	float StickStateX(bool right) { return  m_xinput ? m_x_pad->StickStateX(right) : m_d_pad->StickStateX(right); }
	float StickStateY(bool right) { return  m_xinput ? m_x_pad->StickStateY(right) : m_d_pad->StickStateY(right); }

	bool TriggerPressedState(bool right) { return m_xinput ? m_x_pad->TriggerPressedState(right) : m_d_pad->TriggerPressedState(right); }
	bool TriggerRisingState(bool right) { return m_xinput ? m_x_pad->TriggerRisingState(right) : m_d_pad->TriggerRisingState(right); }
	bool TriggerFallingState(bool right) { return m_xinput ? m_x_pad->TriggerFallingState(right) : m_d_pad->TriggerFallingState(right); }

	bool IsXInput() { return m_xinput; }
};

class GamePadManager
{
	std::unique_ptr<GamePad> pads[4];
	GamePadManager() {};
	~GamePadManager() {};
public:
	void Initialize(int id, bool xinput, HINSTANCE m_hinstance, const float deadzone_x = 0.1f, const float deadzone_y = 0.1f)
	{
		pads[id] = std::make_unique<GamePad>(id, deadzone_x, deadzone_y, xinput, m_hinstance);
	};

	void Update()
	{
		for (std::unique_ptr<GamePad>& pad : pads)
		{
			if (pad.get() != nullptr)
				pad->Update();
		}
	}

	bool ButtonPressedState(int id, int pad) { return this->pads[id]->ButtonPressedState(pad); }
	bool ButtonRisingState(int id, int pad) { return this->pads[id]->ButtonRisingState(pad); }
	bool ButtonFallingState(int id, int pad) { return this->pads[id]->ButtonFallingState(pad); }

	bool DirectionButtonPressedState(int id, int pad) { return pads[id]->DirectionButtonPressedState(pad); }
	bool DirectionButtonRisingState(int id, int pad) { return pads[id]->DirectionButtonRisingState(pad); }
	bool DirectionButtonFallingState(int id, int pad) { return pads[id]->DirectionButtonFallingState(pad); }

	float StickStateX(int id, bool right) { return this->pads[id]->StickStateX(right); }
	float StickStateY(int id, bool right) { return this->pads[id]->StickStateY(right); }

	bool TriggerPressedState(int id, bool right) { return this->pads[id]->TriggerPressedState(right); }
	bool TriggerRisingState(int id, bool right) { return this->pads[id]->TriggerRisingState(right); }
	bool TriggerFallingState(int id, bool right) { return this->pads[id]->TriggerFallingState(right); }

	bool IsXinput(int id) { return pads[id]->IsXInput(); }

	static GamePadManager& GetInstance()
	{
		static GamePadManager instance;
		return instance;
	}
};

namespace input
{
	enum class PadLabel
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		START,
		BACK,
		LTHUMB,
		RTHUMB,
		LSHOULDER,
		RSHOULDER,
		A,
		B,
		X,
		Y,
	};

	void GamepadInitialize(int id, bool xinput, HINSTANCE m_hinstance, const float deadzone_x = 0.1f, const float deadzone_y = 0.1f);
	void GamepadUpdate();

	bool ButtonPressedState(const int pad_id, PadLabel pad);
	bool ButtonRisingState(const int pad_id, PadLabel pad);
	bool ButtonFallingState(const int pad_id, PadLabel pad);

	float StickStateX(const int pad_id, bool right);
	float StickStateY(const int pad_id, bool right);

	bool TriggerPressedState(const int pad_id, bool right);
	bool TriggerRisingState(const int pad_id, bool right);
	bool TriggerFallingState(const int pad_id, bool right);
}
using namespace input;
