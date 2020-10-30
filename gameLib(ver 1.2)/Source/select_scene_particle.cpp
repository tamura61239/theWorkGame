#include "select_scene_particle.h"
#include"misc.h"
#include<vector>
#include"shader.h"
#include"camera_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif


SelectSceneParticle::SelectSceneParticle(ID3D11Device* device)
{
	std::vector<Particle>particles;
	//Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;
	mMaxParticle = 60000;
	particles.resize(mMaxParticle);
	HRESULT hr;
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(Cb);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbStart);
		hr = device->CreateBuffer(&desc, nullptr, mCbStartBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(Particle) * particles.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, mParticleBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.ByteWidth = sizeof(RenderParticle) * particles.size();
		//desc.StructureByteStride = sizeof(RenderParticle);
		hr = device->CreateBuffer(&desc, nullptr, mRenderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

		desc.ByteWidth = sizeof(float);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		float num = 0;
		data.pSysMem = &num;

		hr = device->CreateBuffer(&desc, &data, mNumberBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		desc.BufferEx.FirstElement = 0;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = sizeof(Particle) * particles.size() / 4;
		hr = device->CreateShaderResourceView(mParticleBuffer.Get(), &desc, mSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.Buffer.NumElements = sizeof(Particle) * particles.size() / 4;
		hr = device->CreateUnorderedAccessView(mParticleBuffer.Get(), &desc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Buffer.NumElements = sizeof(RenderParticle) * particles.size() / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Buffer.NumElements = sizeof(float) / 4;
		hr = device->CreateUnorderedAccessView(mNumberBuffer.Get(), &desc, mNumberUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	mCbStart.angleMovement = VECTOR3F(3.14f / 2.f, 3.14f, 0);
	mCbStart.color = VECTOR4F(0, 1, 0.5f, 1);
	mCbStart.range = 200;
	mCbStart.scope = VECTOR3F(1, 1, 0.3f);
	mCbStart.speed = 200;
	mCbStart.startIndex = 0;
	mCb.endPosition = VECTOR3F(0, 0, 1000);

	mCb.defVelocity = VECTOR3F(0, 0, 1);
	hr = create_cs_from_cso(device, "Data/shader/select_scene_create_particle_cs.cso", mCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_cs_from_cso(device, "Data/shader/select_scene_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_cs_from_cso(device, "Data/shader/particle_render_set_cs.cso", mRenderSetCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_cs_from_cso(device, "Data/shader/clear_render_cs.cso", mClearCSShader.GetAddressOf());
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

}

void SelectSceneParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("select scene particle");
	ImGui::InputFloat("speed", &mCbStart.speed, 0.5f);
	ImGui::InputFloat("range", &mCbStart.range, 1);
	float* color[4] = { &mCbStart.color.x,&mCbStart.color.y,&mCbStart.color.z,&mCbStart.color.w };
	ImGui::ColorEdit4("def color", *color);
	float* scope[3] = { &mCbStart.scope.x,&mCbStart.scope.y,&mCbStart.scope.z };
	ImGui::SliderFloat3("scope", *scope, 0, 1);
	float* angleMovement[3] = { &mCbStart.angleMovement.x,&mCbStart.angleMovement.y ,&mCbStart.angleMovement.z };
	ImGui::SliderFloat3("def angleMovement", *angleMovement, -3.14f, 3.14f);
	ImGui::Text("%f", newIndex);
	ImGui::End();
#endif

}

void SelectSceneParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	mCb.elapsdTime = elapsdTime;
	mCbStart.eye = pCameraManager.GetCamera()->GetEye();
	mCbStart.eye.y = 0;
	if (mUAV.Get() == nullptr)return;
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);


	context->CSSetConstantBuffers(0, 1, mCbStartBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());

	context->UpdateSubresource(mCbStartBuffer.Get(), 0, 0, &mCbStart, 0, 0);
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &mCb, 0, 0);

	context->CSSetUnorderedAccessViews(0, 1, mUAV.GetAddressOf(), nullptr);

	newIndex += 2000 * elapsdTime;
	float indexSize = newIndex - mCbStart.startIndex;
	if (indexSize >= 1)
	{
		context->CSSetShader(mCreateShader.Get(), nullptr, 0);

		context->Dispatch(static_cast<UINT>(indexSize), 1, 1);
		mCbStart.startIndex = newIndex;
	}
	if (newIndex >= mMaxParticle)
	{
		newIndex = 0;
		mCbStart.startIndex = 0;
	}
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	context->Dispatch(1000, 1, 1);

	context->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView* uav[3] = { nullptr,nullptr,nullptr };
	context->CSSetUnorderedAccessViews(0, 3, uav, nullptr);


}

void SelectSceneParticle::Render(ID3D11DeviceContext* context)
{
	if (mUAV.Get() == nullptr)return;

	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mMaxParticle, 0);
	mShader->Deactivate(context);

}
