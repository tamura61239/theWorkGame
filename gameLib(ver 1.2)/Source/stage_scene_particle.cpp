#include "stage_scene_particle.h"
#include"misc.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

StageSceneParticle::StageSceneParticle(ID3D11Device* device)
{
	HRESULT hr;
	//定数バッファ作成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(Cb);
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbStart);
		hr = device->CreateBuffer(&desc, nullptr, mCbStartBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//シェーダー作成
	{
		hr = create_cs_from_cso(device, "Data/shader/stage_scene_particle_cs.cso", mCSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_cs_from_cso(device, "Data/shader/stage_scene_particle_create_cs.cso", mCSCreateShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_gs_from_cso(device, "Data/shader/run_particle_gs.cso", mGSShader.GetAddressOf());
		//hr = create_gs_from_cso(device, "Data/shader/stage_obj_particle_gs.cso", mGSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_ps_from_cso(device, "Data/shader/stage_obj_particle_ps.cso", mPSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"LIFE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLORTYPE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		hr = create_vs_from_cso(device, "Data/shader/stage_scene_particle_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		mVelocityShader = std::make_unique<DrowShader>(device, "Data/shader/stage_scene_particle_blur_vs.cso", "Data/shader/stage_scene_particle_blur_gs.cso", "Data/shader/stage_scene_particle_blur_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	}

	//
	std::vector<Particle>particles;
	particles.resize(100000);
	particleSize = particles.size();
	std::vector<VECTOR3F>randData;
	LoadRandData(randData);
	//バッファの生成
	{
		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&data, sizeof(data));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.ByteWidth = sizeof(VECTOR3F) * randData.size();
		data.pSysMem = &randData[0];
		hr = device->CreateBuffer(&desc, &data, mRandBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(Particle) * particleSize;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, mParticleBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc,sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.Buffer.NumElements = sizeof(Particle) * particleSize/4;
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
		desc.BufferEx.NumElements = sizeof(VECTOR3F) * randData.size() / 4;
		hr = device->CreateShaderResourceView(mRandBuffer.Get(), &desc, mRandSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	mCb.angleMovement = VECTOR3F(0, 0, 0);
	mCb.elapsdTime = 0;
	mCbStart.createRange = VECTOR3F(10, 10, 10);
	mCbStart.startIndex = 0;
	oneTimeIndexSize = 100;
	oneframeIndex = 0;
	mCbStart.maxLife = 5;
	mCbStart.startPosition = VECTOR3F(0, 10, 20);
	Load();
}

void StageSceneParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("stage scene particle");
	float* angle[3] = { &mCb.angleMovement.x,&mCb.angleMovement.y,&mCb.angleMovement.z };
	ImGui::SliderFloat3("angleMovement", *angle, -3.14f, 3.14f);
	float* range[3] = { &mCbStart.createRange.x,&mCbStart.createRange.y,&mCbStart.createRange.z };
	ImGui::DragFloat3("range", *range, 10);
	float* position[3] = { &mCbStart.startPosition.x,&mCbStart.startPosition.y,&mCbStart.startPosition.z };
	ImGui::DragFloat3("position", *position, 10);
	ImGui::InputFloat("max life", &mCbStart.maxLife, 0.1f);
	ImGui::InputInt("index size", &oneTimeIndexSize, 1);
	if (ImGui::Button("save"))Save();
	ImGui::End();
#endif

}

void StageSceneParticle::Update(ID3D11DeviceContext* context, float elapsdTime)
{
	mCb.elapsdTime = elapsdTime;
	if (mParticleUAV.Get() == nullptr)return;
	ID3D11ShaderResourceView* srv = mRandSRV.Get();

	context->CSSetShaderResources(0, 1, &srv);

	ID3D11UnorderedAccessView* uav = mParticleUAV.Get();
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

	newStartIndex += oneTimeIndexSize * elapsdTime;
	oneframeIndex = static_cast<int>(newStartIndex - mCbStart.startIndex);
	//if (newStartIndex >= particleSize)
	//{
	//	oneframeIndex -= static_cast<int>(newStartIndex - particleSize);
	//}
	//パーティクル生成
	context->CSSetConstantBuffers(0, 1, mCbStartBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &mCb, 0, 0);
	context->UpdateSubresource(mCbStartBuffer.Get(), 0, 0, &mCbStart, 0, 0);

	if (oneframeIndex >= 1) 
	{

		context->CSSetShader(mCSCreateShader.Get(), nullptr, 0);
		context->Dispatch(oneframeIndex, 1, 1);
		mCbStart.startIndex = newStartIndex;
		if (mCbStart.startIndex >= particleSize)
		{
			mCbStart.startIndex = 0;
			newStartIndex = 0;
		}
	}
	//パーティクル更新
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	context->Dispatch(particleSize/100, 1, 1);

	context->CSSetShader(nullptr, nullptr, 0);

	srv = nullptr;
	uav = nullptr;

	context->CSSetShaderResources(0, 1, &srv);
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
}

void StageSceneParticle::Render(ID3D11DeviceContext* context)
{
	if (mParticleUAV.Get() == nullptr)return;

	context->VSSetShader(mVSShader.Get(), nullptr, 0);
	context->GSSetShader(mGSShader.Get(), nullptr, 0);
	context->PSSetShader(mPSShader.Get(), nullptr, 0);
	context->IASetInputLayout(mInput.Get());


	u_int stride = sizeof(Particle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mParticleBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(particleSize, 0);

	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

}

void StageSceneParticle::Render(ID3D11DeviceContext* context, DrowShader* shader)
{
	if (mParticleUAV.Get() == nullptr)return;

	shader->Activate(context);

	u_int stride = sizeof(Particle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mParticleBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(particleSize, 0);

	shader->Deactivate(context);
}

void StageSceneParticle::RenderVelocity(ID3D11DeviceContext* context)
{
	if (mParticleUAV.Get() == nullptr)return;

	mVelocityShader->Activate(context);
	context->GSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());
	context->PSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());
	context->VSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());

	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &mCb, 0, 0);

	u_int stride = sizeof(Particle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mParticleBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(particleSize, 0);

	mVelocityShader->Deactivate(context);

}

void StageSceneParticle::LoadRandData(std::vector<VECTOR3F>& data)
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/stage_obj_particle_random.bin", "rb") == 0)
	{
		data.resize(400);
		fread(&data[0], sizeof(VECTOR3F), 400, fp);
		fclose(fp);
		return;
	}
	for (int i = 0; i < 400; i++)
	{
		data.push_back(VECTOR3F(static_cast<float>(rand()) / 32767.f, static_cast<float>(rand()) / 32767.f, static_cast<float>(rand()) / 32767.f));
	}
	fopen_s(&fp, "Data/file/stage_obj_particle_random.bin", "wb");
	fwrite(&data[0], sizeof(VECTOR3F), 400, fp);
	fclose(fp);

}

void StageSceneParticle::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/stage_scene_particle_data.bin", "rb") == 0)
	{
		SaveData data;
		fread(&data, sizeof(SaveData), 1, fp);
		mCbStart.createRange = data.createRange;
		mCbStart.startPosition = data.startPosition;
		mCbStart.maxLife = data.maxLife;
		mCb.angleMovement = data.angleMovement;
		oneTimeIndexSize = static_cast<float>(data.oneTimeIndexSize);
		fclose(fp);
	}
}

void StageSceneParticle::Save()
{
	FILE* fp;

	fopen_s(&fp, "Data/file/stage_scene_particle_data.bin", "wb");
	SaveData data;
	data.maxLife = mCbStart.maxLife;
	data.createRange = mCbStart.createRange;
	data.startPosition = mCbStart.startPosition;
	data.angleMovement = mCb.angleMovement;
	data.oneTimeIndexSize = static_cast<float>(oneTimeIndexSize);
	fwrite(&data, sizeof(SaveData), 1, fp);
	fclose(fp);
}
