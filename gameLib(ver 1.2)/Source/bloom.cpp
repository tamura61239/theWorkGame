#include "bloom.h"
#include"shader.h"
#include"misc.h"
#include"vector.h"

Bloom::Bloom(ID3D11Device* device, float screenWidth, float screenHight)
{
	unsigned int wight = static_cast<unsigned int>(screenWidth);
	unsigned int hight = static_cast<unsigned int>(screenHight);
	for (int i = 0;i < 5;i++)
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

	hr = create_ps_from_cso(device, "Data/shader/bloom_ps.cso", mPSShader[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "Data/shader/combined_bloom.cso", mPSShader[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//SamplerState‚Ì¶¬
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 16;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		desc.MaxAnisotropy = 16;
		memcpy(desc.BorderColor, &VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f), sizeof(VECTOR4F));
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

}

void Bloom::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv, bool render)
{

	mFrameBuffer[0]->Clear(context);
	mFrameBuffer[0]->Activate(context);
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

	for (int i = 1;i < 5;i++)
	{
		mFrameBuffer[i]->Clear(context);
		mFrameBuffer[i]->Activate(context);
		context->VSSetShader(mVSShader.Get(), 0, 0);
		context->PSSetShader(mPSShader[0].Get(), 0, 0);
		context->PSSetShaderResources(0, 1, mFrameBuffer[i - 1]->GetRenderTargetShaderResourceView().GetAddressOf());
		context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
		context->RSSetState(mRasterizeState.Get());
		context->IASetInputLayout(nullptr);
		context->IASetVertexBuffers(0, 0, 0, 0, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		context->Draw(4, 0);
		mFrameBuffer[i]->Deactivate(context);
	}
	if (!render)
	{
		mFrameBuffer[5]->Clear(context);
		mFrameBuffer[5]->Activate(context);
		context->VSSetShader(mVSShader.Get(), 0, 0);
		context->PSSetShader(mPSShader[1].Get(), 0, 0);
		for (int i = 0;i < 5;i++)
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
	context->PSSetShader(mPSShader[1].Get(), 0, 0);
	for (int i = 0;i < 5;i++)
	{
		context->PSSetShaderResources(i, 1, mFrameBuffer[i]->GetRenderTargetShaderResourceView().GetAddressOf());
	}
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizeState.Get());
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);

}
