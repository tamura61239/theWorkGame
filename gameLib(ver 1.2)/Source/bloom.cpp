#include "bloom.h"
#include"shader.h"
#include"misc.h"
#include"vector.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

BloomRender::BloomRender(ID3D11Device* device, float screenWidth, float screenHight) :count(3), deviation(2.5f), multiply(1.0f)
{
	unsigned int wight = static_cast<unsigned int>(screenWidth);
	unsigned int hight = static_cast<unsigned int>(screenHight);
	for (int i = 0; i < 5; i++)
	{
		mFrameBuffer.emplace_back(std::make_unique<FrameBuffer>(device, static_cast<int>(wight >> i), static_cast<float>(hight >> i), true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS));
	}
	mFrameBuffer.emplace_back(std::make_unique<FrameBuffer>(device, screenWidth, screenHight, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS));

	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout>input;

	hr = create_vs_from_cso(device, "Data/shader/bloom_vs.cso", mVSShader.GetAddressOf(), input.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "Data/shader/bloomStart_ps.cso", mPSShader[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "Data/shader/bloom_ps.cso", mPSShader[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "Data/shader/combined_bloom.cso", mPSShader[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//SamplerState‚Ì¶¬
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 16;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		desc.MaxAnisotropy = 16;
		memcpy(desc.BorderColor, &VECTOR4F(.0f, .0f, .0f, .0f), sizeof(VECTOR4F));
		hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//DepthStencilState‚Ì¶¬
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = false;
		desc.StencilEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.AntialiasedLineEnable = true;


		hr = device->CreateRasterizerState(&rasterizerDesc, mRasterizeState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		CD3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbBloom);
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCBbuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbBluer);
		hr = device->CreateBuffer(&desc, nullptr, mCbBluerbuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	Load();
}

void BloomRender::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("bloom");
	ImGui::SliderInt("filter count", &count, 0, 4);
	ImGui::SliderFloat("threshold", &mCbuffer.threshold, 0, 10);
	ImGui::SliderFloat("widthBlur", &mCbuffer.widthBlur, 0, 1);
	ImGui::SliderFloat("hightBlur", &mCbuffer.hightBlur, 0, 1);
	ImGui::SliderFloat("devutation", &deviation, 0, 50);
	ImGui::SliderFloat("multiply", &multiply, 0, 10);
	int blurCount = static_cast<int>(mCbuffer.blurCount);
	if (ImGui::SliderInt("blurCount", &blurCount, 1, 8))
	{
		mCbuffer.blurCount = static_cast<float>(blurCount);
	}
	if (ImGui::Button("save"))Save();
	ImGui::End();
#endif
}

void BloomRender::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv, bool render)
{
	ID3D11Buffer* buffer[] =
	{
		mCBbuffer.Get(),
		mCbBluerbuffer.Get()
	};
	context->PSSetConstantBuffers(0, 2, buffer);
	context->VSSetConstantBuffers(0, 2, buffer);
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	mFrameBuffer[0]->Clear(context);
	if (count >= 0)
	{
		mFrameBuffer[0]->Activate(context);
		context->UpdateSubresource(mCBbuffer.Get(), 0, 0, &mCbuffer, 0, 0);
		context->VSSetShader(mVSShader.Get(), 0, 0);
		context->PSSetShader(mPSShader[0].Get(), 0, 0);
		context->PSSetShaderResources(0, 1, &colorSrv);
		context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
		context->RSSetState(mRasterizeState.Get());
		context->IASetInputLayout(nullptr);
		context->IASetVertexBuffers(0, 0, 0, 0, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		context->Draw(4, 0);
		mFrameBuffer[0]->Deactivate(context);
	}
	//Ý’è
	context->VSSetShader(mVSShader.Get(), 0, 0);
	context->PSSetShader(mPSShader[1].Get(), 0, 0);
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizeState.Get());
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 1; i < 5; i++)
	{
		mFrameBuffer[i]->Clear(context);
		if (count < i)continue;
		mFrameBuffer[i]->Activate(context);
		D3D11_VIEWPORT viewport = mFrameBuffer[i]->GetViewPort();
		CalucurateBluer(viewport.Width, viewport.Height, VECTOR2F(mCbuffer.widthBlur, mCbuffer.hightBlur), deviation, multiply);
		context->UpdateSubresource(mCbBluerbuffer.Get(), 0, 0, &mCbBluer, 0, 0);

		context->PSSetShaderResources(0, 1, mFrameBuffer[i - 1]->GetRenderTargetShaderResourceView().GetAddressOf());
		context->Draw(4, 0);
		mFrameBuffer[i]->Deactivate(context);
	}
	if (!render)
	{
		mFrameBuffer[5]->Clear(context);
		mFrameBuffer[5]->Activate(context);
		context->VSSetShader(mVSShader.Get(), 0, 0);
		context->PSSetShader(mPSShader[2].Get(), 0, 0);
		for (int i = 0; i < 5; i++)
		{
			context->PSSetShaderResources(i, 1, mFrameBuffer[i]->GetRenderTargetShaderResourceView().GetAddressOf());
		}
		context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
		context->RSSetState(mRasterizeState.Get());
		context->IASetInputLayout(nullptr);
		context->IASetVertexBuffers(0, 0, 0, 0, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		context->Draw(4, 0);
		mFrameBuffer[5]->Deactivate(context);
		return;
	}
	context->VSSetShader(mVSShader.Get(), 0, 0);
	context->PSSetShader(mPSShader[2].Get(), 0, 0);
	context->PSSetShaderResources(0, 1, &colorSrv);
	for (int i = 0; i < 5; i++)
	{
		context->PSSetShaderResources(i + 1, 1, mFrameBuffer[i]->GetRenderTargetShaderResourceView().GetAddressOf());
	}
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizeState.Get());
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);

}

void BloomRender::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/bloom.bin", "rb") == 0)
	{
		//fread(&mCbuffer, sizeof(CbBloom), 1, fp);
		//fread(&deviation, sizeof(float), 1, fp);
		//fread(&multiply, sizeof(float), 1, fp);
		//fclose(fp);
		int err = static_cast<int>(fread(&mCbuffer, sizeof(CbBloom), 1, fp));
		assert(err >= 1);
		err = static_cast<int>(fread(&deviation, sizeof(float), 1, fp));
		assert(err >= 1);
		err = static_cast<int>(fread(&multiply, sizeof(float), 1, fp));
		assert(err >= 1);
		err = static_cast<int>(fclose(fp));
		assert(err != EOF);

		return;
	}
	mCbuffer.threshold = 0;
	mCbuffer.widthBlur = 0;
	mCbuffer.hightBlur = 0;
	mCbuffer.blurCount = 8;

}

void BloomRender::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/bloom.bin", "wb");
	int err = static_cast<int>(fwrite(&mCbuffer, sizeof(CbBloom), 1, fp));
	assert(err >= 1);
	err = static_cast<int>(fwrite(&deviation, sizeof(float), 1, fp));
	assert(err >= 1);
	err = static_cast<int>(fwrite(&multiply, sizeof(float), 1, fp));
	assert(err >= 1);
	err = static_cast<int>(fclose(fp));
	assert(err != EOF);

}

float BloomRender::GaussianDistribution(const VECTOR2F& position, const float rho)
{
	return exp(-(position.x * position.x + position.y * position.y) / (2.0f * rho * rho));
}

void BloomRender::CalucurateBluer(const float width, const float hight, const VECTOR2F& dir, const float deviation, const float multiply)
{
	float uvX = 1.0f / width;
	float uvY = 1.0f / hight;

	mCbBluer.mOffset[0].z = GaussianDistribution(VECTOR2F(0, 0), deviation) * multiply;
	float totalWeigh = mCbBluer.mOffset[0].z;

	mCbBluer.mOffset[0].x = 0;
	mCbBluer.mOffset[0].y = 0;
	for (int i = 1; i < 8; i++)
	{
		mCbBluer.mOffset[i].x = dir.x * i * uvX;
		mCbBluer.mOffset[i].y = dir.y * i * uvY;
		mCbBluer.mOffset[i].z = GaussianDistribution(dir * float(i), deviation) * multiply;
		totalWeigh += mCbBluer.mOffset[i].z * 2.0f;
	}
	for (int i = 0; i < 8; i++)
	{
		mCbBluer.mOffset[i].z /= totalWeigh;
	}
}
