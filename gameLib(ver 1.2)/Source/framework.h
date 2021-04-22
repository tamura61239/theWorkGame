#pragma once
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include<d3d11.h>
#include <stdlib.h>
#include<memory>
#include <wrl.h>
#include "high_resolution_timer.h"
#include "misc.h"


#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

//フレームワーククラス
class Framework
{
public:
	//インスタンス
	static Framework& Instance() { return *mInst; }

	//コンストラクタ
	Framework(HWND hwnd) :mHwnd(hwnd)
	{
		mInst = this;

	}
	//デストラクタ
	~Framework() {}
	//ゲームループ
	int Run()
	{
		MSG msg = {};
		//初期化
		if (!Initialize(mHwnd)) return 0;
		mTimer.reset();
		//シーンのループ
		while (WM_QUIT != msg.message)
		{
			//スレッドのメッセージキューにメッセージがあるかどうかを確認
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				//fps計算
				mTimer.tick();
				//fps表示
				CalculateFrameStats();
				//更新
				Update(mTimer.time_interval());
				//描画
				Render(mTimer.time_interval());

			}
		}

		//ImGuiの終了処理
#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif


		return static_cast<int>(msg.wParam);
	}
	//取得したメッセージに合わせた処理をする
	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		//imguiにもメッセージなどを送る
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return 1;
#endif

		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			mTimer.stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			mTimer.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		return 0;
	}
	//getter
	HWND GetHwnd() { return mHwnd; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>GetDepthStencilView() { return mDepthStencilView; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>GetRenderTargetView() { return mRenderTargetView; }

	Microsoft::WRL::ComPtr<ID3D11Device>GetDevice() { return mDevice; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>GetDeviceContext() { return mDeviceContext; }
private:
	//********************************//
	//             変数               //
	//********************************//
	const HWND mHwnd;
	HighResolutionTimer mTimer;

	Microsoft::WRL::ComPtr<ID3D11Device>mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>mDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>mSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDepthStencilView;

	D3D_FEATURE_LEVEL       mLevel;

	static Framework* mInst;

	//********************************//
	//           関数                 //
	//********************************//
	//初期化
	bool Initialize(HWND hwnd);
	//更新
	void Update(float elapsed_time/*Elapsed seconds from last frame*/);
	//描画
	void Render(float elapsed_time/*Elapsed seconds from last frame*/);

	//FPS
	void CalculateFrameStats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		static int frames = 0;
		static float timeTlapsed = 0.0f;

		frames++;

		// Compute averages over one second period.
		if ((mTimer.time_stamp() - timeTlapsed) >= 1.0f)
		{
			float fps = static_cast<float>(frames); // fps = frameCnt / 1
			float mspf = 1000.0f / fps;
			std::ostringstream outs;
			outs.precision(6);
			outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
			SetWindowTextA(mHwnd, outs.str().c_str());

			// Reset for next average.
			frames = 0;
			timeTlapsed += 1.0f;
		}
	}

};