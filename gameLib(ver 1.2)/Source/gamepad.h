#pragma once
#include <memory>
#include "x_input.h"
#include "direct_input.h"

//ゲームパッドの状態取得クラス
class GamePad
{
public:
	//コンストラクタ
	GamePad(const int id, const float deadzone_x, const float deadzone_y, bool xinput, HINSTANCE m_hinstance);
	//デストラクタ
	~GamePad() {};
	//更新
	void Update();
	/*****************************ボタンの状態************************************/
	//押してる間
	bool ButtonPressedState(int pad) { return mXinput ? mXPad->ButtonPressedState(pad) : mDPad->ButtonPressedState(pad); }
	//押し始め
	bool ButtonRisingState(int pad) { return mXinput ? mXPad->ButtonRisingState(pad) : mDPad->ButtonRisingState(pad); }
	//離した瞬間
	bool ButtonFallingState(int pad) { return mXinput ? mXPad->ButtonFallingState(pad) : mDPad->ButtonFallingState(pad); }

	/**************************DirectInputから持ってきた十字ボタンの状態*************************/
	//押してる間
	bool DirectionButtonPressedState(int pad) { return mDPad->DirectionButtonPressedState(pad); }
	//押し始め
	bool DirectionButtonRisingState(int pad) { return mDPad->DirectionButtonRisingState(pad); }
	//離した瞬間
	bool DirectionButtonFallingState(int pad) { return mDPad->DirectionButtonFallingState(pad); }

	/**********************************スティックの傾き具合**************************************/
	//X軸
	float StickStateX(bool right) { return  mXinput ? mXPad->StickStateX(right) : mDPad->StickStateX(right); }
	//Y軸
	float StickStateY(bool right) { return  mXinput ? mXPad->StickStateY(right) : mDPad->StickStateY(right); }

	/***************************スティックの押し込み*************************/
	//押してる間
	bool TriggerPressedState(bool right) { return mXinput ? mXPad->TriggerPressedState(right) : mDPad->TriggerPressedState(right); }
	//押し始め
	bool TriggerRisingState(bool right) { return mXinput ? mXPad->TriggerRisingState(right) : mDPad->TriggerRisingState(right); }
	//離した瞬間
	bool TriggerFallingState(bool right) { return mXinput ? mXPad->TriggerFallingState(right) : mDPad->TriggerFallingState(right); }
	//getter
	bool GetXInput() { return mXinput; }
private:
	//XInputで取得するかどうか
	bool mXinput;
	//XInput
	std::unique_ptr<XInput> mXPad;
	//DirectInput
	std::unique_ptr<DirectInput> mDPad;
};
//複数のゲームパッドの管理クラス
class GamePadManager
{
public:
	//初期化
	void Initialize(int id, bool xinput, HINSTANCE m_hinstance, const float deadzone_x = 0.1f, const float deadzone_y = 0.1f)
	{
		mPads[id] = std::make_unique<GamePad>(id, deadzone_x, deadzone_y, xinput, m_hinstance);
	};
	//更新
	void Update()
	{
		for (std::unique_ptr<GamePad>& pad : mPads)
		{
			if (pad.get() != nullptr)
				pad->Update();
		}
	}
	/*****************************ボタンの状態************************************/
	//押してる間
	bool ButtonPressedState(int id, int pad) { return this->mPads[id]->ButtonPressedState(pad); }
	//押し始め
	bool ButtonRisingState(int id, int pad) { return this->mPads[id]->ButtonRisingState(pad); }
	//離した瞬間
	bool ButtonFallingState(int id, int pad) { return this->mPads[id]->ButtonFallingState(pad); }

	/**************************DirectInputから持ってきた十字ボタンの状態*************************/
	//押してる間
	bool DirectionButtonPressedState(int id, int pad) { return mPads[id]->DirectionButtonPressedState(pad); }
	//押し始め
	bool DirectionButtonRisingState(int id, int pad) { return mPads[id]->DirectionButtonRisingState(pad); }
	//離した瞬間
	bool DirectionButtonFallingState(int id, int pad) { return mPads[id]->DirectionButtonFallingState(pad); }

	/**********************************スティックの傾き具合**************************************/
	//X軸
	float StickStateX(int id, bool right) { return this->mPads[id]->StickStateX(right); }
	//Y軸
	float StickStateY(int id, bool right) { return this->mPads[id]->StickStateY(right); }

	/***************************スティックの押し込み*************************/
	//押してる間
	bool TriggerPressedState(int id, bool right) { return this->mPads[id]->TriggerPressedState(right); }
	//押し始め
	bool TriggerRisingState(int id, bool right) { return this->mPads[id]->TriggerRisingState(right); }
	//離した瞬間
	bool TriggerFallingState(int id, bool right) { return this->mPads[id]->TriggerFallingState(right); }
	//getter
	bool GetXinput(int id) { return mPads[id]->GetXInput(); }
	//インスタンス
	static GamePadManager& GetInstance()
	{
		static GamePadManager instance;
		return instance;
	}
private:
	//ゲームパッド
	std::unique_ptr<GamePad> mPads[4];
	//コンストラクタ
	GamePadManager() {};
	//デストラクタ
	~GamePadManager() {};

};

namespace input
{
	//ボタンの種類
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
	//初期化
	void GamepadInitialize(int id, bool xinput, HINSTANCE m_hinstance, const float deadzone_x = 0.1f, const float deadzone_y = 0.1f);
	//更新
	void GamepadUpdate();
	/*****************************ボタンの状態************************************/
    //押してる間
	bool ButtonPressedState(const int pad_id, PadLabel pad);
	//押し始め
	bool ButtonRisingState(const int pad_id, PadLabel pad);
	//離した瞬間
	bool ButtonFallingState(const int pad_id, PadLabel pad);
	/**********************************スティックの傾き具合**************************************/
	//X軸
	float StickStateX(const int pad_id, bool right);
	//Y軸
	float StickStateY(const int pad_id, bool right);
	/***************************スティックの押し込み*************************/
	//押してる間
	bool TriggerPressedState(const int pad_id, bool right);
	//押し始め
	bool TriggerRisingState(const int pad_id, bool right);
	//離した瞬間
	bool TriggerFallingState(const int pad_id, bool right);
}
using namespace input;
