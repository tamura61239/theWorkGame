#pragma once
#include <memory>
#include "x_input.h"
#include "direct_input.h"

class GamePad
{
	bool mXinput;
	std::unique_ptr<XInput> mXPad;
	std::unique_ptr<DirectInput> mDPad;
public:
	GamePad(const int id, const float deadzone_x, const float deadzone_y, bool xinput, HINSTANCE m_hinstance);
	~GamePad() {};

	void Update();
	bool ButtonPressedState(int pad) { return mXinput ? mXPad->ButtonPressedState(pad) : mDPad->ButtonPressedState(pad); }
	bool ButtonRisingState(int pad) { return mXinput ? mXPad->ButtonRisingState(pad) : mDPad->ButtonRisingState(pad); }
	bool ButtonFallingState(int pad) { return mXinput ? mXPad->ButtonFallingState(pad) : mDPad->ButtonFallingState(pad); }

	//direction methods are only used for DirectInput
	bool DirectionButtonPressedState(int pad) { return mDPad->DirectionButtonPressedState(pad); }
	bool DirectionButtonRisingState(int pad) { return mDPad->DirectionButtonRisingState(pad); }
	bool DirectionButtonFallingState(int pad) { return mDPad->DirectionButtonFallingState(pad); }

	float StickStateX(bool right) { return  mXinput ? mXPad->StickStateX(right) : mDPad->StickStateX(right); }
	float StickStateY(bool right) { return  mXinput ? mXPad->StickStateY(right) : mDPad->StickStateY(right); }

	bool TriggerPressedState(bool right) { return mXinput ? mXPad->TriggerPressedState(right) : mDPad->TriggerPressedState(right); }
	bool TriggerRisingState(bool right) { return mXinput ? mXPad->TriggerRisingState(right) : mDPad->TriggerRisingState(right); }
	bool TriggerFallingState(bool right) { return mXinput ? mXPad->TriggerFallingState(right) : mDPad->TriggerFallingState(right); }

	bool IsXInput() { return mXinput; }
};

class GamePadManager
{
	std::unique_ptr<GamePad> mPads[4];
	GamePadManager() {};
	~GamePadManager() {};
public:
	void Initialize(int id, bool xinput, HINSTANCE m_hinstance, const float deadzone_x = 0.1f, const float deadzone_y = 0.1f)
	{
		mPads[id] = std::make_unique<GamePad>(id, deadzone_x, deadzone_y, xinput, m_hinstance);
	};

	void Update()
	{
		for (std::unique_ptr<GamePad>& pad : mPads)
		{
			if (pad.get() != nullptr)
				pad->Update();
		}
	}

	bool ButtonPressedState(int id, int pad) { return this->mPads[id]->ButtonPressedState(pad); }
	bool ButtonRisingState(int id, int pad) { return this->mPads[id]->ButtonRisingState(pad); }
	bool ButtonFallingState(int id, int pad) { return this->mPads[id]->ButtonFallingState(pad); }

	bool DirectionButtonPressedState(int id, int pad) { return mPads[id]->DirectionButtonPressedState(pad); }
	bool DirectionButtonRisingState(int id, int pad) { return mPads[id]->DirectionButtonRisingState(pad); }
	bool DirectionButtonFallingState(int id, int pad) { return mPads[id]->DirectionButtonFallingState(pad); }

	float StickStateX(int id, bool right) { return this->mPads[id]->StickStateX(right); }
	float StickStateY(int id, bool right) { return this->mPads[id]->StickStateY(right); }

	bool TriggerPressedState(int id, bool right) { return this->mPads[id]->TriggerPressedState(right); }
	bool TriggerRisingState(int id, bool right) { return this->mPads[id]->TriggerRisingState(right); }
	bool TriggerFallingState(int id, bool right) { return this->mPads[id]->TriggerFallingState(right); }

	bool IsXinput(int id) { return mPads[id]->IsXInput(); }

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
