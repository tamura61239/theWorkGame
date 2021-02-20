#include "title_texture_particle.h"
#include"texture.h"
#include"misc.h"
#include"shader.h"
#include"framework.h"
#include"camera_manager.h"
#include <codecvt>
#include <locale>
#ifdef USE_IMGUI
#include<imgui.h>
#endif


TitleTextureParticle::TitleTextureParticle(ID3D11Device* device) :mFullCreateFlag(false), mParticleFlag(false), mMaxParticle(1920 * 1080), mTestFlag(false)
, mSceneParticleIndex(0), mChangeMaxParticle(0), mTimer(0), mMaxTexture(0), mBeforeParsent(0), mTextureFlag(true)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer>buffer;
	//Buffer
	{
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//パーティクルデータバッファ
		desc.ByteWidth = sizeof(Particle) * mMaxParticle;
		desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
		desc.StructureByteStride = sizeof(Particle);
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//描画用バッファ
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.ByteWidth = sizeof(RenderParticle) * mMaxParticle;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = 0;
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		data.pSysMem = &renderParticles[0];
		hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//定数バッファ
		desc.ByteWidth = sizeof(CbUpdate);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbCreate);

		hr = device->CreateBuffer(&desc, nullptr, mCbCreateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Buffer.NumElements = mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		hr = device->CreateUnorderedAccessView(buffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	mCreateCSShader.resize(2);
	CreateCSFromCso(device, "Data/shader/titile_texture_change_creat_cs.cso", mCreateCSShader[0].GetAddressOf());
	CreateCSFromCso(device, "Data/shader/title_texture_scene_change_cs.cso", mCSShader.GetAddressOf());

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	//mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "", "Data/shader/particle_render_point_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_cube_mesh_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//hr = MakeDummyTexture(device, mParticleSRV.GetAddressOf());
	wchar_t* names[] =
	{
		L"Data/image/○.png",
		L"",
		L"Data/image/無題1.png",
		L"Data/image/無題2.png",
		L"Data/image/無題3.png",
		L"Data/image/無題4.png",
		L"Data/image/無題5.png",
		L"Data/image/無題6.png",
		L"Data/image/無題7.png",
		L"Data/image/無題8.png",
	};
	for (auto& name : names)
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>srv;
		if (wcscmp(name, L"") == 0)
		{
			hr = MakeDummyTexture(device, srv.GetAddressOf());
		}
		else
		{
			hr = LoadTextureFromFile(device, name, srv.GetAddressOf());
		}
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		mParticleSRV.push_back(srv);
	}
	blend = std::make_unique<BlendState>(device, BLEND_MODE::ALPHA);
	mEditorData.scale = 0.00015f;
	mEditorData.screenSplit = 10;
	Load();

}
using convert_t = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_t, wchar_t> strconverter;

std::string to_string(std::wstring wstr)
{
	return strconverter.to_bytes(wstr);
}
void TitleTextureParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("title texture particle");
	ImGui::Checkbox("create", &mFullCreateFlag);
	if (mParticleFlag || mTestFlag)ImGui::Checkbox("move", &mParticleFlag);

	ImGui::Checkbox("test", &mTestFlag);
	ImGui::Checkbox("drow texture", &mTextureFlag);
	ImGui::InputFloat("speed", &mEditorData.speed, 0.1f);
	ImGui::InputFloat("scale", &mEditorData.scale, 0.1f);
	ImGui::InputFloat("screen split", &mEditorData.screenSplit, 0.1f);
	static int num = 0;
	for (int i = 0; i < static_cast<int>(mTextures.size()); i++)
	{
		auto& texture = mTextures[i];
		std::string name = to_string(texture.data.mTextureName);
		ImGui::RadioButton(name.c_str(), &num, i);
	}
	auto& board = boards[num];
	float* position[3] = { &board.position.x,&board.position.y,&board.position.z };
	ImGui::DragFloat3("position", *position, 1);
	ImGui::InputFloat("board scale x", &board.scale.x, 1);
	ImGui::InputFloat("board scale y", &board.scale.y, 1);

	ImVec2 size = ImVec2(75, 75);
	for (UINT i = 0; i < static_cast<UINT>(mParticleSRV.size()); i++)
	{
		if (ImGui::ImageButton(mParticleSRV[i].Get(), size))
		{
			mEditorData.textureType = i;
		}
		if (i % 4 < 3&&i< static_cast<UINT>(mParticleSRV.size()-1))ImGui::SameLine();
	}
	ImGui::Text(u8"今のテクスチャ");
	size = ImVec2(150, 150);
	ImGui::Image(mParticleSRV[mEditorData.textureType].Get(), size);
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::Text("particle:%d", mMoveParticle);
	ImGui::End();
#endif
}

void TitleTextureParticle::LoadTexture(ID3D11Device* device, std::wstring name, const VECTOR2F& leftTop, const VECTOR2F& size, const VECTOR2F& uv, const VECTOR2F& uvSize)
{
	mTextures.emplace_back();
	auto& textute = mTextures.back();
	HRESULT hr = LoadTextureFromFile(device, name.c_str(), textute.mSRV.GetAddressOf());
	textute.data.mLeftTop = leftTop;
	textute.data.mSize = size;
	textute.data.mUVLeftTop = uv;
	textute.data.mUVSize = uvSize;
	textute.data.mTextureName = name;
	if (boards.size() < mTextures.size())
	{
		boards.emplace_back();
		boards.back().position = VECTOR3F(0, 0, 0);
		boards.back().scale = VECTOR3F(0, 0, 0);
	}
	mMaxTexture += uvSize.x;
}
/**************************************************/
//       更新
/**************************************************/
void TitleTextureParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);

	//TitleSceneUpdate(elapsdTime, context);
	SceneChangeUpdate(elapsdTime, context);
	if (mMoveParticle <= 0)return;
	if (mTestFlag && !mParticleFlag)elapsdTime = 0;
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	context->CSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());
	CbUpdate cbUpdate;
	cbUpdate.elapsdTime = elapsdTime;
	cbUpdate.scale = mEditorData.scale;
	cbUpdate.speed = mEditorData.speed;
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &cbUpdate, 0, 0);
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	context->Dispatch(static_cast<UINT>(mMoveParticle / 100), 1, 1);
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11UnorderedAccessView* uav = nullptr;
	context->CSSetShaderResources(0, 1, &srv);
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(2, 1, &uav, nullptr);
	context->CSSetShader(nullptr, nullptr, 0);
	ID3D11Buffer* buffers[] = { nullptr };
	context->CSSetConstantBuffers(0, 1, buffers);

}

//void TitleTextureParticle::TitleSceneUpdate(float elapsdTime, ID3D11DeviceContext* context)
//{
//	if (mParticleFlag|| mFullCreateFlag)return;
//	context->CSSetConstantBuffers(0, 1, mCbCreateBuffer.GetAddressOf());
//
//	context->CSSetShader(mCreateCSShader.Get(), nullptr, 0);
//	FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
//	view._41 = view._42 = view._43 = 0.0f;
//	view._44 = 1.0f;
//
//	for (int i = 0; i < mTextures.size(); i++)
//	{
//		auto& texture = mTextures.at(i);
//		auto& board = boards.at(i);
//		context->CSSetShaderResources(0, 1, texture.SRV.GetAddressOf());
//
//		CbCreate cbCreate;
//		cbCreate.uvSize = texture.data.mUVSize;
//		cbCreate.screenSplit = mEditorData.screenSplit;
//		cbCreate.startIndex = mSceneParticleIndex;
//		float aspect = texture.data.mUVSize.x / texture.data.mUVSize.y;
//		DirectX::XMMATRIX W;
//		{
//			DirectX::XMMATRIX S, T;
//			S = DirectX::XMMatrixScaling(board.scale.x * aspect, board.scale.y, 0);
//			T = DirectX::XMMatrixTranslation(board.position.x, board.position.y, board.position.z);
//			W = S * T;
//
//		}
//
//		DirectX::XMStoreFloat4x4(&cbCreate.mWorld, DirectX::XMLoadFloat4x4(&view) * W);
//		context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &cbCreate, 0, 0);
//		float drowCount = elapsdTime * (static_cast<float>(mEditorData.mSceneMaxParticle) / 2.f);
//
//		mSceneParticleIndex += drowCount;
//		if (mSceneParticleIndex >= mEditorData.mSceneMaxParticle)
//		{
//			drowCount -= mSceneParticleIndex - mEditorData.mSceneMaxParticle;
//			mSceneParticleIndex = 0;
//		}
//		context->Dispatch(drowCount, 1, 1);
//	}
//
//
//	context->CSSetShader(nullptr, nullptr, 0);
//
//}
//シーンが変わる時のパーティクルの生成
void TitleTextureParticle::SceneChangeUpdate(float elapsdTime, ID3D11DeviceContext* context)
{

	if (mFullCreateFlag || mTestFlag)
	{
		if (mFullCreateFlag)mParticleFlag = true;
		if (mTestFlag)mChangeMaxParticle = 0;
		mFullCreateFlag = false;

		context->CSSetConstantBuffers(0, 1, mCbCreateBuffer.GetAddressOf());
		//context->CSSetShader(mCreateCSShader.Get(), nullptr, 0);

		context->CSSetShader(mCreateCSShader[0].Get(), nullptr, 0);
		FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
		view._41 = view._42 = view._43 = 0.0f;
		view._44 = 1.0f;
		int particleCount = 0;
		for (int i = 0; i < static_cast<int>(mTextures.size()); i++)
		{
			auto& texture = mTextures.at(i);
			auto& board = boards.at(i);
			context->CSSetShaderResources(0, 1, texture.mSRV.GetAddressOf());

			CbCreate cbCreate;
			cbCreate.uvSize = texture.data.mUVSize;
			cbCreate.screenSplit = mEditorData.screenSplit;
			cbCreate.startIndex = particleCount;
			float aspect = texture.data.mUVSize.x / texture.data.mUVSize.y;
			DirectX::XMMATRIX W;
			{
				DirectX::XMMATRIX S, T;
				S = DirectX::XMMatrixScaling(board.scale.x * aspect, board.scale.y, 0);
				T = DirectX::XMMatrixTranslation(board.position.x, board.position.y, board.position.z);
				W = S * T;

			}

			DirectX::XMStoreFloat4x4(&cbCreate.world, DirectX::XMLoadFloat4x4(&view) * W);
			context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &cbCreate, 0, 0);
			int newParticle = static_cast<int>(texture.data.mUVSize.x / cbCreate.screenSplit * texture.data.mUVSize.y / cbCreate.screenSplit);
			particleCount += newParticle;

			context->Dispatch(newParticle, 1, 1);
		}
		mChangeMaxParticle += particleCount;
	}
	else if (!mParticleFlag)
	{
		mChangeMaxParticle = 0;
	}
	mMoveParticle = mChangeMaxParticle;

}

void TitleTextureParticle::Create1(float elapsdTime, ID3D11DeviceContext* context)
{
	//if (mTestFlag)
	//{

	//	mMoveParticle = 0;
	//	mTimer = 0;

	//}
	////前の時間を取得
	//float beforeTime = mTimer;
	//mTimer += elapsdTime;
	//float parsent = mTimer / mEditorData.createTime;
	//if (parsent > 1)parsent = 1.f;
	//float textureX = parsent * mMaxTexture;
	//float beforeTextureX = mBeforeParsent * mMaxTexture;
	//int count = 0;
	//float texX = 0;
	//for (auto& texture : mTextures)
	//{
	//	float x = textureX - texture.data.mUVSize.x;
	//	if (x < 0)break;
	//	count++;
	//	texX + texture.data.mUVSize.x;
	//}


	//context->CSSetConstantBuffers(0, 1, mCbCreateBuffer.GetAddressOf());

	//context->CSSetShader(mCreateCSShader[0].Get(), nullptr, 0);
	//FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
	//view._41 = view._42 = view._43 = 0.0f;
	//view._44 = 1.0f;
	//{
	//	auto& texture = mTextures.at(count);
	//	auto& board = boards.at(count);
	//	context->CSSetShaderResources(0, 1, texture.mSRV.GetAddressOf());

	//	CbCreate cbCreate;
	//	cbCreate.leftTop = VECTOR2F(textureX - texX, 0);
	//	cbCreate.uvSize = texture.data.mUVSize;
	//	cbCreate.screenSplit = mEditorData.screenSplit;
	//	cbCreate.startIndex = mMoveParticle;
	//	float aspect = texture.data.mUVSize.x / texture.data.mUVSize.y;
	//	DirectX::XMMATRIX W;
	//	{
	//		DirectX::XMMATRIX S, T;
	//		S = DirectX::XMMatrixScaling(board.scale.x * aspect, board.scale.y, 0);
	//		T = DirectX::XMMatrixTranslation(board.position.x, board.position.y, board.position.z);
	//		W = S * T;

	//	}

	//	DirectX::XMStoreFloat4x4(&cbCreate.world, DirectX::XMLoadFloat4x4(&view) * W);
	//	context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &cbCreate, 0, 0);

	//	mBeforeParsent = parsent;

	//}

}

/****************************************************/
//    描画
/****************************************************/
void TitleTextureParticle::Render(ID3D11DeviceContext* context)
{
	if (mMoveParticle <= 0)return;
	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->Draw(mMoveParticle, 0);
	mShader->Deactivate(context);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);

}


void TitleTextureParticle::Load()
{
	FILE* fp;
	long size = 0;
	if (fopen_s(&fp, "Data/file/title_texture_data1.bin", "rb") == 0)
	{
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(&mEditorData, size, 1, fp);
		fclose(fp);
	}
	if (fopen_s(&fp, "Data/file/title_texture_data2.bin", "rb") == 0)
	{
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int s = size / sizeof(Board);
		boards.resize(s);
		fread(boards.data(), size, 1, fp);
		fclose(fp);
	}

}

void TitleTextureParticle::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/title_texture_data1.bin", "wb");
	fwrite(&mEditorData, sizeof(EditorData), 1, fp);
	fclose(fp);

	fopen_s(&fp, "Data/file/title_texture_data2.bin", "wb");
	fwrite(&boards[0], sizeof(Board) * boards.size(), 1, fp);
	fclose(fp);

}

