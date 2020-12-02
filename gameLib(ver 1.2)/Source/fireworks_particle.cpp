#include "fireworks_particle.h"
#include<string>
#include"misc.h"
#include"shader.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif

FireworksParticle::FireworksParticle(ID3D11Device* device):createFlag(false),mMaxParticle(10000)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer>buffer;

	{
		D3D11_BUFFER_DESC desc;
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(Particle) * mMaxParticle;
		desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
		desc.StructureByteStride = sizeof(Particle);
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//描画用バッファ
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.ByteWidth = sizeof(RenderParticle) * mMaxParticle;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = 0;
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		data.pSysMem = &renderParticles[0];
		hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//定数バッファ
		desc.ByteWidth = sizeof(CbUpdate);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbUpdateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbCreate);

		hr = device->CreateBuffer(&desc, nullptr, mCbCreateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Buffer.NumElements = mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		hr = device->CreateUnorderedAccessView(buffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	create_cs_from_cso(device, "Data/shader/fireworks_particle_create_cs.cso", mCSCreate1Shader.GetAddressOf());
	create_cs_from_cso(device, "Data/shader/fireworks_particle_cs.cso", mCSShader.GetAddressOf());

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

void FireworksParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("fireworks particle");
	ImGui::Text("emitor");
	float* position[3] = { &mEmitorData.position.x,&mEmitorData.position.y,&mEmitorData.position.z };
	ImGui::DragFloat3("position", *position, 0.5f);
	float* velocity[3] = { &mEmitorData.velocity.x,&mEmitorData.velocity.y,&mEmitorData.velocity.z };
	ImGui::SliderFloat3("velocity", *velocity, -1, 1);
	ImGui::InputFloat("max timer", &mEmitorData.maxTime, 1);
	ImGui::InputFloat("emitor speed", &mEmitorData.speed, 1);
	ImGui::Text("fireworks");
	ImGui::InputFloat("max life", &mFireworkData.maxLife, 1);
	ImGui::InputFloat("max count", &mFireworkData.maxCount, 1);
	ImGui::InputFloat("scale", &mFireworkData.scale, 0.1f);
	ImGui::InputFloat("speed", &mFireworkData.speed, 1);
	ImGui::InputFloat("parsent", &mFireworkData.parsent, 0.1f);
	ImGui::InputFloat("end timer", &mFireworkData.endTimer, 0.1f);
	ImGui::InputFloat("gravity", &mFireworkData.gravity, 0.1f);
	float* color[] = { &mFireworkData.color.x,&mFireworkData.color.y ,&mFireworkData.color.z };
	ImGui::ColorEdit3("color", *color);
	ImGui::SliderFloat("color w", &mFireworkData.color.w, 0, 1);
	float* endColor[] = { &mFireworkData.endColor.x,&mFireworkData.endColor.y ,&mFireworkData.endColor.z };
	ImGui::ColorEdit3("end color", *endColor);
	ImGui::SliderFloat("end color w", &mFireworkData.endColor.w, 0, 1);

	std::string name = createFlag ? "end" : "start";
	if (ImGui::Button(name.c_str()))
	{
		if (createFlag)
		{
			mEmitors.clear();
			createFlag = false;
		}
		else
		{
			createFlag = true;
			mEmitors.emplace_back();
			mEmitors.back().position = mEmitorData.position;
			mEmitors.back().velocity = mEmitorData.velocity * mEmitorData.speed;
			mEmitors.back().maxTime = mEmitorData.maxTime;
		}
	}
	ImGui::End();
#endif
}

void FireworksParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);
	if (createFlag)
	{
		context->CSSetConstantBuffers(0, 1, mCbCreateBuffer.GetAddressOf());
		CbCreate cbCreate, cbCreate2;
		memset(&cbCreate, 0, sizeof(cbCreate));
		/*****************エミッタの更新*********************/
		int fireworksCount = 0;
		int emitorCount = 0;
		for (int i = 0; i < mEmitors.size(); i++)
		{
			auto& emitor = mEmitors[i];
			if (emitor.type == -1)
			{
				continue;
			}
			emitor.position += emitor.velocity * elapsdTime;
			emitor.timer += elapsdTime;
			if (emitor.timer > emitor.maxTime)
			{
				emitor.type = -1;
				cbCreate.createData[fireworksCount].position = emitor.position;
				cbCreate.createData[fireworksCount].velocity = emitor.velocity;
				cbCreate.createData[fireworksCount].firework = mFireworkData;
				fireworksCount++;
			}
			//else
			//{
			//	cbCreate.createData[emitorCount].maxTimer = emitor.maxTime;
			//	cbCreate.createData[emitorCount].position = emitor.position;
			//	cbCreate.createData[emitorCount].velocity = emitor.velocity*0.01f;
			//	cbCreate.createData[emitorCount].type = emitor.type;
			//	emitorCount++;
			//}
		}
		/*********************花火のパーティクル生成*****************************/
		if (fireworksCount>0)
		{
			context->CSSetShader(mCSCreate1Shader.Get(), nullptr, 0);
			context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &cbCreate, 0, 0);
			int count = 0;
			for (auto& create : cbCreate.createData)
			{
				count += create.firework.maxCount;
			}
			context->Dispatch(count, 1, 1);
		}
		/*********************打ち上げ中のパーティクル生成*****************************/
		//if (emitorCount > 0)
		//{
		//	context->CSSetShader(mCSCreate2Shader.Get(), nullptr, 0);
		//	context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &cbCreate2, 0, 0);

		//}
	}
	/*********************パーティクルの更新*****************************/
	context->CSSetConstantBuffers(1, 1, mCbUpdateBuffer.GetAddressOf());
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);

	CbUpdate cbUpdate;
	cbUpdate.elapsdime = elapsdTime;
	context->UpdateSubresource(mCbUpdateBuffer.Get(), 0, 0, &cbUpdate, 0, 0);
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	context->Dispatch(mMaxParticle / 100, 1, 1);

	ID3D11UnorderedAccessView* uav = nullptr;
	context->CSSetUnorderedAccessViews(2, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

	context->CSSetShader(nullptr, nullptr, 0);

}

void FireworksParticle::Render(ID3D11DeviceContext* context)
{
	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mMaxParticle, 0);
	mShader->Deactivate(context);

}
