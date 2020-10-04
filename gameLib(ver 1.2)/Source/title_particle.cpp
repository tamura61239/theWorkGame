#include "title_particle.h"
#include"misc.h"
#include<vector>
#include"shader.h"
#include"camera_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

TitleParticle::TitleParticle(ID3D11Device* device):mSceneChange(false)
{
	std::vector<Particle>particles;
	//Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;

	particles.resize(100000);
	HRESULT hr;
	//buffer
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
		desc.ByteWidth = sizeof(CbStart2);
		hr = device->CreateBuffer(&desc, nullptr, mCbStart2Buffer.GetAddressOf());
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
	mCbStart.leng = 100;
	mCbStart.sphereRatio = VECTOR3F(1, 1, 0.3f);
	mCbStart.startIndex = 0;
	mCbStart.startPosition = VECTOR3F(0, 0, -50);
	mCbStart.color = VECTOR4F(1, 1, 1, 1);
	mCbStart.life = 8;
	mCb.elapsdTime = 0;
	mCb.angleMovement = VECTOR3F(3.14f * 0.78f, 3.14f * 1.12f, 0);
	mCbStart.moveLen = 10;
	mCbStart.randSpeed = 20;
	mCbStart.defSpeed = 10;

	mCbStart2.defMoveLength = 5;
	mCbStart2.randMoveLength = 0.5f;
	mCbStart2.randMoveAngle = 1;
	mCbStart2.randScale = 1;
	mCbStart2.defVelocity = VECTOR3F(-1, 0, 1);

	hr = create_cs_from_cso(device, "Data/shader/title_particle_create_cs.cso", mCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_cs_from_cso(device, "Data/shader/title_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_cs_from_cso(device, "Data/shader/title_scene_change_particle_cs.cso", mSceneChangeCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/title_particle_vs.cso", "Data/shader/run_particle_gs.cso", "Data/shader/stage_obj_particle_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	Load();
	mCbStart.startIndex = 0;

}
//パーティクルの初期化や更新に必要なデータのimgui
void TitleParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("title particle");
	float* angleMovement[3] = { &mCb.angleMovement.x,&mCb.angleMovement.y,&mCb.angleMovement.z };
	ImGui::SliderFloat3("angleMovement", *angleMovement, -3.14f, 3.14f);
	ImGui::InputFloat("length", &mCbStart.leng, 1);
	static float moveSize = 1;
	ImGui::InputFloat("moveSize", &moveSize, 1);
	ImGui::InputFloat("startPosition.x", &mCbStart.startPosition.x, moveSize);
	ImGui::InputFloat("startPosition.y", &mCbStart.startPosition.y, moveSize);
	ImGui::InputFloat("startPosition.z", &mCbStart.startPosition.z, moveSize);
	ImGui::InputFloat("life", &mCbStart.life, 1);
	ImGui::InputFloat("moveLen", &mCbStart.moveLen, 1);
	float* sphereRatio[3] = { &mCbStart.sphereRatio.x,&mCbStart.sphereRatio.y ,&mCbStart.sphereRatio.z };
	ImGui::SliderFloat3("sphereRatio", *sphereRatio, 0, 1);
	float* color[4] = { &mCbStart.color.x,&mCbStart.color.y ,&mCbStart.color.z ,&mCbStart.color.w };
	ImGui::ColorEdit4("color", *color);
	ImGui::InputFloat("randSpeed", &mCbStart.randSpeed, 1);
	ImGui::InputFloat("defSpeed", &mCbStart.defSpeed, 1);
	ImGui::InputFloat("randMoveLength", &mCbStart2.randMoveLength, 0.1f);
	ImGui::InputFloat("defMoveLength", &mCbStart2.defMoveLength, 0.1f);
	ImGui::InputFloat("randScale", &mCbStart2.randScale, 0.1f);
	ImGui::SliderFloat("randMoveAngle", &mCbStart2.randMoveAngle, 3.14f, 3.14f);
	float* defVelocity[3] = { &mCbStart2.defVelocity.x,&mCbStart2.defVelocity.y ,&mCbStart2.defVelocity.z };
	ImGui::SliderFloat3("def velocity", *defVelocity, -1, 1);
	ImGui::Checkbox("scene change", &mSceneChange);
	if (ImGui::Button("save"))Save();
	ImGui::End();
#endif
}

void TitleParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	ImGuiUpdate();
	mCb.elapsdTime = elapsdTime;
	if (mUAV.Get() == nullptr)return;
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);


	context->CSSetConstantBuffers(0, 1, mCbStartBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mCbStart2Buffer.GetAddressOf());
	context->CSSetConstantBuffers(2, 1, mCbBuffer.GetAddressOf());

	context->UpdateSubresource(mCbStartBuffer.Get(), 0, 0, &mCbStart, 0, 0);
	context->UpdateSubresource(mCbStart2Buffer.Get(), 0, 0, &mCbStart2, 0, 0);
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &mCb, 0, 0);

	context->CSSetUnorderedAccessViews(0, 1, mUAV.GetAddressOf(), nullptr);
	if (!mSceneChange)
	{
		mNewIndex += 200 * elapsdTime;
		float indexSize = mNewIndex - mCbStart.startIndex;
		if (indexSize >= 1)
		{
			context->CSSetShader(mCreateShader.Get(), nullptr, 0);

			context->Dispatch(static_cast<UINT>(indexSize), 1, 1);
			mCbStart.startIndex = mNewIndex;
		}
		if (mNewIndex > 100000)
		{
			mNewIndex = 0;
			mCbStart.startIndex = 0;
		}
		context->CSSetShader(mCSShader.Get(), nullptr, 0);
	}
	else
	{
		context->CSSetShader(mSceneChangeCSShader.Get(), nullptr, 0);
	}
	context->Dispatch(1000, 1, 1);

	context->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView* uav[3] = { nullptr,nullptr,nullptr };
	context->CSSetUnorderedAccessViews(0, 3, uav, nullptr);

}

void TitleParticle::Render(ID3D11DeviceContext* context)
{
	if (mUAV.Get() == nullptr)return;

	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(100000, 0);
	mShader->Deactivate(context);

}

void TitleParticle::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/title_particle_data.bin", "rb") == 0)
	{
		fread(&mCbStart, sizeof(CbStart), 1, fp);
		fread(&mCbStart2, sizeof(CbStart2), 1, fp);
		fread(&mCb, sizeof(Cb), 1, fp);
		fclose(fp);
	}
}

void TitleParticle::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/title_particle_data.bin", "wb");
	{
		fwrite(&mCbStart, sizeof(CbStart), 1, fp);
		fwrite(&mCbStart2, sizeof(CbStart2), 1, fp);
		fwrite(&mCb, sizeof(Cb), 1, fp);
		fclose(fp);
	}

}
