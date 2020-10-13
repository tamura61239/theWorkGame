#include "run_particle.h"
#include"misc.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif


RunParticles::RunParticles(ID3D11Device* device)
{
	HRESULT hr;
	std::vector<Particle> particles;
	std::vector<VECTOR2F>datas;
	particles.resize(100000);
	SetRandBufferData(datas);
	//バッファ生成
	{
		//定数バッファ
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
		desc.ByteWidth = sizeof(VECTOR2F) * datas.size();
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		//乱数バッファ
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &datas[0];
		hr = device->CreateBuffer(&desc, &data, mRandBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//パーティクルバッファ
		desc.ByteWidth = sizeof(Particle) * particles.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, mParticleBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//UAV生成
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = sizeof(Particle) * particles.size() / 4;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		hr = device->CreateUnorderedAccessView(mParticleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
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
		desc.BufferEx.NumElements = sizeof(VECTOR2F) * datas.size() / 4;
		hr = device->CreateShaderResourceView(mRandBuffer.Get(), &desc, mRandSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	mCb.angleMovement = VECTOR3F(DirectX::XMConvertToRadians(200), DirectX::XMConvertToRadians(100), 0);
	mCb.startIndex = 0;
	mCb.indexSize = 100;
	mCb.createFlag = 0;

	mCbStart.color = VECTOR4F(1, 0, 0, 1);
	mCbStart.maxLife = 1;
	mCbStart.moveType = 0;
	mCbStart.playerPosition = VECTOR4F(0, 0, 0, 1);
	mCbStart.playerVelocity = VECTOR3F(0, 0, 0);
	mCbStart.rand1 = 550;
	mCbStart.rand2 = 150;
	mCbStart.totalRand = 400;
	//シェーダー生成
	{
		hr = create_cs_from_cso(device, "Data/shader/run_particle_create_cs.cso", mCSCreateShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_cs_from_cso(device, "Data/shader/run_particle_cs.cso", mCSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_gs_from_cso(device, "Data/shader/run_particle_gs.cso", mGSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_ps_from_cso(device, "Data/shader/stage_obj_particle_ps.cso", mPSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"ACCEL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"LIFE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		hr = create_vs_from_cso(device, "Data/shader/run_particle_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	flag = false;
}

void RunParticles::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("run particle");
	float* color[4] = { &mCbStart.color.x,&mCbStart.color.y ,&mCbStart.color.z ,&mCbStart.color.w };
	ImGui::ColorEdit4("color", *color);
	ImGui::End();
#endif

}

void RunParticles::SetPlayerData(const VECTOR3F& velocity, bool groundFlag, const VECTOR3F& position)
{
	mPlayerData.mGroundFlag = groundFlag;
	mPlayerData.mPosition = position;
	mPlayerData.mVelocity = velocity;
}

void RunParticles::Update(ID3D11DeviceContext* context, float elapsd_time)
{
	float length = 0;
	DirectX::XMStoreFloat(&length, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&mPlayerData.mVelocity)));
	mCb.createFlag = 1;
	if (length <= 0.1f)mCb.createFlag = 0;
	mCb.elapsdTime = elapsd_time;
	mCb.startIndex += mCb.indexSize * elapsd_time;
	if (mCb.startIndex >= 100000)mCb.startIndex -= 100000;
	mCbStart.playerPosition = VECTOR4F(mPlayerData.mPosition.x, mPlayerData.mPosition.y, mPlayerData.mPosition.z, 1);
	mCbStart.playerVelocity = mPlayerData.mVelocity;
	mCbStart.moveType = 0;
	if (mPlayerData.mGroundFlag && !flag)mCbStart.moveType = 1;
	flag = mPlayerData.mGroundFlag;
	context->CSSetConstantBuffers(0, 1, mCbBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mCbStartBuffer.GetAddressOf());

	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &mCb, 0, 0);
	context->UpdateSubresource(mCbStartBuffer.Get(), 0, 0, &mCbStart, 0, 0);

	context->CSSetShader(mCSCreateShader.Get(), nullptr, 0);

	ID3D11UnorderedAccessView* uav[1] = { mParticleUAV.Get() };
	context->CSSetUnorderedAccessViews(0, 1, uav, 0);

	ID3D11ShaderResourceView* srv[1] = { mRandSRV.Get() };
	context->CSSetShaderResources(0, 1, srv);

	context->Dispatch(1 + static_cast<UINT>(mCbStart.moveType) * 9, 1, 1);
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	context->Dispatch(1000, 1, 1);

	uav[0] = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, uav, 0);

	srv[0] = nullptr;
	context->CSSetShaderResources(0, 1, srv);

	context->CSSetShader(nullptr, nullptr, 0);
}

void RunParticles::Render(ID3D11DeviceContext* context)
{
	context->VSSetShader(mVSShader.Get(), nullptr, 0);
	context->GSSetShader(mGSShader.Get(), nullptr, 0);
	context->PSSetShader(mPSShader.Get(), nullptr, 0);
	context->IASetInputLayout(mInput.Get());
	u_int stride = sizeof(Particle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mParticleBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(100000, 0);

	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

}

void RunParticles::SetRandBufferData(std::vector<VECTOR2F>& data)
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/run_particle_rundom_data.bin", "rb") == 0)
	{
		data.resize(400);
		fread(&data[0], sizeof(VECTOR2F), 400, fp);
		fclose(fp);
		return;
	}
	for (int i = 0; i < 400; i++)
	{
		VECTOR2F rad;
		rad.x = DirectX::XMConvertToRadians(static_cast<float>(rand() % 360));
		rad.y = static_cast<float>(rand() - 16383.5f) / 16383.5f;
		data.push_back(rad);
	}
	fopen_s(&fp, "Data/file/run_particle_rundom_data.bin", "wb");
	fwrite(&data[0], sizeof(VECTOR2F), 400, fp);
	fclose(fp);
}

void RunParticles::Load()
{
}

void RunParticles::Save()
{
}
