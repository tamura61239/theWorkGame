#include "gamepad.h"
#include <Windows.h>
#include <algorithm>
#include "direct_input.h"
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

GamePad::GamePad(const int id, const float deadzone_x, const float deadzone_y, bool xinput, HINSTANCE m_hinstance)
{
	mXinput = xinput;
	if (mXinput)
	{//XInputフラグがONの時
		mXPad = std::make_unique<XInput>(id, deadzone_x, deadzone_y);
	}
	else
	{//OFFの時
		mDPad = std::make_unique<DirectInput>(id, deadzone_x, deadzone_y, m_hinstance);
	}
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void GamePad::Update()
{
	//XInputフラグがONかOFFかでどっちを更新するかを決める
	mXinput ? mXPad->Update() : mDPad->Update();
}


namespace input
{
/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/

	void GamepadInitialize(int id, bool xinput, HINSTANCE m_hinstance, const float deadzone_x, const float deadzone_y)
	{
		GamePadManager::GetInstance().Initialize(id, xinput, m_hinstance, deadzone_x, deadzone_y);
		

	}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

	void GamepadUpdate() { GamePadManager::GetInstance().Update(); }
	/**********************************押してる間************************************/
	bool ButtonPressedState(const int id, PadLabel pad)
	{
		switch (pad)
		{
		case input::PadLabel::UP:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_DPAD_UP) :
				GamePadManager::GetInstance().DirectionButtonPressedState(id, 0);
			break;
		case input::PadLabel::DOWN:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_DPAD_DOWN) :
				GamePadManager::GetInstance().DirectionButtonPressedState(id, 1);
			break;
		case input::PadLabel::LEFT:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_DPAD_LEFT) :
				GamePadManager::GetInstance().DirectionButtonPressedState(id, 2);
			break;
		case input::PadLabel::RIGHT:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_DPAD_RIGHT) :
				GamePadManager::GetInstance().DirectionButtonPressedState(id, 3);
			break;
		case input::PadLabel::START:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_START) :
				GamePadManager::GetInstance().ButtonPressedState(id, 9);
			break;
		case input::PadLabel::BACK:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_BACK) :
				GamePadManager::GetInstance().ButtonPressedState(id, 8);
			break;
		case input::PadLabel::LTHUMB:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_LEFT_THUMB) :
				GamePadManager::GetInstance().ButtonPressedState(id, 10);
			break;
		case input::PadLabel::RTHUMB:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_RIGHT_THUMB) :
				GamePadManager::GetInstance().ButtonPressedState(id, 11);
			break;
		case input::PadLabel::LSHOULDER:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_LEFT_SHOULDER) :
				GamePadManager::GetInstance().ButtonPressedState(id, 4);
			break;
		case input::PadLabel::RSHOULDER:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_RIGHT_SHOULDER) :
				GamePadManager::GetInstance().ButtonPressedState(id, 5);
			break;
		case input::PadLabel::A:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_A) :
				GamePadManager::GetInstance().ButtonPressedState(id, 1);
			break;
		case input::PadLabel::B:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_B) :
				GamePadManager::GetInstance().ButtonPressedState(id, 2);
			break;
		case input::PadLabel::X:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_X) :
				GamePadManager::GetInstance().ButtonPressedState(id, 0);
			break;
		case input::PadLabel::Y:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonPressedState(id, XINPUT_GAMEPAD_Y) :
				GamePadManager::GetInstance().ButtonPressedState(id, 3);
			break;
		default:
			break;
		}
		return false;
	}
	/********************************押した瞬間**********************************/
	bool ButtonRisingState(const int id, PadLabel pad)
	{
		switch (pad)
		{
		case input::PadLabel::UP:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_DPAD_UP) :
				GamePadManager::GetInstance().DirectionButtonRisingState(id, 0);
			break;
		case input::PadLabel::DOWN:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_DPAD_DOWN) :
				GamePadManager::GetInstance().DirectionButtonRisingState(id, 1);
			break;
		case input::PadLabel::LEFT:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_DPAD_LEFT) :
				GamePadManager::GetInstance().DirectionButtonRisingState(id, 2);
			break;
		case input::PadLabel::RIGHT:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_DPAD_RIGHT) :
				GamePadManager::GetInstance().DirectionButtonRisingState(id, 3);
			break;
		case input::PadLabel::START:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_START) :
				GamePadManager::GetInstance().ButtonRisingState(id, 9);
			break;
		case input::PadLabel::BACK:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_BACK) :
				GamePadManager::GetInstance().ButtonRisingState(id, 8);
			break;
		case input::PadLabel::LTHUMB:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_LEFT_THUMB) :
				GamePadManager::GetInstance().ButtonRisingState(id, 10);
			break;
		case input::PadLabel::RTHUMB:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_RIGHT_THUMB) :
				GamePadManager::GetInstance().ButtonRisingState(id, 11);
			break;
		case input::PadLabel::LSHOULDER:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_LEFT_SHOULDER) :
				GamePadManager::GetInstance().ButtonRisingState(id, 4);
			break;
		case input::PadLabel::RSHOULDER:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_RIGHT_SHOULDER) :
				GamePadManager::GetInstance().ButtonRisingState(id, 5);
			break;
		case input::PadLabel::A:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_A) :
				GamePadManager::GetInstance().ButtonRisingState(id, 1);
			break;
		case input::PadLabel::B:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_B) :
				GamePadManager::GetInstance().ButtonRisingState(id, 2);
			break;
		case input::PadLabel::X:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_X) :
				GamePadManager::GetInstance().ButtonRisingState(id, 0);
			break;
		case input::PadLabel::Y:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonRisingState(id, XINPUT_GAMEPAD_Y) :
				GamePadManager::GetInstance().ButtonRisingState(id, 3);
			break;
		default:
			break;
		}

		return false;
	}
	/******************************離した瞬間*********************************/
	bool ButtonFallingState(const int id, PadLabel pad)
	{
		switch (pad)
		{
		case input::PadLabel::UP:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_DPAD_UP) :
				GamePadManager::GetInstance().DirectionButtonFallingState(id, 0);
			break;
		case input::PadLabel::DOWN:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_DPAD_DOWN) :
				GamePadManager::GetInstance().DirectionButtonFallingState(id, 1);
			break;
		case input::PadLabel::LEFT:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_DPAD_LEFT) :
				GamePadManager::GetInstance().DirectionButtonFallingState(id, 2);
			break;
		case input::PadLabel::RIGHT:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_DPAD_RIGHT) :
				GamePadManager::GetInstance().DirectionButtonFallingState(id, 3);
			break;
		case input::PadLabel::START:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_START) :
				GamePadManager::GetInstance().ButtonFallingState(id, 9);
			break;
		case input::PadLabel::BACK:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_BACK) :
				GamePadManager::GetInstance().ButtonFallingState(id, 8);
			break;
		case input::PadLabel::LTHUMB:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_LEFT_THUMB) :
				GamePadManager::GetInstance().ButtonFallingState(id, 10);
			break;
		case input::PadLabel::RTHUMB:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_RIGHT_THUMB) :
				GamePadManager::GetInstance().ButtonFallingState(id, 11);
			break;
		case input::PadLabel::LSHOULDER:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_LEFT_SHOULDER) :
				GamePadManager::GetInstance().ButtonFallingState(id, 4);
			break;
		case input::PadLabel::RSHOULDER:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_RIGHT_SHOULDER) :
				GamePadManager::GetInstance().ButtonFallingState(id, 5);
			break;
		case input::PadLabel::A:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_A) :
				GamePadManager::GetInstance().ButtonFallingState(id, 1);
			break;
		case input::PadLabel::B:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_B) :
				GamePadManager::GetInstance().ButtonFallingState(id, 2);
			break;
		case input::PadLabel::X:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_X) :
				GamePadManager::GetInstance().ButtonFallingState(id, 0);
			break;
		case input::PadLabel::Y:
			return GamePadManager::GetInstance().GetXinput(id) ?
				GamePadManager::GetInstance().ButtonFallingState(id, XINPUT_GAMEPAD_Y) :
				GamePadManager::GetInstance().ButtonFallingState(id, 3);
			break;
		default:
			break;
		}
		return false;
	}
	/******************************スティックの傾き具合*****************************/
	//x軸
	float StickStateX(const int pad_id, bool right)
	{
		return GamePadManager::GetInstance().StickStateX(pad_id, right);
	}
	//y軸
	float StickStateY(const int pad_id, bool right)
	{
		return GamePadManager::GetInstance().StickStateY(pad_id, right);
	}
	/*****************************スティックの押してる間****************************/
	bool TriggerPressedState(const int pad_id, bool right)
	{
		return GamePadManager::GetInstance().TriggerPressedState(pad_id, right);
	}
	/*****************************スティックの押した瞬間****************************/
	bool TriggerRisingState(const int pad_id, bool right)
	{
		return GamePadManager::GetInstance().TriggerRisingState(pad_id, right);
	}
	/*****************************スティックの離した瞬間****************************/
	bool TriggerFallingState(const int pad_id, bool right)
	{
		return GamePadManager::GetInstance().TriggerFallingState(pad_id, right);
	}
}