#include "stage_scene_particle.h"
#include"misc.h"
#include"shader.h"
#include"texture.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

StageSceneParticle::StageSceneParticle(ID3D11Device* device) :mMaxCount(100000), mIndexCount(0), mBeforeIndex(0)
{
	::memset(&mEditorData, 0, sizeof(mEditorData));
	std::vector<Particle>particles;
	particles.resize(mMaxCount);
	std::vector<RenderParticle>renderParticles;
	renderParticles.resize(mMaxCount);
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer>particleBuffer;
	//バッファの生成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, 0, sizeof(desc));
		//定数バッファ生成
		desc.ByteWidth = sizeof(CbCreate);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbCreateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbUpdate);
		hr = device->CreateBuffer(&desc, nullptr, mCbUpdateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//パーティクルのバッファ生成
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.ByteWidth = sizeof(Particle) * mMaxCount;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, particleBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//描画用バッファ
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = sizeof(RenderParticle) * mMaxCount;
		data.pSysMem = &renderParticles[0];
		hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//UAVの生成
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, 0, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.Buffer.NumElements = sizeof(Particle) * mMaxCount / 4;
		hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxCount / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//シェーダーの読み込み
	hr = create_cs_from_cso(device, "Data/shader/stage_scene_particle_create_cs.cso", mCSCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_cs_from_cso(device, "Data/shader/stage_scene_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

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
	//	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/particle_motion_data_render_vs.cso", "Data/shader/particle_motiom_blur_gs.cso", "Data/shader/particle_motion_blur_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc)));

	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "", "Data/shader/particle_render_point_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc)));
	//テクスチャの読み込み
	hr = load_texture_from_file(device, L"Data/image/○.png", mTextureSRV.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//SamplerStateの生成
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.MaxAnisotropy = 16;
		memcpy(desc.BorderColor, &VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f), sizeof(VECTOR4F));
		hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	Load();
}

void StageSceneParticle::ImGuiUpdate()
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
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::Text("position x:%f,y:%f,z:%f", mEditorData.createCentralPosition.x, mEditorData.createCentralPosition.y, mEditorData.createCentralPosition.z);
	ImGui::End();
#endif
}

void StageSceneParticle::Update(ID3D11DeviceContext* context, float elapsdTime)
{
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	ID3D11Buffer* buffers[] =
	{
		mCbCreateBuffer.Get(),
		mCbUpdateBuffer.Get()
	};
	context->CSSetConstantBuffers(0, 2, buffers);
	mIndexCount += mEditorData.oneSecondCreateNumber * elapsdTime;
	float size = mIndexCount - mBeforeIndex;
	//パーティクルの生成
	if (size >= 1)
	{
		context->CSSetShader(mCSCreateShader.Get(), nullptr, 0);
		CbCreate create;
		create.randX = mEditorData.randX;
		create.startIndex = mBeforeIndex;
		create.createArea = mEditorData.createArea;
		create.createCentralPosition = mEditorData.createCentralPosition;
		create.color = mEditorData.color;
		create.maxLife = mEditorData.maxLife;
		create.scale = mEditorData.scale;
		create.colorRatio = mEditorData.colorRatio;
		create.color2 = mEditorData.color2;
		context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &create, 0, 0);
		int count = static_cast<int>(size);
		if (create.startIndex + count >= mMaxCount)
		{
			mIndexCount = 0;
			count -= static_cast<int>(mIndexCount - mMaxCount);
		}
		context->Dispatch(count, 1, 1);
		mBeforeIndex = mIndexCount;
	}
	//更新
	CbUpdate update;
	update.elapsdTime = elapsdTime;
	update.windDirection = mEditorData.windDirection;
	update.maxSpeed = mEditorData.maxSpeed;

	context->UpdateSubresource(mCbUpdateBuffer.Get(), 0, 0, &update, 0, 0);
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	context->Dispatch(mMaxCount / 100, 1, 1);


	buffers[0] = nullptr;
	buffers[1] = nullptr;
	context->CSSetConstantBuffers(0, 2, buffers);

	ID3D11UnorderedAccessView* uav[2] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 1, &uav[0], nullptr);
	context->CSSetUnorderedAccessViews(2, 1, &uav[1], nullptr);

	context->CSSetShader(nullptr, nullptr, 0);

}

void StageSceneParticle::Render(ID3D11DeviceContext* context)
{
	mShader[mEditorData.shaderType]->Activate(context);

	context->PSSetShaderResources(0, 1, mTextureSRV.GetAddressOf());
	context->PSSetSamplers(0,1,mSamplerState.GetAddressOf());
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mMaxCount, 0);
	mShader[mEditorData.shaderType]->Deactivate(context);

	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11SamplerState* sample = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
	context->PSSetSamplers(0, 1, &sample);

}

void StageSceneParticle::Render(ID3D11DeviceContext* context, DrowShader* shader)
{
	shader->Activate(context);

	//context->PSSetShaderResources(0, 1, mTextureSRV.GetAddressOf());
	//context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mMaxCount, 0);
	shader->Deactivate(context);

	//ID3D11ShaderResourceView* srv = nullptr;
	//ID3D11SamplerState* sample = nullptr;
	//context->PSSetShaderResources(0, 1, &srv);
	//context->PSSetSamplers(0, 1, &sample);

}


void StageSceneParticle::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/stage_scene_paricte_data.bin", "rb") == 0)
	{
		fseek(fp, 0, SEEK_END);
		long size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		fread(&mEditorData, size, 1, fp);
		fclose(fp);

	}

}

void StageSceneParticle::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/stage_scene_paricte_data.bin", "wb");
	fwrite(&mEditorData, sizeof(EditorData), 1, fp);
	fclose(fp);
}
