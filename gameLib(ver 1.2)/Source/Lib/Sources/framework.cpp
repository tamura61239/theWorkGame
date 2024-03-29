#include "framework.h"
#include"screen_size.h"
#include"blend_state.h"
#include"scene_manager.h"
#include"camera_manager.h"
#include"stage_manager.h"
#include"gamepad.h"
#include<time.h>
#include"scene_select.h"
#include"scene_game.h"
Framework* Framework::mInst = nullptr;
/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/

bool Framework::Initialize(HWND hwnd)
{
	HRESULT hr;

	//SwapChainとDevice,Devicecontextの生成
	{
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};


		DXGI_SWAP_CHAIN_DESC    desc = {};
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		desc.BufferCount = 1;					// バッファの数
		desc.BufferDesc.Width = SCREEN_WIDTH;	// バッファの横幅
		desc.BufferDesc.Height = SCREEN_HEIGHT;	// バッファの縦幅
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// カラーフォーマット
		desc.BufferDesc.RefreshRate.Numerator = 60;		// リフレッシュレートの分母
		desc.BufferDesc.RefreshRate.Denominator = 1;		// リフレッシュレートの分子
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;		// バックバッファの数
		desc.OutputWindow = hwnd;	// DirectXで描いた画を表示するウインドウ
		desc.Windowed = TRUE;		// ウインドウモードか、フルスクリーンにするか。
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,	// どのビデオアダプタを使用するか？既定ならばnullptrで、IDXGIAdapterのアドレスを渡す.
			D3D_DRIVER_TYPE_HARDWARE,	// ドライバのタイプを渡す。これ以外は基本的にソフトウェア実装で、どうしてもという時やデバグ用に用いるべし.
			nullptr,	// 上記をD3D_DRIVER_TYPE_SOFTWAREに設定した際に、その処理を行うDLLのハンドルを渡す。それ以外を指定している際には必ずnullptrを渡す.
			0,			// 何らかのフラグを指定する。詳しくはD3D11_CREATE_DEVICE列挙型で検索検索ぅ.
			featureLevels,	// 実はここでD3D_FEATURE_LEVEL列挙型の配列を与える。nullptrにすることで上記featureと同等の内容の配列が使用される.
			ARRAYSIZE(featureLevels),			// 上記引数で、自分で定義した配列を与えていた場合、その配列の要素数をここに記述する.
			D3D11_SDK_VERSION,	// SDKのバージョン。必ずこの値.
			&desc,		// DXGI_SWAP_CHAIN_DESC構造体のアドレスを設定する。ここで設定した構造愛に設定されているパラメータでSwapChainが作成される.
			&mSwapChain,	// 作成が成功した場合に、そのSwapChainのアドレスを格納するポインタ変数へのアドレス。ここで指定したポインタ変数経由でSwapChainを操作する.
			&mDevice,	// 上記とほぼ同様で、こちらにはDeviceのポインタ変数のアドレスを設定する.
			&mLevel,		// 実際に作成に成功したD3D_FEATURE_LEVELを格納するためのD3D_FEATURE_LEVEL列挙型変数のアドレスを設定する.
			&mDeviceContext	// SwapChainやDeviceと同様に、こちらにはContextのポインタ変数のアドレスを設定する.
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//RenderTargetViewの生成
	{
		ID3D11Texture2D* texture2D;

		hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&texture2D);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = mDevice->CreateRenderTargetView(texture2D, nullptr, &mRenderTargetView);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//DepthStencilViewの生成
	{
		ID3D11Texture2D* texture2D;
		D3D11_TEXTURE2D_DESC desc;

		desc.Width = SCREEN_WIDTH;
		desc.Height = SCREEN_HEIGHT;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = mDevice->CreateTexture2D(&desc, nullptr, &texture2D);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = mDevice->CreateDepthStencilView(texture2D, nullptr, &mDepthStencilView);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//viewportの設定
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(SCREEN_WIDTH);
		viewport.Height = static_cast<float>(SCREEN_HEIGHT);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		mDeviceContext->RSSetViewports(1, &viewport);

	}
	//ImGuiの初期化
#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(mDevice.Get(), mDeviceContext.Get());

	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); /*(void)io;*/

	io.Fonts->AddFontFromFileTTF("Data\\ipag.ttf", 17.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

#endif
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	mDeviceContext.Get()->RSGetViewports(&num_viewports, &viewport);
	//シーンの初期化
	CameraManager::Create();
	pCameraManager->SetDefaultPerspective(30 * (3.14f / 180.f), viewport.Width / viewport.Height, 0.1f, 20000);
	pSceneManager.Initialize(mDevice.Get());
	StageManager::StageCount();
	return true;
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void Framework::Update(float elapsed_time)
{
	input::GamepadUpdate();
	
	//ImGuiの更新
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	//rand関数の更新
	srand((unsigned int)time(NULL));
	//シーンの更新
	pSceneManager.Update(elapsed_time);
}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
void Framework::Render(float elapsed_time)
{
	float ClearColor[4] = { .0f, .0f, .0f, 1.0f }; //red,green,blue,alpha
	//レンダーターゲットなどの初期化
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), ClearColor);
	
	mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
	//シーンの描画
	pSceneManager.Render(mDeviceContext.Get(), elapsed_time);
	//ImGuiの描画
#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
	//fps固定かどうかを決める
	mSwapChain->Present(0, 0);
}

