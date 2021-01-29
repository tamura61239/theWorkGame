#include "run_particle.h"
#include"misc.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
#define TYPE 0

RunParticles::RunParticles(ID3D11Device* device, std::shared_ptr<PlayerAI>player) :mMaxParticle(0), mCreateTime(0), mTimer(0), mCreateCount(3), mIndexNum(0), mRenderCount(0)
{
	/************�萔�o�b�t�@�쐬**************/
	mCbBoneBuffer = std::make_unique<ConstantBuffer<CbBone>>(device);
	mCbCreateBuffer = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdateBuffer = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	int totalIndex = 0;
	HRESULT hr;
	mPlayer = player;
	/**************************���b�V���̃o�b�t�@��SRV*****************************/
	for (auto& mesh : player->GetCharacter()->GetModel()->GetModelResource()->GetMeshDatas())
	{
		Mesh meshData;

		Microsoft::WRL::ComPtr<ID3D11Buffer>indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>vertexBuffer;
		//�o�b�t�@����
		{
			D3D11_BUFFER_DESC desc = {};
			D3D11_SUBRESOURCE_DATA data = {};
			desc.ByteWidth = sizeof(ModelData::Vertex) * mesh.vertices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;//�X�e�[�W�̓��o�͂�OK�BGPU�̓��o��OK�B
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
			desc.StructureByteStride = sizeof(ModelData::Vertex);
			data.pSysMem = &mesh.vertices[0];
			hr = device->CreateBuffer(&desc, &data, vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			desc.ByteWidth = sizeof(int) * mesh.indices.size();
			desc.StructureByteStride = sizeof(int);
			data.pSysMem = &mesh.indices[0];
			hr = device->CreateBuffer(&desc, &data, indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		//srv����
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.Buffer.NumElements = mesh.vertices.size();
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			hr = device->CreateShaderResourceView(vertexBuffer.Get(), &desc, meshData.mVertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			desc.BufferEx.NumElements = mesh.indices.size();
			hr = device->CreateShaderResourceView(indexBuffer.Get(), &desc, meshData.mIndexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		}
		meshData.mMwshSize = mesh.indices.size() / 3;
		totalIndex += mesh.indices.size();
		mMeshs.push_back(meshData);
	}
	//�p�[�e�B�N���̍ő吔���v�Z
	int count = totalIndex / 10000;
	mMaxParticle = 100000 * (count + 1);
#if (TYPE==1)
	/*************************�o�b�t�@��UAV�̍쐬**********************/
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>particleBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>deleteIndexBuffer;
		{
			//�p�[�e�B�N���p�o�b�t�@
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));
			desc.ByteWidth = sizeof(Particle) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;//�X�e�[�W�̓��o�͂�OK�BGPU�̓��o��OK�B
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
			desc.StructureByteStride = sizeof(Particle);
			std::vector<Particle>particles;
			particles.resize(mMaxParticle);
			memset(particles.data(), 0, sizeof(Particle) * mMaxParticle);
			data.pSysMem = &particles[0];
			hr = device->CreateBuffer(&desc, &data, particleBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			//index�o�b�t�@
			desc.ByteWidth = sizeof(UINT) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;//�X�e�[�W�̓��o�͂�OK�BGPU�̓��o��OK�B
			desc.BindFlags = /*D3D11_BIND_INDEX_BUFFER |*/ D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
			desc.StructureByteStride = sizeof(UINT);
			std::vector<UINT>indices;
			indices.resize(mMaxParticle);
			memset(indices.data(), 0, sizeof(UINT) * mMaxParticle);
			data.pSysMem = &indices[0];
			for (auto& index : mParticleIndexBuffer)
			{
				hr = device->CreateBuffer(&desc, &data, index.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			for (int i = 0; i < mMaxParticle; i++)
			{
				indices[i] = i;
			}
			ZeroMemory(&data, sizeof(data));
			data.pSysMem = &indices[0];
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			hr = device->CreateBuffer(&desc, &data, deleteIndexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//Vertex�o�b�t�@
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			int size = sizeof(RenderParticle);
			desc.ByteWidth = sizeof(RenderParticle) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;
			std::vector<RenderParticle>renderParticles;
			renderParticles.resize(mMaxParticle);
			memset(renderParticles.data(), 0, sizeof(RenderParticle) * mMaxParticle);
			ZeroMemory(&data, sizeof(data));
			data.pSysMem = &renderParticles[0];
			hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			//�p�[�e�B�N���̃J�E���g�o�b�t�@
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			desc.ByteWidth = sizeof(ParticleCount);
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			ZeroMemory(&data, sizeof(data));
			ParticleCount particleCount;
			memset(&particleCount, 0, sizeof(particleCount));
			particleCount.deActiveParticleCount = mMaxParticle;
			data.pSysMem = &particleCount;
			hr = device->CreateBuffer(&desc, &data, mParticleCountBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			ZeroMemory(&desc, sizeof(desc));

			desc.ByteWidth = sizeof(UINT) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			data.pSysMem = &indices[0];
			hr = device->CreateBuffer(&desc, &data, mRenderIndexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		}
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//�p�[�e�B�N���o�b�t�@��UAV
		desc.Buffer.NumElements = mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//index�o�b�t�@��UAV
		for (int i = 0; i < 2; i++)
		{
			hr = device->CreateUnorderedAccessView(mParticleIndexBuffer[i].Get(), &desc, mParticleIndexUAV[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		hr = device->CreateUnorderedAccessView(deleteIndexBuffer.Get(), &desc, mDeleteIndexUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		ZeroMemory(&desc, sizeof(desc));
		//�`��f�[�^��UAV
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//�p�[�e�B�N���̃J�E���gUAV
		desc.Buffer.NumElements = sizeof(ParticleCount) / 4;
		hr = device->CreateUnorderedAccessView(mParticleCountBuffer.Get(), &desc, mParticleCountUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
#elif (TYPE==0)
	//�p�[�e�B�N��
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>particleBuffer;
		//Microsoft::WRL::ComPtr<ID3D11Buffer>deactiveParticleBuffer;
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));
			desc.ByteWidth = sizeof(Particle) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;//�X�e�[�W�̓��o�͂�OK�BGPU�̓��o��OK�B
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
			desc.StructureByteStride = sizeof(Particle);

			std::vector<Particle>particles;
			//std::vector<Particle>deactiveParticles;

			particles.resize(mMaxParticle);
			//deactiveParticles.resize(mMaxParticle);
			memset(particles.data(), 0, sizeof(Particle) * mMaxParticle);

			data.pSysMem = &particles[0];

			hr = device->CreateBuffer(&desc, &data, particleBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//�`��p�o�b�t�@
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			int size = sizeof(RenderParticle);

			desc.ByteWidth = sizeof(RenderParticle) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;
			std::vector<RenderParticle>renderParticles;
			renderParticles.resize(mMaxParticle);
			memset(renderParticles.data(), 0, sizeof(RenderParticle) * mMaxParticle);
			data.pSysMem = &renderParticles[0];
			hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Buffer.NumElements = mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		ZeroMemory(&desc, sizeof(desc));

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
#endif
	hr = CreateCSFromCso(device, "Data/shader/run_particle_create_cs.cso", mCreateCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/run_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_count_cs.cso", mStartCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	mCbCreateBuffer->data.startIndex = 0;
	mCbCreateBuffer->data.life = 0.5f;
	mCreateTime = 0.05f;
	Load();
}



void RunParticles::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("run particle");
	ImGui::ColorEdit4("color", mColor);
	ImGui::SliderInt(u8"1���b�V������o��p�[�e�B�N����", &mCreateCount, 0, 10);
	ImGui::InputFloat(u8"�p�[�e�B�N�����o���Ԋu", &mCreateTime, 0.1f);
	ImGui::InputFloat(u8"���x", &mCbCreateBuffer->data.speed, 0.1f);
	ImGui::InputFloat("life", &mCbCreateBuffer->data.life, 0.1f);
	ImGui::Text("%d", mRenderCount);
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::End();
#endif
}


void RunParticles::Update(ID3D11DeviceContext* context, float elapsd_time)
{
#if (TYPE==0)
	if (mPlayer.lock()->GetPlayFlag())mTimer += elapsd_time;
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);
	//�p�[�e�B�N���̐���

	if (mTimer > mCreateTime)
	{
		context->CSSetShader(mCreateCSShader.Get(), nullptr, 0);
		const ModelResource* resouce = mPlayer.lock()->GetCharacter()->GetModel()->GetModelResource();
		const std::vector<Model::Node>& nodes = mPlayer.lock()->GetCharacter()->GetModel()->GetNodes();
		mCbCreateBuffer->data.color = 0;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mColor[0] * 255) & 0x00FFFFFF) << 24;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mColor[1] * 255) & 0x00FFFFFF) << 16;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mColor[2] * 255) & 0x00FFFFFF) << 8;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mColor[3] * 255) & 0x00FFFFFF) << 0;
		if (mCreateCount > 0)
		{
			for (int i = 0; i < mMeshs.size(); i++)
			{
				const auto& mesh = mMeshs[i];
				const auto& boneData = resouce->GetMeshes()[i];
				if (mCbCreateBuffer->data.startIndex + mesh.mMwshSize * mCreateCount >= mMaxParticle)
				{
					mCbCreateBuffer->data.startIndex -= mMaxParticle;
				}
				if (boneData.nodeIndices.size() > 0)
				{
					//�{�[���̃��[���h�s��̌v�Z
					for (int j = 0; j < boneData.nodeIndices.size(); j++)
					{
						DirectX::XMMATRIX inverseTransform = DirectX::XMLoadFloat4x4(boneData.inverseTransforms[j]);
						DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes[boneData.nodeIndices[j]].worldTransform);
						DirectX::XMMATRIX boneTransform = inverseTransform * worldTransform;
						DirectX::XMStoreFloat4x4(&mCbBoneBuffer->data.boneTransForm[j], boneTransform);
					}
				}
				else
				{
					mCbBoneBuffer->data.boneTransForm[0] = nodes[boneData.nodeIndex].worldTransform;
				}
				mCbCreateBuffer->data.indexCount = mesh.mMwshSize;
				mCbCreateBuffer->Activate(context, 1, false, false, false, true);
				mCbBoneBuffer->Activate(context, 0, false, false, false, true);
				ID3D11ShaderResourceView* srv[2] = { mesh.mVertexBuffer.Get(),mesh.mIndexBuffer.Get() };
				context->CSSetShaderResources(0, 2, srv);
				context->Dispatch(mesh.mMwshSize * mCreateCount, 1, 1);
				mCbCreateBuffer->DeActivate(context);
				mCbBoneBuffer->DeActivate(context);
				srv[0] = nullptr;
				srv[1] = nullptr;
				context->CSSetShaderResources(0, 2, srv);
				mCbCreateBuffer->data.startIndex += mesh.mMwshSize * mCreateCount;

			}
			mTimer = 0;

		}
	}
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	//�p�[�e�B�N���̍X�V
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	mCbUpdateBuffer->data.elapsdTime = elapsd_time;
	mCbUpdateBuffer->Activate(context, 2, false, false, false, true);

	context->Dispatch(mMaxParticle/100, 1, 1);
	mCbUpdateBuffer->DeActivate(context);
	ID3D11UnorderedAccessView* uav[3] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 3, uav, nullptr);
	context->CSSetShader(nullptr, nullptr, 0);
#elif (TYPE==1)
	/*******************UAV��GPU�ɓn��******************/
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(1, 1, mParticleCountUAV.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(3, 1, mParticleIndexUAV[mIndexNum].GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(4, 1, mParticleIndexUAV[1 - mIndexNum].GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(5, 1, mDeleteIndexUAV.GetAddressOf(), nullptr);
	/****************�p�[�e�B�N������*****************/
	if (mPlayer.lock()->GetPlayFlag())mTimer += elapsd_time;
	mIndexNum++;
	if (mIndexNum >= 2)mIndexNum = 0;

	if (mTimer >= mCreateTime)
	{
		context->CSSetShader(mCreateCSShader.Get(), nullptr, 0);
		const ModelResource* resouce = mPlayer.lock()->GetCharacter()->GetModel()->GetModelResource();
		const std::vector<Model::Node>& nodes = mPlayer.lock()->GetCharacter()->GetModel()->GetNodes();
		mCbCreateBuffer->data.color = 0;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mColor[0] * 255) & 0x00FFFFFF) << 24;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mColor[1] * 255) & 0x00FFFFFF) << 16;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mColor[2] * 255) & 0x00FFFFFF) << 8;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mColor[3] * 255) & 0x00FFFFFF) << 0;
		if (mCreateCount > 0)
		{

			for (int i = 0; i < mMeshs.size(); i++)
			{
				const auto& mesh = mMeshs[i];
				const auto& boneData = resouce->GetMeshes()[i];
				if (boneData.nodeIndices.size() > 0)
				{
					//�{�[���̃��[���h�s��̌v�Z
					for (int j = 0; j < boneData.nodeIndices.size(); j++)
					{
						DirectX::XMMATRIX inverseTransform = DirectX::XMLoadFloat4x4(boneData.inverseTransforms[j]);
						DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes[boneData.nodeIndices[j]].worldTransform);
						DirectX::XMMATRIX boneTransform = inverseTransform * worldTransform;
						DirectX::XMStoreFloat4x4(&mCbBoneBuffer->data.boneTransForm[j], boneTransform);
					}
				}
				else
				{
					mCbBoneBuffer->data.boneTransForm[0] = nodes[boneData.nodeIndex].worldTransform;
				}
				mCbCreateBuffer->data.indexCount = mesh.mMwshSize;
				mCbCreateBuffer->Activate(context, 1, false, false, false, true);
				mCbBoneBuffer->Activate(context, 0, false, false, false, true);
				ID3D11ShaderResourceView* srv[2] = { mesh.mVertexBuffer.Get(),mesh.mIndexBuffer.Get() };
				context->CSSetShaderResources(0, 2, srv);
				context->Dispatch(mesh.mMwshSize * mCreateCount, 1, 1);
				mCbCreateBuffer->DeActivate(context);
				mCbBoneBuffer->DeActivate(context);
				srv[0] = nullptr;
				srv[1] = nullptr;
				context->CSSetShaderResources(0, 2, srv);

			}
			mTimer = 0;
		}
	}
	/**************�p�[�e�B�N���̍X�V*****************/
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	mCbUpdateBuffer->data.elapsdTime = elapsd_time;
	mCbUpdateBuffer->Activate(context, 2, false, false, false, true);

	context->Dispatch(mMaxParticle / 100, 1, 1);
	mCbUpdateBuffer->DeActivate(context);
	/******************�p�[�e�B�N�����̍X�V*******************/
	context->CSSetShader(mStartCSShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);

	ID3D11UnorderedAccessView* uav[6] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 6, uav, nullptr);
	context->CSSetShader(nullptr, nullptr, 0);
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mParticleCountBuffer.Get(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mRenderCount = particleCount->aliveParticleCount;
	context->Unmap(mParticleCountBuffer.Get(), NULL);
	context->CopyResource(mParticleIndexBuffer[mIndexNum].Get(), mRenderIndexBuffer.Get());
#endif
}

void RunParticles::Render(ID3D11DeviceContext* context)
{
#if (TYPE==0)

	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mMaxParticle, 0);
	mShader->Deactivate(context);
#elif (TYPE==1)
	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* vertex = mRenderBuffer.Get();
	ID3D11Buffer* index = mRenderIndexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mRenderCount, 0);
	mShader->Deactivate(context);
	offset = 0;
	vertex = nullptr;
	index = nullptr;
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

#endif
}

void RunParticles::Render(ID3D11DeviceContext* context, DrowShader* shader)
{
#if (TYPE==0)

	//shader->Activate(context);

	//u_int stride = sizeof(RenderParticle);
	//u_int offset = 0;

	//context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//context->Draw(mMaxParticle, 0);
	//shader->Deactivate(context);
#elif (TYPE==1)
	//shader->Activate(context);

	//u_int stride = sizeof(RenderParticle);
	//u_int offset = 0;

	//ID3D11Buffer* vertex = mRenderBuffer.Get();
	//ID3D11Buffer* index = mRenderIndexBuffer.Get();
	//context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	//context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//context->Draw(mRenderCount, 0);
	//shader->Deactivate(context);
	//offset = 0;
	//vertex = nullptr;
	//index = nullptr;
	//context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	//context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

#endif

}



void RunParticles::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/run_particle.bin", "rb") == 0)
	{
		fread(&mColor[0], sizeof(float), 4, fp);
		fread(&mCbCreateBuffer->data.life, sizeof(float), 1, fp);
		fread(&mCbCreateBuffer->data.speed, sizeof(float), 1, fp);
		fread(&mCreateTime, sizeof(float), 1, fp);

		fclose(fp);
	}
}

void RunParticles::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/run_particle.bin", "wb");
	fwrite(&mColor[0], sizeof(float), 4, fp);
	fwrite(&mCbCreateBuffer->data.life, sizeof(float), 1, fp);
	fwrite(&mCbCreateBuffer->data.speed, sizeof(float), 1, fp);
	fwrite(&mCreateTime, sizeof(float), 1, fp);
	fclose(fp);
}
