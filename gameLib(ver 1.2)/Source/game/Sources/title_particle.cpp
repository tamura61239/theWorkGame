#include "title_particle.h"
#include"misc.h"
#include"texture.h"

#include"shader.h"
#include"camera_manager.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

TitleParticle::TitleParticle(ID3D11Device* device) :mSceneChange(false), mStartIdex(0)
{
	::memset(&mEditorData, 0, sizeof(EditorData));
	mMaxParticle = 1000000;
	HRESULT hr;
	{
		//パーティクルのバッファ
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particles, true);
		//描画用バッファ
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
		//indexバッファ
		std::vector<UINT>indices;
		indices.resize(mMaxParticle);
		for (int i = 0; i < 2; i++)mParticleIndexs[i] = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, indices, true);
		//deleteindexバッファ
		for (int i = 0; i < mMaxParticle; i++)
		{
			indices[i] = i;
		}
		mParticleDeleteIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, indices, true);
		//カウントバッファ
		ParticleCount particleCount;
		memset(&particleCount, 0, sizeof(particleCount));
		particleCount.deActiveParticleCount = mMaxParticle;
		mParticleCount = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particleCount, true, false, D3D11_CPU_ACCESS_READ);

	}
	//定数バッファ生成
	mCbStart = std::make_unique<ConstantBuffer<CbStart>>(device);
	mCbStart2 = std::make_unique<ConstantBuffer<CbStart2>>(device);
	mCb = std::make_unique<ConstantBuffer<Cb>>(device);
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
	//コンピュートシェーダーの生成
	hr = CreateCSFromCso(device, "Data/shader/title_particle_create_cs.cso", mCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/title_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/title_scene_change_particle_cs.cso", mSceneChangeCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr= CreateCSFromCso(device, "Data/shader/particle_count_cs.cso", mCSCountShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//描画用シェーダーの生成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	FileFunction::Load(mEditorData, "Data/file/title_particle_data.bin", "rb");
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void TitleParticle::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("title particle");
	//パラメーターを調整する
	float* angleMovement[3] = { &mEditorData.angleMovement.x,&mEditorData.angleMovement.y,&mEditorData.angleMovement.z };
	ImGui::SliderFloat3("angleMovement", *angleMovement, -3.14f, 3.14f);
	ImGui::InputFloat("length", &mEditorData.leng, 1);
	static float moveSize = 1;
	ImGui::InputFloat("moveSize", &moveSize, 1);
	ImGui::InputFloat("startPosition.x", &mEditorData.startPosition.x, moveSize);
	ImGui::InputFloat("startPosition.y", &mEditorData.startPosition.y, moveSize);
	ImGui::InputFloat("startPosition.z", &mEditorData.startPosition.z, moveSize);
	ImGui::InputFloat("life", &mEditorData.life, 1);
	ImGui::InputFloat("moveLen", &mEditorData.moveLen, 1);
	float* sphereRatio[3] = { &mEditorData.sphereRatio.x,&mEditorData.sphereRatio.y ,&mEditorData.sphereRatio.z };
	ImGui::SliderFloat3("sphereRatio", *sphereRatio, 0, 1);
	float* color[4] = { &mEditorData.color.x,&mEditorData.color.y ,&mEditorData.color.z ,&mEditorData.color.w };
	ImGui::ColorEdit4("color", *color);
	ImGui::InputFloat("randSpeed", &mEditorData.randSpeed, 1);
	ImGui::InputFloat("defSpeed", &mEditorData.defSpeed, 1);
	ImGui::InputFloat("randMoveLength", &mEditorData.randMoveLength, 0.1f);
	ImGui::InputFloat("defMoveLength", &mEditorData.defMoveLength, 0.1f);
	ImGui::InputFloat("randScale", &mEditorData.randScale, 0.1f);
	ImGui::SliderFloat("randMoveAngle", &mEditorData.randMoveAngle, 3.14f, 3.14f);
	float* defVelocity[3] = { &mEditorData.defVelocity.x,&mEditorData.defVelocity.y ,&mEditorData.defVelocity.z };
	ImGui::SliderFloat3("def velocity", *defVelocity, -1, 1);
	ImGui::Checkbox("scene change", &mSceneChange);
	//描画に使うテクスチャの選択
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
	if (ImGui::Button("save"))FileFunction::Save(mEditorData, "Data/file/title_particle_data.bin", "wb");
	ImGui::Text("%f", mNewIndex);

	ImGui::End();
#endif
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void TitleParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	//UAVをGPUに渡す
	mParticle->Activate(context, 0, true);
	mParticleCount->Activate(context, 1, true);
	mParticleRender->Activate(context, 2, true);
	mParticleIndexs[mIndexNum]->Activate(context, 3, true);
	mParticleIndexs[1 - mIndexNum]->Activate(context, 4, true);
	mParticleDeleteIndex->Activate(context, 5, true);
	mIndexNum++;
	if (mIndexNum >= 2)mIndexNum = 0;
	//定数バッファのデータ更新
	mCb->data.elapsdTime = elapsdTime;
	mCb->data.angleMovement = mEditorData.angleMovement;
	mCbStart->data.color = mEditorData.color;
	mCbStart->data.startPosition = mEditorData.startPosition;
	mCbStart->data.leng = mEditorData.leng;
	mCbStart->data.sphereRatio = mEditorData.sphereRatio;
	mCbStart->data.life = mEditorData.life;
	mCbStart->data.moveLen = mEditorData.moveLen;
	mCbStart->data.randSpeed = mEditorData.randSpeed;
	mCbStart->data.defSpeed = mEditorData.defSpeed;
	mCbStart->data.startIndex = mStartIdex;
	mCbStart2->data.randMoveLength = mEditorData.randMoveLength;
	mCbStart2->data.defMoveLength = mEditorData.defMoveLength;
	mCbStart2->data.randMoveAngle = mEditorData.randMoveAngle;
	mCbStart2->data.randScale = mEditorData.randScale;
	mCbStart2->data.defVelocity = mEditorData.defVelocity;

	//定数バッファをGPU側に送る
	mCbStart->Activate(context, 0, false, false, false, true);
	mCbStart2->Activate(context, 1, false, false, false, true);
	mCb->Activate(context, 2, false, false, false, true);
	//シーン変更処理前
	if (!mSceneChange)
	{
		//生成数の更新
		mNewIndex += 1000 * elapsdTime;
		float indexSize = mNewIndex - mStartIdex;
		//生成数が1以上なら
		if (indexSize >= 1)
		{
			context->CSSetShader(mCreateShader.Get(), nullptr, 0);
			//パーティクルの生成
			context->Dispatch(static_cast<UINT>(indexSize), 1, 1);
			mStartIdex = mNewIndex;
		}
		//最大数より多ければ
		if (mNewIndex > mMaxParticle)
		{
			mNewIndex = 0;
			mStartIdex = 0;
		}
		//シェーダーを設定
		context->CSSetShader(mCSShader.Get(), nullptr, 0);
	}
	else
	{
		//シェーダーを設定
		context->CSSetShader(mSceneChangeCSShader.Get(), nullptr, 0);
	}
	//パーティクルの更新
	context->Dispatch(1000, 1, 1);

	//GPU側に送ったデータを元に戻す
	mCbStart->DeActivate(context);
	mCbStart2->DeActivate(context);
	mCb->DeActivate(context);
	//パーティクル数の更新
	context->CSSetShader(mCSCountShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);

	//カウントを取得
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mParticleCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mRenderCount = particleCount->aliveParticleCount;
	context->Unmap(mParticleCount->GetBuffer(), NULL);
	//GPUのデータを解放
	context->CSSetShader(nullptr, nullptr, 0);
	mParticle->DeActivate(context);
	mParticleCount->DeActivate(context);
	mParticleRender->DeActivate(context);
	mParticleIndexs[mIndexNum]->DeActivate(context);
	mParticleIndexs[1 - mIndexNum]->DeActivate(context);
	mParticleDeleteIndex->DeActivate(context);

}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

void TitleParticle::Render(ID3D11DeviceContext* context)
{
	//シェーダーの設定
	mShader->Activate(context);
	//描画時に使うテクスチャの設定
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
	//バッファの設定
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* buffer = mParticleRender->GetBuffer();
	ID3D11Buffer* index = mParticleIndexs[mIndexNum]->GetBuffer();
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->DrawIndexed(mRenderCount, 0, 0);
	//GPUからの解放
	mShader->Deactivate(context);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
	buffer = nullptr;
	index = nullptr;
	stride = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

}

