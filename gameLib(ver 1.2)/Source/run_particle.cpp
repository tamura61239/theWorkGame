#include "run_particle.h"
#include"misc.h"
#include"shader.h"
#include"texture.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
#define TYPE 1

RunParticles::RunParticles(ID3D11Device* device, std::shared_ptr<PlayerAI>player) :mMaxParticle(0), mTimer(0), mIndexNum(0), mRenderCount(0), mTestFlag(false)
{
	memset(&mEditorData, 0, sizeof(mEditorData));
	memset(&mEditorData.mColor, 1, sizeof(mEditorData.mColor));
	mEditorData.mCreateCount = 3;
	/************定数バッファ作成**************/
	mCbBoneBuffer = std::make_unique<ConstantBuffer<CbBone>>(device);
	mCbCreateBuffer = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdateBuffer = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	int totalIndex = 0;
	HRESULT hr;
	mPlayer = player;
	/**************************メッシュのバッファとSRV*****************************/
	for (auto& mesh : player->GetCharacter()->GetModel()->GetModelResource()->GetMeshDatas())
	{
		Mesh meshData;

		Microsoft::WRL::ComPtr<ID3D11Buffer>indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>vertexBuffer;
		//バッファ生成
		{
			D3D11_BUFFER_DESC desc = {};
			D3D11_SUBRESOURCE_DATA data = {};
			desc.ByteWidth = sizeof(ModelData::Vertex) * mesh.vertices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
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
		//srv生成
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
	//パーティクルの最大数を計算
	int count = totalIndex / 10000;
	mMaxParticle = 100000 * (count + 1);
#if (TYPE==1)
	/*************************バッファとUAVの作成**********************/
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>particleBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>deleteIndexBuffer;
		{
			//パーティクル用バッファ
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));
			desc.ByteWidth = sizeof(Particle) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
			desc.StructureByteStride = sizeof(Particle);
			std::vector<Particle>particles;
			particles.resize(mMaxParticle);
			memset(particles.data(), 0, sizeof(Particle) * mMaxParticle);
			data.pSysMem = &particles[0];
			hr = device->CreateBuffer(&desc, &data, particleBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			ZeroMemory(&desc, sizeof(desc));

			//indexバッファ
			desc.ByteWidth = sizeof(UINT) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS; // 構造化バッファ
			desc.StructureByteStride = 0;
			std::vector<UINT>indices;
			indices.resize(mMaxParticle);
			memset(indices.data(), 0, sizeof(UINT) * mMaxParticle);
			data.pSysMem = &indices[0];
			for (auto& index : mParticleIndexBuffer)
			{
				hr = device->CreateBuffer(&desc, &data, index.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			//deleteindexバッファ
			for (int i = 0; i < mMaxParticle; i++)
			{
				indices[i] = i;
			}
			ZeroMemory(&data, sizeof(data));
			data.pSysMem = &indices[0];
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
			desc.StructureByteStride = sizeof(UINT);
			desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。

			hr = device->CreateBuffer(&desc, &data, deleteIndexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//Vertexバッファ
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
			//パーティクルのカウントバッファ
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

		}
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//パーティクルバッファのUAV
		desc.Buffer.NumElements = mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//indexバッファのUAV
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = sizeof(UINT) * mMaxParticle / 4;

		for (int i = 0; i < 2; i++)
		{
			hr = device->CreateUnorderedAccessView(mParticleIndexBuffer[i].Get(), &desc, mParticleIndexUAV[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		ZeroMemory(&desc, sizeof(desc));

		desc.Buffer.NumElements = mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

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
		//パーティクルのカウントUAV
		desc.Buffer.NumElements = sizeof(ParticleCount) / 4;
		hr = device->CreateUnorderedAccessView(mParticleCountBuffer.Get(), &desc, mParticleCountUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
#elif (TYPE==0)
	//パーティクル
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>particleBuffer;
		//Microsoft::WRL::ComPtr<ID3D11Buffer>deactiveParticleBuffer;
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));
			desc.ByteWidth = sizeof(Particle) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
			desc.StructureByteStride = sizeof(Particle);

			std::vector<Particle>particles;
			//std::vector<Particle>deactiveParticles;

			particles.resize(mMaxParticle);
			//deactiveParticles.resize(mMaxParticle);
			memset(particles.data(), 0, sizeof(Particle) * mMaxParticle);

			data.pSysMem = &particles[0];

			hr = device->CreateBuffer(&desc, &data, particleBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//描画用バッファ
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
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
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

	FileFunction::Load(mEditorData, "Data/file/run_particle.bin", "rb");
}



void RunParticles::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("run particle");
	ImGui::ColorEdit4("color", mEditorData.mColor);
	ImGui::SliderInt(u8"1メッシュから出るパーティクル数", &mEditorData.mCreateCount, 0, 10);
	ImGui::InputFloat(u8"パーティクルを出す間隔", &mEditorData.mCreateTime, 0.1f);
	ImGui::InputFloat(u8"速度", &mEditorData.speed, 0.1f);
	ImGui::InputFloat("life", &mEditorData.life, 0.1f);
	ImGui::Checkbox("test", &mTestFlag);
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

	ImGui::Text("%d", mRenderCount);

	if (ImGui::Button("save"))
	{
		FileFunction::Save(mEditorData, "Data/file/run_particle.bin", "wb");
	}
	ImGui::End();
#endif
}


void RunParticles::Update(ID3D11DeviceContext* context, float elapsd_time)
{
#if (TYPE==0)
	if (mPlayer.lock()->GetPlayFlag())mTimer += elapsd_time;
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);
	//パーティクルの生成

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
					//ボーンのワールド行列の計算
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

	//パーティクルの更新
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	mCbUpdateBuffer->data.elapsdTime = elapsd_time;
	mCbUpdateBuffer->Activate(context, 2, false, false, false, true);

	context->Dispatch(mMaxParticle/100, 1, 1);
	mCbUpdateBuffer->DeActivate(context);
	ID3D11UnorderedAccessView* uav =  nullptr ;
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(2, 1, &uav, nullptr);
	context->CSSetShader(nullptr, nullptr, 0);
#elif (TYPE==1)
	/*******************UAVをGPUに渡す******************/
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(1, 1, mParticleCountUAV.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(3, 1, mParticleIndexUAV[mIndexNum].GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(4, 1, mParticleIndexUAV[1 - mIndexNum].GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(5, 1, mDeleteIndexUAV.GetAddressOf(), nullptr);
	/****************パーティクル生成*****************/
	if (mPlayer.lock()->GetPlayFlag()|| mTestFlag)mTimer += elapsd_time;
	mIndexNum++;
	if (mIndexNum >= 2)mIndexNum = 0;

	if (mTimer >= mEditorData.mCreateTime)
	{
		context->CSSetShader(mCreateCSShader.Get(), nullptr, 0);
		const ModelResource* resouce = mPlayer.lock()->GetCharacter()->GetModel()->GetModelResource();
		const std::vector<Model::Node>& nodes = mPlayer.lock()->GetCharacter()->GetModel()->GetNodes();
		mCbCreateBuffer->data.color = 0;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[0] * 255) & 0x00FFFFFF) << 24;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[1] * 255) & 0x00FFFFFF) << 16;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[2] * 255) & 0x00FFFFFF) << 8;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[3] * 255) & 0x00FFFFFF) << 0;
		if (mEditorData.mCreateCount > 0)
		{
			mCbCreateBuffer->data.speed = mEditorData.speed;
			mCbCreateBuffer->data.life = mEditorData.life;

			for (int i = 0; i < static_cast<int>(mMeshs.size()); i++)
			{
				const auto& mesh = mMeshs[i];
				const auto& boneData = resouce->GetMeshes()[i];
				if (boneData.nodeIndices.size() > 0)
				{
					//ボーンのワールド行列の計算
					for (int j = 0; j < static_cast<int>(boneData.nodeIndices.size()); j++)
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
				context->Dispatch(mesh.mMwshSize * mEditorData.mCreateCount, 1, 1);
				mCbCreateBuffer->DeActivate(context);
				mCbBoneBuffer->DeActivate(context);
				srv[0] = nullptr;
				srv[1] = nullptr;
				context->CSSetShaderResources(0, 2, srv);

			}
			mTimer = 0;
		}
	}
	/**************パーティクルの更新*****************/
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	mCbUpdateBuffer->data.elapsdTime = elapsd_time;
	mCbUpdateBuffer->Activate(context, 2, false, false, false, true);

	context->Dispatch(mMaxParticle / 100, 1, 1);
	mCbUpdateBuffer->DeActivate(context);
	/******************パーティクル数の更新*******************/
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

	//context->CopyResource(mParticleIndexBuffer[mIndexNum].Get(), mRenderIndexBuffer.Get());
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
	stride = 0;
	ID3D11Buffer* buffer = nullptr;
	context->IASetVertexBuffers(0, 1,&buffer, &stride, &offset);

#elif (TYPE==1)
	mShader->Activate(context);
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* vertex = mRenderBuffer.Get();
	ID3D11Buffer* index = mParticleIndexBuffer[mIndexNum].Get();
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	context->DrawIndexed(mRenderCount, 0,0);
	mShader->Deactivate(context);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);

	//offset = 0;
	//vertex = nullptr;
	//index = nullptr;
	//context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	//context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

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


