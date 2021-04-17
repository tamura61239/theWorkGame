#include "title_particle.h"
#include"misc.h"
#include"texture.h"

#include"shader.h"
#include"camera_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

TitleParticle::TitleParticle(ID3D11Device* device):mSceneChange(false), mStartIdex(0)
{
	::memset(&mEditorData, 0, sizeof(EditorData));
	std::vector<Particle>particles;
	//Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;
	mMaxParticle = 30000;
	particles.resize(mMaxParticle);
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

	hr = CreateCSFromCso(device, "Data/shader/title_particle_create_cs.cso", mCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/title_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/title_scene_change_particle_cs.cso", mSceneChangeCSShader.GetAddressOf());
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
	Load();

}
//パーティクルの初期化や更新に必要なデータのimgui
void TitleParticle::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("title particle");
	float* angleMovement[3] = { &mEditorData.angleMovement.x,&mEditorData.angleMovement.y,&mEditorData.angleMovement.z };
	ImGui::SliderFloat3("angleMovement", *angleMovement, -3.14f, 3.14f);
	ImGui::InputFloat("length", &mEditorData.leng, 1);
	static float moveSize = 1;
	ImGui::InputFloat("moveSize", &moveSize, 1);
	ImGui::InputFloat("startPosition.x", &mEditorData.startPosition.x, moveSize);
	ImGui::InputFloat("startPosition.y", &mEditorData.startPosition.y, moveSize);
	ImGui::InputFloat("startPosition.z", &mEditorData.startPosition.z, moveSize);
	ImGui::InputFloat("life", &mEditorData.life, 1);
	ImGui::InputFloat("moveLen", &mEditorData.moveLen, 1);
	float* sphereRatio[3] = { &mEditorData.sphereRatio.x,&mEditorData.sphereRatio.y ,&mEditorData.sphereRatio.z };
	ImGui::SliderFloat3("sphereRatio", *sphereRatio, 0, 1);
	float* color[4] = { &mEditorData.color.x,&mEditorData.color.y ,&mEditorData.color.z ,&mEditorData.color.w };
	ImGui::ColorEdit4("color", *color);
	ImGui::InputFloat("randSpeed", &mEditorData.randSpeed, 1);
	ImGui::InputFloat("defSpeed", &mEditorData.defSpeed, 1);
	ImGui::InputFloat("randMoveLength", &mEditorData.randMoveLength, 0.1f);
	ImGui::InputFloat("defMoveLength", &mEditorData.defMoveLength, 0.1f);
	ImGui::InputFloat("randScale", &mEditorData.randScale, 0.1f);
	ImGui::SliderFloat("randMoveAngle", &mEditorData.randMoveAngle, 3.14f, 3.14f);
	float* defVelocity[3] = { &mEditorData.defVelocity.x,&mEditorData.defVelocity.y ,&mEditorData.defVelocity.z };
	ImGui::SliderFloat3("def velocity", *defVelocity, -1, 1);
	ImGui::Checkbox("scene change", &mSceneChange);
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

	if (ImGui::Button("save"))Save();
	ImGui::Text("%f", mNewIndex);
	ImGui::End();
#endif
}

void TitleParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	Cb cb;
	CbStart cbStart;
	CbStart2 cbStart2;
	cb.elapsdTime = elapsdTime;
	if (mUAV.Get() == nullptr)return;
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);

	cb.angleMovement = mEditorData.angleMovement;
	cbStart.color = mEditorData.color;
	cbStart.startPosition = mEditorData.startPosition;
	cbStart.leng = mEditorData.leng;
	cbStart.sphereRatio = mEditorData.sphereRatio;
	cbStart.life = mEditorData.life;
	cbStart.moveLen = mEditorData.moveLen;
	cbStart.randSpeed = mEditorData.randSpeed;
	cbStart.defSpeed = mEditorData.defSpeed;
	cbStart.startIndex = mStartIdex;
	cbStart2.randMoveLength = mEditorData.randMoveLength;
	cbStart2.defMoveLength = mEditorData.defMoveLength;
	cbStart2.randMoveAngle = mEditorData.randMoveAngle;
	cbStart2.randScale = mEditorData.randScale;
	cbStart2.defVelocity = mEditorData.defVelocity;


	context->CSSetConstantBuffers(0, 1, mCbStartBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mCbStart2Buffer.GetAddressOf());
	context->CSSetConstantBuffers(2, 1, mCbBuffer.GetAddressOf());

	context->UpdateSubresource(mCbStartBuffer.Get(), 0, 0, &cbStart, 0, 0);
	context->UpdateSubresource(mCbStart2Buffer.Get(), 0, 0, &cbStart2, 0, 0);
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &cb, 0, 0);

	context->CSSetUnorderedAccessViews(0, 1, mUAV.GetAddressOf(), nullptr);
	if (!mSceneChange)
	{
		mNewIndex += 1000 * elapsdTime;
		float indexSize = mNewIndex - mStartIdex;
		if (indexSize >= 1)
		{
			context->CSSetShader(mCreateShader.Get(), nullptr, 0);

			context->Dispatch(static_cast<UINT>(indexSize), 1, 1);
			mStartIdex = mNewIndex;
		}
		if (mNewIndex > mMaxParticle)
		{
			mNewIndex = 0;
			mStartIdex = 0;
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
	ID3D11Buffer* buffers[] = { nullptr,nullptr ,nullptr};
	context->CSSetConstantBuffers(0, 3, buffers);

}

void TitleParticle::Render(ID3D11DeviceContext* context)
{
	if (mUAV.Get() == nullptr)return;

	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mMaxParticle, 0);
	mShader->Deactivate(context);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);

}

void TitleParticle::Load()
{
	FILE* fp;
	long size = 0;
	if (fopen_s(&fp, "Data/file/title_particle_data.bin", "rb") == 0)
	{
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(&mEditorData, size, 1, fp);
		fclose(fp);
	}
}

void TitleParticle::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/title_particle_data.bin", "wb");
	{
		fwrite(&mEditorData, sizeof(EditorData), 1, fp);
		fclose(fp);
	}

}
