#include "select_scene_particle.h"
#include"misc.h"
#include"shader.h"
#include"camera_manager.h"
#include"texture.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
SelectSceneParticle::SelectSceneParticle(ID3D11Device* device) :mIndexCount(0), mRenderCount(0)
{
	HRESULT hr;

	mMaxParticle = 60000;
	memset(&mEditorData, 0, sizeof(mEditorData));

	{
		//パーティクルのバッファ
		std::vector<Particle>particles;
		particles.resize(static_cast<size_t>(mMaxParticle));
		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particles, true);
		//描画用バッファ
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(static_cast<size_t>(mMaxParticle));
		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
		//indexバッファ
		std::vector<UINT>indices;
		indices.resize(static_cast<size_t>(mMaxParticle));
		for (auto& index : mParticleIndices)
		{
			index = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, indices, true);
		}
		//deleteIndexバッファ
		for (int i = 0; i < mMaxParticle; i++)
		{
			indices[i] = i;
		}
		mParticleDeleteIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, indices, true);
		//パーティクルのカウントバッファ
		ParticleCount particleCount;
		memset(&particleCount, 0, sizeof(particleCount));
		particleCount.deActiveParticleCount = static_cast<UINT>(mMaxParticle);
		mParticleCount = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particleCount, true, false, D3D11_CPU_ACCESS_READ);
	}

	//定数バッファ
	mCbCreate = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdate = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	//描画用のテクスチャの生成
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

	//コンピュートシェーダーの生成
	hr = CreateCSFromCso(device, "Data/shader/select_scene_create_particle_cs.cso", mCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/select_scene_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_count_cs.cso", mCSEndShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//描画用のシェーダーの生成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//データの設定
	mEditorData.angleMovement = VECTOR3F(3.14f / 2.f, 3.14f, 0);
	mEditorData.color = VECTOR4F(0, 1, 0.5f, 1);
	mEditorData.range = 200;
	mEditorData.scope = VECTOR3F(1, 1, 0.3f);
	mEditorData.speed = 200;
	mEditorData.endPosition = VECTOR3F(0, 0, 1000);
	mEditorData.defVelocity = VECTOR3F(0, 0, 1);
	mEditorData.sinLeng = 10;
	FileFunction::Load(mEditorData, "Data/file/selete_scene_particle_data.bin", "rb");
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void SelectSceneParticle::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("select scene particle");
	//パラメーターを調整する
	ImGui::InputFloat("speed", &mEditorData.speed, 0.5f);
	ImGui::InputFloat("range", &mEditorData.range, 1);
	float* color[4] = { &mEditorData.color.x,&mEditorData.color.y,&mEditorData.color.z,&mEditorData.color.w };
	ImGui::ColorEdit4("def color", *color);
	float* scope[3] = { &mEditorData.scope.x,&mEditorData.scope.y,&mEditorData.scope.z };
	ImGui::SliderFloat3("scope", *scope, 0, 1);
	float* angleMovement[3] = { &mEditorData.angleMovement.x,&mEditorData.angleMovement.y ,&mEditorData.angleMovement.z };
	ImGui::SliderFloat3("def angleMovement", *angleMovement, -3.14f, 3.14f);
	ImGui::InputFloat("sinLeng", &mEditorData.sinLeng, 0.1f);
	//描画に使うテクスチャを選択する
	ImVec2 size = ImVec2(75, 75);
	for (UINT i = 0; i < static_cast<UINT>(mParticleSRV.size()); i++)
	{
		if (ImGui::ImageButton(mParticleSRV[i].Get(), size))
		{
			mEditorData.textureType = i;
		}
		if (i % 4 < 3 && i < static_cast<UINT>(mParticleSRV.size() - 1))ImGui::SameLine();
	}
	//今選択してるテクスチャの表示
	ImGui::Text(u8"今のテクスチャ");
	size = ImVec2(150, 150);
	ImGui::Image(mParticleSRV[mEditorData.textureType].Get(), size);
	//セーブ
	if (ImGui::Button("save"))
	{
		FileFunction::Save(mEditorData, "Data/file/selete_scene_particle_data.bin", "wb");

	}
	ImGui::Text("%f", newIndex);
	ImGui::End();
#endif

}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void SelectSceneParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	//GPU側にパーティクルにデータを送る
	mParticle->Activate(context, 0, true);
	mParticleCount->Activate(context, 1, true);
	mParticleRender->Activate(context, 2, true);
	mParticleIndices[mIndexCount]->Activate(context, 3, true);
	mParticleIndices[1 - mIndexCount]->Activate(context, 4, true);
	mParticleDeleteIndex->Activate(context, 5, true);
	//定数バッファのデータ更新
	mCbCreate->data.angleMovement = mEditorData.angleMovement;
	mCbCreate->data.color = mEditorData.color;
	mCbCreate->data.range = mEditorData.range;
	mCbCreate->data.scope = mEditorData.scope;
	mCbCreate->data.speed = mEditorData.speed;
	mCbCreate->data.sinLeng = mEditorData.sinLeng;
	mCbUpdate->data.endPosition = mEditorData.endPosition;
	mCbUpdate->data.defVelocity = mEditorData.defVelocity;

	mIndexCount++;
	if (mIndexCount >= 2)mIndexCount = 0;
	//生成する数の更新
	newIndex += elapsdTime * 2000;
	//パーティクルの生成
	if (newIndex >= 1)
	{
		mCbCreate->data.eye = pCameraManager->GetCamera()->GetEye();
		mCbCreate->data.eye.y = 0;
		//シェーダーの設定
		context->CSSetShader(mCreateShader.Get(), nullptr, 0);
		//定数バッファをGPU側に送る
		mCbCreate->Activate(context, 0, false, false, false, true);
		//小数点以下切り捨て
		UINT count = static_cast<UINT>(newIndex);
		//更新
		context->Dispatch(count, 1, 1);
		//送ったデータを元に戻す
		mCbCreate->DeActivate(context);
		//生成した分だけ引いとく
		newIndex -= static_cast<float>(count);
	}
	//パーティクルの更新
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	mCbUpdate->data.elapsdTime = elapsdTime;
	mCbUpdate->Activate(context, 1, false, false, false, true);
	context->Dispatch(static_cast<UINT>(mMaxParticle * 0.01f), 1, 1);
	mCbUpdate->DeActivate(context);
	//パーティクルカウントの更新
	context->CSSetShader(mCSEndShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);
	//カウントのバッファから生きてる分のカウントを取得
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mParticleCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mRenderCount = particleCount->aliveParticleCount;
	context->Unmap(mParticleCount->GetBuffer(), NULL);
	//シェーダーの解除
	context->CSSetShader(nullptr, nullptr, 0);
	//GPU側に送ったデータを元に戻す
	mParticle->DeActivate(context);
	mParticleCount->DeActivate(context);
	mParticleRender->DeActivate(context);
	mParticleIndices[mIndexCount]->DeActivate(context);
	mParticleIndices[1 - mIndexCount]->DeActivate(context);
	mParticleDeleteIndex->DeActivate(context);

}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

void SelectSceneParticle::Render(ID3D11DeviceContext* context)
{
	//シェーダーの設定
	mShader->Activate(context);
	//GPU側にデータを送る
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* vertex = mParticleRender->GetBuffer();
	ID3D11Buffer* index = mParticleIndices[mIndexCount]->GetBuffer();
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//描画
	context->DrawIndexed(mRenderCount, 0, 0);
	//送ったデータを元に戻す
	mShader->Deactivate(context);
	offset = 0;
	vertex = nullptr;
	index = nullptr;
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);

}

