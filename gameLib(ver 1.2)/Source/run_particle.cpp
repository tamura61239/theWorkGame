#include "run_particle.h"
#include"misc.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif


RunParticles::RunParticles(ID3D11Device* device): mRenderSize(0),mPlayFlag(false), mNewIndex(0), mTestFlag(false)
{
	std::vector<Particle>particles;
	std::vector<RenderParticle>renderParticles;
	renderParticles.resize(30000);
	mRenderSize = renderParticles.size();
	particles.resize(mRenderSize);
	Microsoft::WRL::ComPtr<ID3D11Buffer>particleBuffer;
	HRESULT hr;
	//バッファの作成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.ByteWidth = sizeof(Particle) * mRenderSize;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, particleBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = sizeof(RenderParticle) * mRenderSize;
		data.pSysMem = &renderParticles[0];
		hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//定数バッファ
		desc.ByteWidth = sizeof(CbeBone);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbBoneBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbCreateData);
		hr = device->CreateBuffer(&desc, nullptr, mCbCreateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbUpdate);
		hr = device->CreateBuffer(&desc, nullptr, mCbUpdateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//UAVの作成
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.Buffer.NumElements = sizeof(Particle) * mRenderSize / 4;
		hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Buffer.NumElements = sizeof(RenderParticle) * mRenderSize / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	create_cs_from_cso(device, "Data/shader/run_particle_create_cs.cso", mCreateShader.GetAddressOf());
	create_cs_from_cso(device, "Data/shader/run_particle_cs.cso", mUpdateShader.GetAddressOf());
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_cube_mesh_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	mCbCreateData.mStartNumber = 0;
	memset(&mEditorData, 0, sizeof(mEditorData));
	Load();
}



void RunParticles::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("run");
	ImGui::Checkbox("new", &mTestFlag);
	if (!mTestFlag && !mPlayFlag)
	{
		mCbCreateData.mStartNumber = 0;
		mNewIndex = 0;
	}
	ImGui::InputFloat("CreateSize", &mEditorData.mCreateSize, 10);
	ImGui::InputFloat("max life", &mEditorData.maxLife, 1);
	ImGui::InputFloat("scale", &mEditorData.scale, 1);
	ImGui::InputFloat("speed", &mEditorData.speed, 10);
	float* color[4] = { &mEditorData.color.x,&mEditorData.color.y ,&mEditorData.color.z ,&mEditorData.color.w };
	ImGui::ColorEdit4("color", *color);
	if (mTestFlag)
	{
		float* velocity[3] = { &mTestVelocity.x,&mTestVelocity.y,&mTestVelocity.z };
		ImGui::DragFloat3("testVelocity", *velocity);
	}
	ImGui::Text("%f", mCbCreateData.mStartNumber);
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::End();
#endif
}

void RunParticles::SetBoneData(Model* model)
{
	const ModelResource* model_resource = model->GetModelResource();
	const std::vector<Model::Node>& nodes = model->GetNodes();
	mCbBones.clear();
	for (const ModelResource::Mesh& mesh:model->GetModelResource()->GetMeshes())
	{
		::memset(&mCbBone, 0, sizeof(&mCbBone));
		mCbBone.boneNumber = nodes.size();
		for (int i = 0; i < mCbBone.boneNumber; i++)
		{
			auto& node = nodes.at(i);
			FLOAT4X4 world = node.world_transform;
			mCbBone.boneWorld[i] = VECTOR4F(world._41, world._42, world._43, world._44);
		}
	}

}


void RunParticles::SetPlayerData(const VECTOR3F& velocity, const bool playFlag)
{
	mCbCreateData.velocity = -velocity*0.0f;
	mPlayFlag = playFlag;
}

void RunParticles::Update(ID3D11DeviceContext* context, float elapsd_time)
{
	ID3D11Buffer* cbBuffer[] =
	{
		mCbBoneBuffer.Get(),
		mCbCreateBuffer.Get(),
		mCbUpdateBuffer.Get()
	};
	context->CSSetConstantBuffers(0, ARRAYSIZE(cbBuffer), cbBuffer);
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), 0);
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), 0);
	if (mPlayFlag|| mTestFlag)
	{
		context->CSSetShader(mCreateShader.Get(), nullptr, 0);
		//for (auto& data : mVertexDatas)
		//{
		//	if (mCbCreateData.mStartNumber >= mRenderSize)
		//	{
		//		mCbCreateData.mStartNumber -= mRenderSize;
		//	}
		//	context->UpdateSubresource(mCbBoneBuffer.Get(), 0, 0, &data.mCbBone, 0, 0);
		//	context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &mCbCreateData, 0, 0);
		//	context->CSSetShaderResources(0, 1, data.mMeshSRV.GetAddressOf());
		//	context->Dispatch(data.mNumber, 1, 1);
		//	mCbCreateData.mStartNumber += data.mNumber;
		//}
		//for (auto& bone : mCbBones)
		//{
		//	if (mCbCreateData.mStartNumber >= mRenderSize)
		//	{
		//		mCbCreateData.mStartNumber = 0;
		//	}
		//	context->UpdateSubresource(mCbBoneBuffer.Get(), 0, 0, &bone, 0, 0);
		//	context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &mCbCreateData, 0, 0);
		//	context->Dispatch(bone.boneNumber, 1, 1);
		//	mCbCreateData.mStartNumber += bone.boneNumber;
		//}
		//mPlayFlag = false;
		mNewIndex += mEditorData.mCreateSize * elapsd_time;
		float createAmount = mNewIndex - mCbCreateData.mStartNumber;
		if (createAmount > 0)
		{
			if (mTestFlag)mCbCreateData.velocity = mTestVelocity;
			mCbCreateData.maxLife = mEditorData.maxLife;
			mCbCreateData.color = mEditorData.color;
			mCbCreateData.scale = mEditorData.scale;
			mCbCreateData.speed = mEditorData.speed;
			context->UpdateSubresource(mCbBoneBuffer.Get(), 0, 0, &mCbBone, 0, 0);
			context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &mCbCreateData, 0, 0);
			mCbCreateData.mStartNumber = mNewIndex;
			if (mNewIndex + createAmount > mRenderSize)
			{
				mNewIndex = 0;
				mCbCreateData.mStartNumber = 0;
			}
			context->Dispatch(createAmount, 1, 1);
		}
	}
	CbUpdate cbUpdate;
	cbUpdate.elapsdTime = elapsd_time;
	context->UpdateSubresource(mCbUpdateBuffer.Get(), 0, 0, &cbUpdate, 0, 0);
	context->CSSetShader(mUpdateShader.Get(), nullptr, 0);
	context->Dispatch(mRenderSize/100, 1, 1);

	context->CSSetShader(nullptr, nullptr, 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->CSSetShaderResources(0, 1, &srv);
	ID3D11UnorderedAccessView* uavs[] =
	{
		nullptr,
		nullptr,
		nullptr
	};
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, 0);

}

void RunParticles::Render(ID3D11DeviceContext* context)
{
	if (mRenderBuffer.Get() == nullptr)return;
	if (mShader.get() == nullptr)return;

	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mRenderSize, 0);
	mShader->Deactivate(context);
}


void RunParticles::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/run_paricte_data.bin", "rb") == 0)
	{
		fseek(fp, 0, SEEK_END);
		long size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		fread(&mEditorData, size, 1, fp);
		fclose(fp);

	}

}

void RunParticles::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/run_paricte_data.bin", "wb");
	fwrite(&mEditorData, sizeof(mEditorData), 1, fp);
	fclose(fp);
}
