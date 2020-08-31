#include "framework.h"
#include"screen_size.h"
#include"blend_state.h"
#include"scene_manager.h"
#include"camera_manager.h"
#include"gamepad.h"
Framework* Framework::inst = nullptr;

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
		//ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
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
			&level,		// 実際に作成に成功したD3D_FEATURE_LEVELを格納するためのD3D_FEATURE_LEVEL列挙型変数のアドレスを設定する.
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

#endif
	//sprites = std::make_unique<Sprite>(mDevice.Get(), L"Data/image/かめれおんの拝啓.png");
	//primitiveRender = std::make_unique<PrimitiveRender>(mDevice.Get());
	//cube = std::make_unique<GeometricCube>(mDevice.Get());
	//sphere = std::make_unique<GeometricSphere>(mDevice.Get(), 32, 16);
	//cylinder = std::make_unique<GeometricCylinder>(mDevice.Get(), 32);
	//staticMesh = std::make_unique<StaticMesh>(mDevice.Get(), "Data/FBX/source/Dragon_ver2.fbx");
	//meshRender = std::make_unique<MeshRender>(mDevice.Get());
	//std::unique_ptr<ModelData>modelData = std::make_unique<ModelData>("Data/FBX/Deathwing.fbx", true,0);
	//std::shared_ptr<ModelResource>modelResource = std::make_shared<ModelResource>(mDevice.Get(), std::move(modelData));
	//model = std::make_unique<Model>(modelResource);
	//model->PlayAnimation(0, true);
	//modelRenderer = std::make_unique<ModelRenderer>(mDevice.Get());
	//board[0] = std::make_unique<Board>(mDevice.Get(), L"Data/image/koko.png");
	//board[1] = std::make_unique<Board>(mDevice.Get(), L"Data/image/RESULT_ani.png");
	//blend = std::make_unique<blend_state>(mDevice.Get(), BLEND_MODE::ADD);
	//camera = std::make_shared<Camera>();
	//cameraOperation = std::make_unique<CameraOperation>(camera);
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	mDeviceContext.Get()->RSGetViewports(&num_viewports, &viewport);
	pCamera.SetDefaultPerspective(30 * (3.14f / 180.f), viewport.Width / viewport.Height, 0.1f, 10000.0f);
	//camera->SetPerspective(30 * (3.14f / 180.f), viewport.Width / viewport.Height, 0.1f, 1000.0f);
	//cameraOperation->SetCameraType(CameraOperation::CAMERA_TYPE::DEBUG);
	pSceneManager.SetDevice(mDevice.Get());
	pSceneManager.ChangeScene(SCENETYPE::GAME);
	return true;
}
//更新
void Framework::Update(float elapsed_time)
{
	input::GamepadUpdate();
	
	//ImGuiの更新
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	//cameraOperation->Update(elapsed_time);
	//camera->CalculateMatrix();
	//model->UpdateAnimation(elapsed_time);
	//model->CalculateLocalTransform();
	//model->CalculateWorldTransform(DirectX::XMLoadFloat4x4(&CalculateTransform(VECTOR3F(0, 3, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 3.14f, 0))));
	pSceneManager.Update(elapsed_time);
}
//描画
void Framework::Render(float elapsed_time)
{
	float ClearColor[4] = { .0f, .0f, .0f, 1.0f }; //red,green,blue,alpha

	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), ClearColor);
	
	mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
	
	pSceneManager.Render(mDeviceContext.Get(), elapsed_time);
	//FLOAT4X4 view = camera->GetView();
	//DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	//{
	//	//カメラの設定
	//	DirectX::XMVECTOR eye, focus, up;
	//	eye = DirectX::XMVectorSet(0.0f, 0.0f, -200.0f, 1.0f);//視点
	//	focus = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);//注視点
	//	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);//上ベクトル

	//	V = DirectX::XMMatrixLookAtLH(eye, focus, up);

	//	DirectX::XMStoreFloat4x4(&view, V);
	//}

	//FLOAT4X4 projection = camera->GetProjection();
	//DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	//{
	//	//画面サイズ取得のためビューポートを取得
	//	D3D11_VIEWPORT viewport;
	//	UINT num_viewports = 1;
	//	mDeviceContext.Get()->RSGetViewports(&num_viewports, &viewport);

	//	//角度をラジアン(θ)に変換
	//	float fov_y = 30 * (3.14f / 180.f);//角度
	//	float aspect = viewport.Width / viewport.Height;//画面比率
	//	float near_z = 0.1f;//表示最近面までの距離
	//	float far_z = 1000.0f;//表紙最遠面までの距離

	//	P = DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z);

	//	DirectX::XMStoreFloat4x4(&projection, P);
	//}
//	const VECTOR4F light = VECTOR4F(0, 0, 1, 0);
//
//#if TYPE
//	cube->CalculateTransform(VECTOR3F(5, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0));
//	sphere->CalculateTransform(VECTOR3F(0, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0));
//	cylinder->CalculateTransform(VECTOR3F(-5, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0));
//	//sprites->Render(mDeviceContext.Get(), VECTOR2F(0, 0), VECTOR2F(SCREEN_WIDTH, SCREEN_HEIGHT), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
//	primitiveRender->Begin(mDeviceContext.Get(), light);
//	primitiveRender->Render(mDeviceContext.Get(), cube.get(), view, projection);
//	primitiveRender->Render(mDeviceContext.Get(), sphere.get(), view, projection);
//	primitiveRender->Render(mDeviceContext.Get(), cylinder.get(), view, projection);
//	primitiveRender->End(mDeviceContext.Get());
//#else
//	sprites->Render(mDeviceContext.Get(), VECTOR2F(0, 0), VECTOR2F(SCREEN_WIDTH/3.0f, SCREEN_HEIGHT/3.0f), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
//
//	primitiveRender->Begin(mDeviceContext.Get(), light,view,projection);
//	primitiveRender->Render(mDeviceContext.Get(), cube.get(), CalculateTransform(VECTOR3F(5, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0)));
//	primitiveRender->Render(mDeviceContext.Get(), sphere.get(), CalculateTransform(VECTOR3F(0, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0)));
//	primitiveRender->Render(mDeviceContext.Get(), cylinder.get(), CalculateTransform(VECTOR3F(-5, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0)));
//	primitiveRender->End(mDeviceContext.Get());
//
//	meshRender->Begin(mDeviceContext.Get(), light, view, projection);
//	meshRender->Render(mDeviceContext.Get(), staticMesh.get(), CalculateTransform(VECTOR3F(0, -5, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0)));
//	meshRender->End(mDeviceContext.Get());
//
//	FLOAT4X4 view_projection;
//	DirectX::XMMATRIX C = DirectX::XMMatrixSet(
//		1, 0, 0, 0,
//		0, 1, 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1
//	);
//
//	DirectX::XMStoreFloat4x4(&view_projection, C * V * P);
//	modelRenderer->Begin(mDeviceContext.Get(), view_projection, light);
//	modelRenderer->Draw(mDeviceContext.Get(), *model.get());
//	modelRenderer->End(mDeviceContext.Get());
//	static float timer = 0;
//	VECTOR2F texturePosition = VECTOR2F(1920 * static_cast<int>(timer*2.0f), 0);
//	timer += elapsed_time;
//	if (timer >= 4.0f)timer = 0;
//	board[1]->Anim(mDeviceContext.Get(), texturePosition, VECTOR2F(1920, 1080));
//	blend->activate(mDeviceContext.Get());
//	board[0]->Render(mDeviceContext.Get(), VECTOR3F(20, 0, 0), VECTOR3F(10, 10, 10), VECTOR3F(DirectX::XMConvertToRadians(45), 0, 0), view, projection);
//	blend->deactivate(mDeviceContext.Get());
//	board[1]->Render(mDeviceContext.Get(), VECTOR3F(-20, 0, 0), VECTOR3F(10, 10, 10), VECTOR3F(0, 0, 0), view, projection);

//#endif
	//ImGuiの描画
#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	mSwapChain->Present(1, 0);
}

