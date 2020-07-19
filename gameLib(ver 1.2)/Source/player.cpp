#include "player.h"
#include"camera_manager.h"
#include"key_board.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

Player::Player(std::shared_ptr<Character> character)
{
	mCharacter = character;
	mCharacter->GetModel()->PlayAnimation(0, true);
	mCharacter->SetScale(VECTOR3F(0.1f, 0.1f, 0.1f));
	mCharacter->SetPosition(VECTOR3F(0, 0, 30));
}

void Player::Update(float elapsd_time)
{
	VECTOR3F position = mCharacter->GetPosition();
	position.y -= 0.1f;

	mCharacter->SetPosition(position);
}
