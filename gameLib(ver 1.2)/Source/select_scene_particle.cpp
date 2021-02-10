#include "select_scene_particle.h"
#include"misc.h"
#include<vector>
#include"shader.h"
#include"camera_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif


SelectSceneParticle::SelectSceneParticle(ID3D11Device* device):mIndexCount(0), mRenderCount(0)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer>particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>deleteIndexBuffer;

	HRESULT hr;
	//{
	//	D3D11_BUFFER_DESC desc;
	//	ZeroMemory(&desc, sizeof(desc));
	//	desc.ByteWidth = sizeof(Cb);
	//	desc.Usage = D3D11_USAGE_DEFAULT;
	//	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//	desc.CPUAccessFlags = 0;
	//	desc.MiscFlags = 0;
	//	desc.StructureByteStride = 0;
	//	hr = device->CreateBuffer(&desc, nullptr, mCbBuffer.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//	desc.ByteWidth = sizeof(CbStart);
	//	hr = device->CreateBuffer(&desc, nullptr, mCbStartBuffer.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//	desc.ByteWidth = sizeof(Particle) * particles.size();
	//	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	//	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	//	D3D11_SUBRESOURCE_DATA data;
	//	ZeroMemory(&data, sizeof(data));
	//	data.pSysMem = &particles[0];
	//	hr = device->CreateBuffer(&desc, &data, mParticleBuffer.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//	//desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//	desc.ByteWidth = sizeof(RenderParticle) * particles.size();
	//	//desc.StructureByteStride = sizeof(RenderParticle);
	//	hr = device->CreateBuffer(&desc, nullptr, mRenderBuffer.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//	desc.Usage = D3D11_USAGE_DEFAULT;
	//	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

	//	desc.ByteWidth = sizeof(float);
	//	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//	float num = 0;
	//	data.pSysMem = &num;

	//	hr = device->CreateBuffer(&desc, &data, mNumberBuffer.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	////SRV
	//{
	//	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	//	ZeroMemory(&desc, sizeof(desc));
	//	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	//	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	//	desc.BufferEx.FirstElement = 0;
	//	desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	//	desc.BufferEx.NumElements = sizeof(Particle) * particles.size() / 4;
	//	hr = device->CreateShaderResourceView(mParticleBuffer.Get(), &desc, mSRV.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	////UAV
	//{
	//	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	//	ZeroMemory(&desc, sizeof(desc));
	//	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	//	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	//	desc.Buffer.FirstElement = 0;
	//	desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	//	desc.Buffer.NumElements = sizeof(Particle) * particles.size() / 4;
	//	hr = device->CreateUnorderedAccessView(mParticleBuffer.Get(), &desc, mUAV.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//	desc.Buffer.NumElements = sizeof(RenderParticle) * particles.size() / 4;
	//	hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//	desc.Buffer.NumElements = sizeof(float) / 4;
	//	hr = device->CreateUnorderedAccessView(mNumberBuffer.Get(), &desc, mNumberUAV.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//}
	//mCbCreate.angleMovement = VECTOR3F(3.14f / 2.f, 3.14f, 0);
	//mCbCreate.color = VECTOR4F(0, 1, 0.5f, 1);
	//mCbCreate.range = 200;
	//mCbCreate.scope = VECTOR3F(1, 1, 0.3f);
	//mCbCreate.speed = 200;
	//mCbCreate.startIndex = 0;
	//mCbUpdate.endPosition = VECTOR3F(0, 0, 1000);

	//mCbUpdate.defVelocity = VECTOR3F(0, 0, 1);
	mMaxParticle = 60000;
	//バッファ
	{
		std::vector<Particle>particles;
		particles.resize(static_cast<size_t>(mMaxParticle));
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//パーティクルのバッファ生成
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
		desc.StructureByteStride = sizeof(Particle);
		desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
		desc.ByteWidth = sizeof(Particle) * mMaxParticle;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, particleBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//描画用バッファ
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = sizeof(RenderParticle) * mMaxParticle;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		data.pSysMem = &renderParticles[0];
		hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//パーティクルのindexバッファ
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = sizeof(UINT) * mMaxParticle;
		std::vector<UINT>indices;
		indices.resize(mMaxParticle);
		memset(indices.data(), 0, sizeof(UINT)* mMaxParticle);
		data.pSysMem = &indices[0];

		for (auto& index : mParticleIndexBuffer)
		{
			hr = device->CreateBuffer(&desc, &data, index.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		//パーティクルのカウントバッファ
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = sizeof(ParticleCount);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		ParticleCount particleCount;
		memset(&particleCount, 0, sizeof(particleCount));
		particleCount.deActiveParticleCount = mMaxParticle;
		data.pSysMem = &particleCount;
		hr = device->CreateBuffer(&desc, &data, mParticleCountBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//パーティクルの死んでる分のindexバッファ
		for (int i = 0; i < mMaxParticle; i++)
		{
			indices[i] = i;
		}
		ZeroMemory(&desc, sizeof(desc));
		data.pSysMem = &indices[0];
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
		desc.StructureByteStride = sizeof(UINT);
		desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
		desc.ByteWidth = sizeof(UINT) * mMaxParticle;
		hr = device->CreateBuffer(&desc, &data, deleteIndexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Buffer.NumElements = mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = device->CreateUnorderedAccessView(deleteIndexBuffer.Get(), &desc, mDeleteIndexUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		ZeroMemory(&desc, sizeof(desc));
		//描画データのUAV
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//indexバッファのUAV
		desc.Buffer.NumElements = sizeof(UINT) * mMaxParticle / 4;
		for (int i = 0; i < 2; i++)
		{
			hr = device->CreateUnorderedAccessView(mParticleIndexBuffer[i].Get(), &desc, mParticleIndexUAV[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		//パーティクルのカウントUAV
		desc.Buffer.NumElements = sizeof(ParticleCount) / 4;
		hr = device->CreateUnorderedAccessView(mParticleCountBuffer.Get(), &desc, mParticleCountUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//定数バッファ
	mCbCreate = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdate = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	//シェーダーの読み込み
	hr = CreateCSFromCso(device, "Data/shader/select_scene_create_particle_cs.cso", mCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/select_scene_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_count_cs.cso", mCSEndShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_cube_mesh_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));

	mCbCreate->data.angleMovement = VECTOR3F(3.14f / 2.f, 3.14f, 0);
	mCbCreate->data.color = VECTOR4F(0, 1, 0.5f, 1);
	mCbCreate->data.range = 200;
	mCbCreate->data.scope = VECTOR3F(1, 1, 0.3f);
	mCbCreate->data.speed = 200;
	mCbCreate->data.startIndex = 0;
	mCbUpdate->data.endPosition = VECTOR3F(0, 0, 1000);
	mCbUpdate->data.defVelocity = VECTOR3F(0, 0, 1);
}

void SelectSceneParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("select scene particle");
	ImGui::InputFloat("speed", &mCbCreate->data.speed, 0.5f);
	ImGui::InputFloat("range", &mCbCreate->data.range, 1);
	float* color[4] = { &mCbCreate->data.color.x,&mCbCreate->data.color.y,&mCbCreate->data.color.z,&mCbCreate->data.color.w };
	ImGui::ColorEdit4("def color", *color);
	float* scope[3] = { &mCbCreate->data.scope.x,&mCbCreate->data.scope.y,&mCbCreate->data.scope.z };
	ImGui::SliderFloat3("scope", *scope, 0, 1);
	float* angleMovement[3] = { &mCbCreate->data.angleMovement.x,&mCbCreate->data.angleMovement.y ,&mCbCreate->data.angleMovement.z };
	ImGui::SliderFloat3("def angleMovement", *angleMovement, -3.14f, 3.14f);
	ImGui::Text("%f", newIndex);
	ImGui::End();
#endif

}

void SelectSceneParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	//mCbUpdate.elapsdTime = elapsdTime;
	//mCbCreate.eye = pCameraManager->GetCamera()->GetEye();
	//mCbCreate.eye.y = 0;
	//if (mUAV.Get() == nullptr)return;
	//context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);


	//context->CSSetConstantBuffers(0, 1, mCbStartBuffer.GetAddressOf());
	//context->CSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());

	//context->UpdateSubresource(mCbStartBuffer.Get(), 0, 0, &mCbCreate, 0, 0);
	//context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &mCbUpdate, 0, 0);

	//context->CSSetUnorderedAccessViews(0, 1, mUAV.GetAddressOf(), nullptr);

	//newIndex += 2000 * elapsdTime;
	//float indexSize = newIndex - mCbCreate.startIndex;
	//if (indexSize >= 1)
	//{
	//	context->CSSetShader(mCreateShader.Get(), nullptr, 0);

	//	context->Dispatch(static_cast<UINT>(indexSize), 1, 1);
	//	mCbCreate.startIndex = newIndex;
	//}
	//if (newIndex >= mMaxParticle)
	//{
	//	newIndex = 0;
	//	mCbCreate.startIndex = 0;
	//}
	//context->CSSetShader(mCSShader.Get(), nullptr, 0);

	//context->Dispatch(1000, 1, 1);

	//context->CSSetShader(nullptr, nullptr, 0);
	//ID3D11UnorderedAccessView* uav[3] = { nullptr,nullptr,nullptr };
	//context->CSSetUnorderedAccessViews(0, 3, uav, nullptr);
	//ID3D11Buffer* buffers[] = { nullptr,nullptr };
	//context->CSSetConstantBuffers(0, 2, buffers);
	ID3D11UnorderedAccessView* uavs[6] =
	{
		mParticleUAV.Get(),
		mParticleCountUAV.Get(),
		mRenderUAV.Get(),
		mParticleIndexUAV[mIndexCount].Get(),
		mParticleIndexUAV[1 - mIndexCount].Get(),
		mDeleteIndexUAV.Get()
	};
	context->CSSetUnorderedAccessViews(0, 6, uavs, nullptr);

	mIndexCount++;
	if (mIndexCount >= 2)mIndexCount = 0;
	newIndex += elapsdTime * 2000;
	//パーティクルの生成
	if (newIndex >= 1)
	{
		mCbCreate->data.eye = pCameraManager->GetCamera()->GetEye();
		mCbCreate->data.eye.y = 0;
		context->CSSetShader(mCreateShader.Get(), nullptr, 0);
		mCbCreate->Activate(context, 0, false, false, false, true);
		UINT count = static_cast<UINT>(newIndex);
		context->Dispatch(count, 1, 1);
		mCbCreate->DeActivate(context);
		newIndex -= static_cast<float>(count);
	}
	//パーティクルの更新
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	mCbUpdate->data.elapsdTime = elapsdTime;
	mCbUpdate->Activate(context, 1, false, false, false, true);
	context->Dispatch(mMaxParticle / 100, 1, 1);
	mCbUpdate->DeActivate(context);
	//パーティクルカウントの更新
	context->CSSetShader(mCSEndShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);
	//カウントのバッファから生きてる分のカウントを取得
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mParticleCountBuffer.Get(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mRenderCount = particleCount->aliveParticleCount;
	context->Unmap(mParticleCountBuffer.Get(), NULL);

	context->CSSetShader(nullptr, nullptr, 0);
	//後処理
	for (auto& uav : uavs)
	{
		uav = nullptr;
	}
	context->CSSetUnorderedAccessViews(0, 6, uavs, nullptr);

}

void SelectSceneParticle::Render(ID3D11DeviceContext* context)
{

	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* vertex = mRenderBuffer.Get();
	ID3D11Buffer* index = mParticleIndexBuffer[mIndexCount].Get();
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->DrawIndexed(mRenderCount, 0, 0);
	mShader->Deactivate(context);
	offset = 0;
	vertex = nullptr;
	index = nullptr;
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

}
