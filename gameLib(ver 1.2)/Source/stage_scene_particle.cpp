#include "stage_scene_particle.h"
#include"misc.h"
#include"shader.h"
#include"texture.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

StageSceneParticle::StageSceneParticle(ID3D11Device* device) :mMaxCount(100000), mIndexCount(0)
, mCreateCount(0), mRenderCount(0)
{
	::memset(&mEditorData, 0, sizeof(mEditorData));
	HRESULT hr;
	{
		//パーティクルのバッファ
		std::vector<Particle>particles;
		particles.resize(static_cast<size_t>(mMaxCount));
		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particles, true);
		//描画用バッファ
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(static_cast<size_t>(mMaxCount));
		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
		//indexバッファ
		std::vector<UINT>indices;
		indices.resize(static_cast<size_t>(mMaxCount));
		for (auto& index : mParticleIndices)
		{
			index = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, indices, true);
		}
		//deleteIndexバッファ
		for (int i = 0; i < mMaxCount; i++)
		{
			indices[i] = i;
		}
		mParticleDeleteIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, indices, true);
		//パーティクルのカウントバッファ
		ParticleCount particleCount;
		memset(&particleCount, 0, sizeof(particleCount));
		particleCount.deActiveParticleCount = static_cast<UINT>(mMaxCount);
		mParticleCount = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particleCount, true, false, D3D11_CPU_ACCESS_READ);

	}
	//定数バッファ
	mCbCreate = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdate = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	//コンピュートシェーダーの生成
	hr = CreateCSFromCso(device, "Data/shader/stage_scene_particle_create_cs.cso", mCSCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/stage_scene_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_count_cs.cso", mCSEndShader.GetAddressOf());
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
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_cube_mesh_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc)));
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc)));
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "", "Data/shader/particle_render_point_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc)));
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
	//ファイルからデータのロード
	FileFunction::Load(mEditorData, "Data/file/stage_scene_paricte_data.bin", "rb");
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

void StageSceneParticle::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("stage scene particle");
	ImGui::InputFloat("randX", &mEditorData.randX, 0.1f);
	ImGui::InputFloat("create No", &mEditorData.oneSecondCreateNumber, 1);
	float* windDirection[3] = { &mEditorData.windDirection.x,&mEditorData.windDirection.y ,&mEditorData.windDirection.z };
	ImGui::DragFloat3("wind direction", *windDirection, 0.1f);
	float* createCentralPosition[3] = { &mEditorData.createCentralPosition.x,&mEditorData.createCentralPosition.y,&mEditorData.createCentralPosition.z };
	ImGui::DragFloat3("create central position", *createCentralPosition, 10);
	float* createArea[3] = { &mEditorData.createArea.x,&mEditorData.createArea.y,&mEditorData.createArea.z };
	ImGui::DragFloat3("create area", *createArea, 10);
	float* color[4] = { &mEditorData.color.x,&mEditorData.color.y,&mEditorData.color.z,&mEditorData.color.w };
	ImGui::ColorEdit4("color", *color);
	float* color2[4] = { &mEditorData.color2.x,&mEditorData.color2.y,&mEditorData.color2.z,&mEditorData.color2.w };
	ImGui::ColorEdit4("color2", *color2);
	ImGui::SliderFloat("ratio", &mEditorData.colorRatio.y, 0, 1);

	mEditorData.colorRatio.x = 1 - mEditorData.colorRatio.y;

	ImGui::Text("colorRatio %f:%f", mEditorData.colorRatio.x, mEditorData.colorRatio.y);

	ImGui::InputFloat("max life", &mEditorData.maxLife, 1);
	ImGui::InputFloat("max speed", &mEditorData.maxSpeed, 1);
	ImGui::InputFloat("scale", &mEditorData.scale, 0.1f);

	ImGui::RadioButton("cube", &mEditorData.shaderType, 0); ImGui::SameLine();
	ImGui::RadioButton("texe", &mEditorData.shaderType, 1); ImGui::SameLine();
	ImGui::RadioButton("point", &mEditorData.shaderType, 2);

	ImVec2 size = ImVec2(75, 75);
	for (UINT i = 0; i < static_cast<UINT>(mParticleSRV.size()); i++)
	{
		if (ImGui::ImageButton(mParticleSRV[i].Get(), size))
		{
			mEditorData.textureType = i;
		}
		if (i % 4 < 3 && i < static_cast<UINT>(mParticleSRV.size() - 1))ImGui::SameLine();
	}
	ImGui::Text(u8"今のテクスチャ");
	size = ImVec2(150, 150);
	ImGui::Image(mParticleSRV[mEditorData.textureType].Get(), size);

	if (ImGui::Button("save"))
	{
		FileFunction::Save(mEditorData, "Data/file/stage_scene_paricte_data.bin", "wb");

	}
	ImGui::Text("position x:%f,y:%f,z:%f", mEditorData.createCentralPosition.x, mEditorData.createCentralPosition.y, mEditorData.createCentralPosition.z);
	ImGui::End();
#endif
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void StageSceneParticle::Update(ID3D11DeviceContext* context, float elapsdTime)
{
	//GPU側にパーティクルにデータを送る
	mParticle->Activate(context, 0, true);
	mParticleCount->Activate(context, 1, true);
	mParticleRender->Activate(context, 2, true);
	mParticleIndices[mIndexCount]->Activate(context, 3, true);
	mParticleIndices[1 - mIndexCount]->Activate(context, 4, true);
	mParticleDeleteIndex->Activate(context, 5, true);
	mIndexCount++;
	if (mIndexCount >= 2)
	{
		mIndexCount = 0;
	}
	//生成する数の更新
	mCreateCount += elapsdTime * mEditorData.oneSecondCreateNumber;
	if (mCreateCount >= 1)
	{
		//シェーダーの設定
		context->CSSetShader(mCSCreateShader.Get(), nullptr, 0);
		//定数バッファのデータ更新
		mCbCreate->data.color = mEditorData.color;
		mCbCreate->data.randX = mEditorData.randX;
		mCbCreate->data.createArea = mEditorData.createArea;
		mCbCreate->data.createCentralPosition = mEditorData.createCentralPosition;
		mCbCreate->data.maxLife = mEditorData.maxLife;
		mCbCreate->data.scale = mEditorData.scale;
		mCbCreate->data.colorRatio = mEditorData.colorRatio;
		mCbCreate->data.color2 = mEditorData.color2;
		//定数バッファをGPU側に送る
		mCbCreate->Activate(context, 0, false, false, false, true);
		//小数点以下切り捨て
		UINT count = static_cast<UINT>(mCreateCount);
		//更新
		context->Dispatch(count, 1, 1);
		//生成した分だけ引いとく
		mCreateCount -= static_cast<float>(count);
		//送ったデータを元に戻す
		mCbCreate->DeActivate(context);

	}
	//パーティクルの更新
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	mCbUpdate->data.elapsdTime = elapsdTime;
	mCbUpdate->data.windDirection = mEditorData.windDirection;
	mCbUpdate->data.maxSpeed = mEditorData.maxSpeed;
	mCbUpdate->Activate(context, 1, false, false, false, true);
	context->Dispatch(mMaxCount / 100, 1, 1);
	mCbUpdate->DeActivate(context);
	//パーティクルのカウントの更新
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

/****************************描画(シェーダーを取得してない)******************************/
void StageSceneParticle::Render(ID3D11DeviceContext* context)
{
	//シェーダーの設定
	mShader[mEditorData.shaderType]->Activate(context);
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
	mShader[mEditorData.shaderType]->Deactivate(context);
	offset = 0;
	vertex = nullptr;
	index = nullptr;
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);

}
/****************************描画(シェーダーを取得する)******************************/

void StageSceneParticle::Render(ID3D11DeviceContext* context, DrowShader* shader)
{
	//シェーダーの設定
	shader->Activate(context);
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
	shader->Deactivate(context);
	offset = 0;
	vertex = nullptr;
	index = nullptr;
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);

}

