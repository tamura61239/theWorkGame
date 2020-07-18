#include "geometric_primitive.h"
#include "misc.h"
#include<memory>

/****************************/
//    オブジェクトクラス
/***************************/
void GeometricPrimitive::CreateBuffer(ID3D11Device* device, std::vector<Vertex> vertics, std::vector<UINT> index)
{
	HRESULT hr = S_OK;
	indexNum = static_cast<int>(index.size());
	// 頂点バッファの生成
	{
		// 頂点バッファを作成するための設定オプション
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertics.size());
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		// 頂点バッファに頂点データを入れるための設定
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = vertics.data();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&desc, &data, mVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// インテックスバッファの生成
	{
		// インテックスバッファを作成するための設定オプション
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * index.size());
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		// インテックスバッファに頂点データを入れるための設定
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = index.data();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&desc, &data, mIndexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
}
//キューブ
GeometricCube::GeometricCube(ID3D11Device* device) : GeometricPrimitive(device)
{
	// 頂点データの定義
    // 0           1
    // +-----------+
    // |           |
    // |           |
    // +-----------+
    // 2           3
	std::vector<Vertex>vertices;
	std::vector<UINT>indices;
	Vertex vertex[] = {
		//手前の面(赤)
		{ VECTOR3F(-0.5, +0.5, -0.5), VECTOR3F(0, 0, -1) },//0
		{ VECTOR3F(+0.5, +0.5, -0.5), VECTOR3F(0, 0, -1) },//1
		{ VECTOR3F(-0.5, -0.5, -0.5), VECTOR3F(0, 0, -1) },//2
		{ VECTOR3F(+0.5, -0.5, -0.5), VECTOR3F(0, 0, -1) },//3
																			 //奥の面(緑)											   3
		{ VECTOR3F(-0.5, +0.5, +0.5), VECTOR3F(0, 0, 1) },//4
		{ VECTOR3F(+0.5, +0.5, +0.5), VECTOR3F(0, 0, 1) },//5
		{ VECTOR3F(-0.5, -0.5, +0.5), VECTOR3F(0, 0, 1) },//6
		{ VECTOR3F(+0.5, -0.5, +0.5), VECTOR3F(0, 0, 1) },//7
																			//右の面(青)											   3
		{ VECTOR3F(+0.5, +0.5, -0.5), VECTOR3F(1, 0, 0) },
		{ VECTOR3F(+0.5, +0.5, +0.5), VECTOR3F(1, 0, 0) },
		{ VECTOR3F(+0.5, -0.5, -0.5), VECTOR3F(1, 0, 0) },
		{ VECTOR3F(+0.5, -0.5, +0.5), VECTOR3F(1, 0, 0) },
		//左の面(紫)											   3
		{ VECTOR3F(-0.5, +0.5, -0.5), VECTOR3F(-1, 0, 0) },
		{ VECTOR3F(-0.5, +0.5, +0.5), VECTOR3F(-1, 0, 0) },
		{ VECTOR3F(-0.5, -0.5, -0.5), VECTOR3F(-1, 0, 0) },
		{ VECTOR3F(-0.5, -0.5, +0.5), VECTOR3F(-1, 0, 0) },
		//上の面(水色)											  3
		{ VECTOR3F(-0.5, +0.5, +0.5), VECTOR3F(0, 1, 0) },
		{ VECTOR3F(+0.5, +0.5, +0.5), VECTOR3F(0, 1, 0) },
		{ VECTOR3F(-0.5, +0.5, -0.5), VECTOR3F(0, 1, 0) },
		{ VECTOR3F(+0.5, +0.5, -0.5), VECTOR3F(0, 1, 0) },
		//下の面(白)											   3
		{ VECTOR3F(-0.5, -0.5, +0.5), VECTOR3F(0, -1, 0) },
		{ VECTOR3F(+0.5, -0.5, +0.5), VECTOR3F(0, -1, 0) },
		{ VECTOR3F(-0.5, -0.5, -0.5), VECTOR3F(0, -1, 0) },
		{ VECTOR3F(+0.5, -0.5, -0.5), VECTOR3F(0, -1, 0) },
	};
	for (int i = 0;i < 24;i++)
	{
		vertices.push_back(vertex[i]);
	}
	unsigned int index[] = {
		//手前の面(時計回り)
		0,1,2,
		2,1,3,
		//奥の面(反時計回り)
		5,4,7,
		7,4,6,
		//右の面(時計回り)
		8,9,10,
		10,9,11,
		//左の面(反時計回り)
		12,14,13,
		13,14,15,
		//上の面(時計回り)
		16,17,18,
		18,17,19,
		//下の面(反時計回り)
		20,22,21,
		21,22,23,
	};
	for (int i = 0;i < 36;i++)
	{
		indices.push_back(index[i]);
	}
	CreateBuffer(device, vertices, indices);
}
//円柱
GeometricCylinder::GeometricCylinder(ID3D11Device* device, u_int slices) : GeometricPrimitive(device)
{
	std::vector<Vertex> vertices;
	std::vector<u_int> indices;

	float d = 2.0f * DirectX::XM_PI / slices;
	float r = 0.5f;

	Vertex vertex;
	u_int base_index = 0;

	// top cap centre
	vertex.position = VECTOR3F(0.0f, +0.5f, 0.0f);
	vertex.normal = VECTOR3F(0.0f, +1.0f, 0.0f);
	vertices.push_back(vertex);
	// top cap ring
	for (u_int i = 0; i < slices; ++i)
	{
		float x = r * cosf(i * d);
		float z = r * sinf(i * d);
		vertex.position = VECTOR3F(x, +0.5f, z);
		vertex.normal = VECTOR3F(0.0f, +1.0f, 0.0f);
		vertices.push_back(vertex);
	}
	base_index = 0;
	for (u_int i = 0; i < slices - 1; ++i)
	{
		indices.push_back(base_index + 0);
		indices.push_back(base_index + i + 2);
		indices.push_back(base_index + i + 1);
	}
	indices.push_back(base_index + 0);
	indices.push_back(base_index + 1);
	indices.push_back(base_index + slices);

	// bottom cap centre
	vertex.position = VECTOR3F(0.0f, -0.5f, 0.0f);
	vertex.normal = VECTOR3F(0.0f, -1.0f, 0.0f);
	vertices.push_back(vertex);
	// bottom cap ring
	for (u_int i = 0; i < slices; ++i)
	{
		float x = r * cosf(i * d);
		float z = r * sinf(i * d);
		vertex.position = VECTOR3F(x, -0.5f, z);
		vertex.normal = VECTOR3F(0.0f, -1.0f, 0.0f);
		vertices.push_back(vertex);
	}
	base_index = slices + 1;
	for (u_int i = 0; i < slices - 1; ++i)
	{
		indices.push_back(base_index + 0);
		indices.push_back(base_index + i + 1);
		indices.push_back(base_index + i + 2);
	}
	indices.push_back(base_index + 0);
	indices.push_back(base_index + (slices - 1) + 1);
	indices.push_back(base_index + (0) + 1);

	// side rectangle
	for (u_int i = 0; i < slices; ++i)
	{
		float x = r * cosf(i * d);
		float z = r * sinf(i * d);

		vertex.position = VECTOR3F(x, +0.5f, z);
		vertex.normal = VECTOR3F(x, 0.0f, z);
		vertices.push_back(vertex);

		vertex.position = VECTOR3F(x, -0.5f, z);
		vertex.normal = VECTOR3F(x, 0.0f, z);
		vertices.push_back(vertex);
	}
	base_index = slices * 2 + 2;
	for (u_int i = 0; i < slices - 1; ++i)
	{
		indices.push_back(base_index + i * 2 + 0);
		indices.push_back(base_index + i * 2 + 2);
		indices.push_back(base_index + i * 2 + 1);

		indices.push_back(base_index + i * 2 + 1);
		indices.push_back(base_index + i * 2 + 2);
		indices.push_back(base_index + i * 2 + 3);
	}
	indices.push_back(base_index + (slices - 1) * 2 + 0);
	indices.push_back(base_index + (0) * 2 + 0);
	indices.push_back(base_index + (slices - 1) * 2 + 1);

	indices.push_back(base_index + (slices - 1) * 2 + 1);
	indices.push_back(base_index + (0) * 2 + 0);
	indices.push_back(base_index + (0) * 2 + 1);
	CreateBuffer(device, vertices, indices);
}
//球
GeometricSphere::GeometricSphere(ID3D11Device* device, u_int slices, u_int stacks) : GeometricPrimitive(device)
{
	std::vector<Vertex> vertices;
	std::vector<u_int> indices;


	float r = 0.5f;		//	半径 0.5f = 直径 1.0f

						//
						// Compute the vertices stating at the top pole and moving down the stacks.
						//

						// Poles: note that there will be texture coordinate distortion as there is
						// not a unique point on the texture map to assign to the pole when mapping
						// a rectangular texture onto a sphere.
	Vertex top_vertex;
	top_vertex.position = VECTOR3F(0.0f, +r, 0.0f);
	top_vertex.normal = VECTOR3F(0.0f, +1.0f, 0.0f);

	Vertex bottom_vertex;
	bottom_vertex.position = VECTOR3F(0.0f, -r, 0.0f);
	bottom_vertex.normal = VECTOR3F(0.0f, -1.0f, 0.0f);

	vertices.push_back(top_vertex);

	float phi_step = DirectX::XM_PI / stacks;
	float theta_step = 2.0f * DirectX::XM_PI / slices;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (u_int i = 1; i <= stacks - 1; ++i)
	{
		float phi = i * phi_step;
		float rs_phi = r * sinf(phi), rc_phi = r * cosf(phi);

		// Vertices of ring.
		for (u_int j = 0; j <= slices; ++j)
		{
			float theta = j * theta_step;

			Vertex v;

			// spherical to cartesian
			v.position.x = rs_phi * cosf(theta);
			v.position.y = rc_phi;
			v.position.z = rs_phi * sinf(theta);

			DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottom_vertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//
	for (UINT i = 1; i <= slices; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	u_int base_index = 1;
	u_int ring_vertex_count = slices + 1;
	for (u_int i = 0; i < stacks - 2; ++i)
	{
		u_int i_rvc = i * ring_vertex_count;
		u_int i1_rvc = (i + 1) * ring_vertex_count;

		for (u_int j = 0; j < slices; ++j)
		{
			indices.push_back(base_index + i_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j);

			indices.push_back(base_index + i1_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	u_int south_pole_index = (u_int)vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	base_index = south_pole_index - ring_vertex_count;

	for (u_int i = 0; i < slices; ++i)
	{
		indices.push_back(south_pole_index);
		indices.push_back(base_index + i);
		indices.push_back(base_index + i + 1);
	}
	CreateBuffer(device, vertices, indices);
}
/************************************/
//       描画クラス
/***********************************/
PrimitiveRender::PrimitiveRender(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	// 定数バッファの生成
	{
		// 定数バッファを作成するための設定オプション
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(CbScene);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		hr = device->CreateBuffer(&desc, nullptr,mCbSceneBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		desc.ByteWidth = sizeof(CbObj);
		hr = device->CreateBuffer(&desc, nullptr, mCbObjBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//頂点シェーダー＆頂点入力レイアウトの生成
	{
		// 頂点シェーダーファイルを開く(sprite_vs.hlsl をコンパイルしてできたファイル)
		FILE* fp = nullptr;
		fopen_s(&fp, "Data/shader/geometric_primitive_vs.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// 頂点シェーダーファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		std::unique_ptr<u_char[]> cso_data = std::make_unique<u_char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		// 頂点シェーダーデータを基に頂点シェーダーオブジェクトを生成する
		hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, mVSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// GPUに頂点データの内容を教えてあげるための設定
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 入力レイアウトの生成
		hr = device->CreateInputLayout(
			input_element_desc,				// 頂点データの内容
			ARRAYSIZE(input_element_desc),	// 頂点データの要素数
			cso_data.get(),						// 頂点シェーダーデータ（input_element_descの内容と sprite_vs.hlslの内容に不一致がないかチェックするため）
			cso_sz,							// 頂点シェーダーデータサイズ
			mInput.GetAddressOf()					// 入力レイアウトオブジェクトのポインタの格納先。
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// ピクセルシェーダーの生成
	{
		// ピクセルシェーダーファイルを開く(sprite_ps.hlsl をコンパイルしてできたファイル)
		FILE* fp = nullptr;
		fopen_s(&fp, "Data/shader/geometric_primitive_ps.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ピクセルシェーダーファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		std::unique_ptr<u_char[]> cso_data = std::make_unique<u_char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		// ピクセルシェーダーの生成
		hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, mPSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// ラスタライザステートの生成
	{
		// ラスタライザステートを作成するための設定オプション
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = false;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;

		hr = device->CreateRasterizerState(&desc, mRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// 深度ステンシルステートの設定
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}
#if TYPE
void PrimitiveRender::Begin(ID3D11DeviceContext* context, const VECTOR4F& light)
{
	context->VSSetShader(mVSShader.Get(), nullptr, 0);
	context->PSSetShader(mPSShader.Get(), nullptr, 0);
	context->IASetInputLayout(mInput.Get());

	ID3D11Buffer* constantBuffer[] =
	{
		mCbSceneBuffer.Get(),
		mCbObjBuffer.Get(),
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffer), constantBuffer);
	//context->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffer), constantBuffer);
	context->RSSetState(mRasterizerState.Get());

	CbScene cbScene;
	cbScene.lightDirection = light;
	context->UpdateSubresource(mCbSceneBuffer.Get(), 0, 0, &cbScene,0,0);
}

void PrimitiveRender::Render(ID3D11DeviceContext* context, GeometricPrimitive* obj, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR4F& color)
{
	CbObj cbObj;
	cbObj.world = obj->GetWorld();

	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&cbObj.world);
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX WVP = W*V*P;
	DirectX::XMStoreFloat4x4(&cbObj.worldViewProjection, WVP);
	cbObj.color = color;
	context->UpdateSubresource(mCbObjBuffer.Get(), 0, 0, &cbObj,0,0);

	UINT stride = sizeof(GeometricPrimitive::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, obj->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(obj->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	int num = obj->GetIndexNum();
	context->DrawIndexed(num, 0, 0);
}
#else
void PrimitiveRender::Begin(ID3D11DeviceContext* context, const VECTOR4F& light, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	context->VSSetShader(mVSShader.Get(), nullptr, 0);
	context->PSSetShader(mPSShader.Get(), nullptr, 0);
	context->IASetInputLayout(mInput.Get());

	ID3D11Buffer* constantBuffer[] =
	{
		mCbSceneBuffer.Get(),
		mCbObjBuffer.Get(),
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffer), constantBuffer);
	//context->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffer), constantBuffer);
	context->RSSetState(mRasterizerState.Get());
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	CbScene cbScene;
	cbScene.lightDirection = light;
	cbScene.view = view;
	cbScene.projection = projection;
	context->UpdateSubresource(mCbSceneBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

void PrimitiveRender::Render(ID3D11DeviceContext* context, GeometricPrimitive* obj, const FLOAT4X4& world, const VECTOR4F& color)
{
	CbObj cbObj;
	cbObj.world = world;
	cbObj.color = color;
	context->UpdateSubresource(mCbObjBuffer.Get(), 0, 0, &cbObj, 0, 0);

	UINT stride = sizeof(GeometricPrimitive::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, obj->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(obj->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	int num = obj->GetIndexNum();
	context->DrawIndexed(num, 0, 0);
}

#endif
void PrimitiveRender::End(ID3D11DeviceContext* context)
{
}
