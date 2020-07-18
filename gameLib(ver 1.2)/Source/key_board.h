#pragma once
#include<Windows.h>
class KeyBoad
{
public:
	void Update()
	{
		if (!GetKeyboardState(key))return;
		for (int i = 0; i < 256; i++)
		{
			before_key[i] = push_key[i];
			if (key[i] & 0x80)
			{
				push_key[i]++;
			}
			else
			{
				push_key[i] = 0;
			}
		}
	}
	//‰Ÿ‚µ‚½‚Æ‚«
	bool PressedState(const int key) { return push_key[key] > 0; }

	//‰Ÿ‚µŽn‚ß
	bool RisingState(const int key) { return  push_key[key] > 0 && before_key[key] == 0; }

	//—£‚µ‚½‚Æ‚«
	bool FallingState(const int key) { return  push_key[key] == 0 && before_key[key] > 0; }
	static KeyBoad& getinctanse()
	{
		static KeyBoad keyboad;
		return keyboad;
	}
private:
	BYTE key[256] = { 0 };
	int push_key[256] = { 0 };
	int before_key[256] = { 0 };
	KeyBoad()
	{
		for (int i = 0; i < 256; i++)
		{
			push_key[i] = 0;
			before_key[i] = 0;
		}
	}
};
#define pKeyBoad (KeyBoad::getinctanse())
enum KeyLabel
{
	A = static_cast<int>('A'),
	B = static_cast<int>('B'),
	C = static_cast<int>('C'),
	D = static_cast<int>('D'),
	E = static_cast<int>('E'),
	F = static_cast<int>('F'),
	G = static_cast<int>('G'),
	H = static_cast<int>('H'),
	I = static_cast<int>('I'),
	J = static_cast<int>('J'),
	K = static_cast<int>('K'),
	L = static_cast<int>('L'),
	M = static_cast<int>('M'),
	N = static_cast<int>('N'),
	O = static_cast<int>('O'),
	P = static_cast<int>('P'),
	Q = static_cast<int>('Q'),
	R = static_cast<int>('R'),
	S = static_cast<int>('S'),
	T = static_cast<int>('T'),
	U = static_cast<int>('U'),
	V = static_cast<int>('V'),
	W = static_cast<int>('W'),
	X = static_cast<int>('X'),
	Y = static_cast<int>('Y'),
	Z = static_cast<int>('Z'),

	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	TAB = VK_TAB,
	LSHIFT = VK_LSHIFT,
	RSHIFT = VK_RSHIFT,
	LCONTROL = VK_LCONTROL,
	RCONTROL = VK_RCONTROL,

	ESCAPE = VK_ESCAPE,
	SPACE = VK_SPACE,
	BACK = VK_BACK,
	DEL = VK_DELETE,
	ENTER = VK_RETURN,

	NUM1 = VK_NUMPAD1,
	NUM2 = VK_NUMPAD2,
	NUM3 = VK_NUMPAD3,
	NUM4 = VK_NUMPAD4,
	NUM5 = VK_NUMPAD5,
	NUM6 = VK_NUMPAD6,
	NUM7 = VK_NUMPAD7,
	NUM8 = VK_NUMPAD8,
	NUM9 = VK_NUMPAD9,
	NUM0 = VK_NUMPAD0,

	F1 = VK_F1,
	F2 = VK_F2,
	F3 = VK_F3,
	F4 = VK_F4,
	F5 = VK_F5,
	F6 = VK_F6,
	F7 = VK_F7,
	F8 = VK_F8,
	F9 = VK_F9,
	F10 = VK_F10,
	F11 = VK_F11,
	F12 = VK_F12,

};