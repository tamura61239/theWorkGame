#include "title_texture_particle.h"
#include"texture.h"
#include"misc.h"
#include"shader.h"
#include"framework.h"
#include"camera_manager.h"
#include <codecvt>
#include <locale>
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

TitleTextureParticle::TitleTextureParticle(ID3D11Device* device) :mFullCreateFlag(false), mParticleFlag(false), mMaxParticle(1920 * 1080), mTestFlag(false)
, mChangeMaxParticle(0), mMaxTexture(0), mTextureFlag(true)
{
	HRESULT hr;
	{
		//パーティクルのバッファ
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particles, true);
		//描画用バッファ
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
	}
	//定数バッファ生成
	mCbCreate = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdate = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	//コンピュートシェーダーの生成
	mCreateCSShader.resize(2);
	CreateCSFromCso(device, "Data/shader/titile_texture_change_creat_cs.cso", mCreateCSShader[0].GetAddressOf());
	CreateCSFromCso(device, "Data/shader/title_texture_scene_change_cs.cso", mCSShader.GetAddressOf());
	//描画用テクスチャの生成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//描画用テクスチャの生成
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
	//エディタデータの初期化
	mEditorData.scale = 0.00015f;
	mEditorData.screenSplit = 10;
	//ファイルからデータをロード
	Load();
}
/************************wstring型をstring型に変換する関数****************/
using convert_t = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_t, wchar_t> strconverter;

std::string to_string(std::wstring wstr)
{
	return strconverter.to_bytes(wstr);
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

void TitleTextureParticle::Editor()
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
/***************************パーティクルを生成するのに使うテクスチャの読み込み関数********************************/
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
	mMaxTexture += static_cast<UINT>(uvSize.x);
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
/**************************パーティクルの更新***************************/
void TitleTextureParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	//パーティクルのデータをGPU側に送る
	mParticle->Activate(context, 0, true);
	SceneChangeUpdate(elapsdTime, context);
	if (mMoveParticle <= 0)
	{
		mParticle->DeActivate(context);
		return;
	}
	if (mTestFlag && !mParticleFlag)elapsdTime = 0;
	//パーティクルのデータをGPU側に送る
	mParticleRender->Activate(context, 2, true);

	//シェーダーを設定
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	//定数バッファのデータを更新
	mCbUpdate->data.elapsdTime = elapsdTime;
	mCbUpdate->data.scale = mEditorData.scale;
	mCbUpdate->data.speed = mEditorData.speed;
	//定数バッファをGPU側に送る
	mCbUpdate->Activate(context, 1, false, false, false, true);
	//更新
	context->Dispatch(static_cast<UINT>(mMoveParticle / 100), 1, 1);

	//GPU側に送ったデータを元に戻す
	mParticle->DeActivate(context);
	mParticleRender->DeActivate(context);
	mCbUpdate->DeActivate(context);
	context->CSSetShader(nullptr, nullptr, 0);

}

/************************シーンが変更されるときのパーティクル生成*****************************/
void TitleTextureParticle::SceneChangeUpdate(float elapsdTime, ID3D11DeviceContext* context)
{
	//テストプレイフラグがONになってるか生成フラグがONになっているとき
	if (mFullCreateFlag || mTestFlag)
	{
		//どのフラグがONになっているのか調べてその結果で処理を変えている
		if (mFullCreateFlag)mParticleFlag = true;
		if (mTestFlag)mChangeMaxParticle = 0;
		mFullCreateFlag = false;
		//シェーダーを設定
		context->CSSetShader(mCreateCSShader[0].Get(), nullptr, 0);
		//view行列を取得
		FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
		view._41 = view._42 = view._43 = 0.0f;
		view._44 = 1.0f;
		int particleCount = 0;
		//テクスチャ(板ポリ)の分だけパーティクルの生成処理をする
		for (int i = 0; i < static_cast<int>(mTextures.size()); i++)
		{
			auto& texture = mTextures.at(i);
			auto& board = boards.at(i);
			//テクスチャをGPU側に渡す
			context->CSSetShaderResources(0, 1, texture.mSRV.GetAddressOf());
			//定数バッファのデータを更新
			mCbCreate->data.uvSize = texture.data.mUVSize;
			mCbCreate->data.screenSplit = mEditorData.screenSplit;
			mCbCreate->data.startIndex = particleCount;
			float aspect = texture.data.mUVSize.x / texture.data.mUVSize.y;
			DirectX::XMMATRIX W;
			{
				DirectX::XMMATRIX S, T;
				S = DirectX::XMMatrixScaling(board.scale.x * aspect, board.scale.y, 0);
				T = DirectX::XMMatrixTranslation(board.position.x, board.position.y, board.position.z);
				W = S * T;

			}

			DirectX::XMStoreFloat4x4(&mCbCreate->data.world, DirectX::XMLoadFloat4x4(&view) * W);
			//定数バッファをGPU側に送る
			mCbCreate->Activate(context, 0, false, false, false, true);
			int newParticle = static_cast<int>(texture.data.mUVSize.x / mCbCreate->data.screenSplit * texture.data.mUVSize.y / mCbCreate->data.screenSplit);
			//パーティクルの数を増やす
			particleCount += newParticle;
			//生成
			context->Dispatch(newParticle, 1, 1);
			//定数バッファを元に戻す
			mCbCreate->DeActivate(context);

		}
		//生成した分をパーティクル数に追加
		mChangeMaxParticle += particleCount;
	}
	else if (!mParticleFlag)
	{
		mChangeMaxParticle = 0;
	}
	mMoveParticle = mChangeMaxParticle;

}


/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
void TitleTextureParticle::Render(ID3D11DeviceContext* context)
{
	if (mMoveParticle <= 0)return;
	//シェーダーを設定
	mShader->Activate(context);
	//データをGPU側に送る
	ID3D11Buffer* buffer = mParticleRender->GetBuffer();
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//描画
	context->Draw(mMoveParticle, 0);
	//シェーダーを元に戻す
	mShader->Deactivate(context);
	//GPU側に送ったデータを元に戻す
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
	buffer = nullptr;
	stride = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

}


void TitleTextureParticle::Load()
{
	FILE* fp;
	long size = 0;
	FileFunction::Load(mEditorData, "Data/file/title_texture_data1.bin", "rb");
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
	FileFunction::Save(mEditorData, "Data/file/title_texture_data1.bin", "wb");

	fopen_s(&fp, "Data/file/title_texture_data2.bin", "wb");
	fwrite(&boards[0], sizeof(Board) * boards.size(), 1, fp);
	fclose(fp);

}

