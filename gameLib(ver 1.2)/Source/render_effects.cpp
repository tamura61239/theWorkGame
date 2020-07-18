#include "render_effects.h"
#include"shader.h"
#include"misc.h"

RenderEffects::RenderEffects(ID3D11Device* device)
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout>input;

	hr=create_vs_from_cso(device, "Data/shader/render_effects_vs.cso", mVSShader.GetAddressOf(), input.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr=create_ps_from_cso(device, "Data/shader/render_effects_ps.cso", mPSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(CbScene);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//SamplerState‚Ì¶¬
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 16;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		desc.MaxAnisotropy = 16;
		memcpy(desc.BorderColor, &VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f), sizeof(VECTOR4F));
		hr = device->CreateSamplerState(&desc, mSamplerState[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		hr = device->CreateSamplerState(&desc, mSamplerState[0].GetAddressOf());
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

		//desc.DepthEnable = TRUE;
		//desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		//desc.DepthFunc = D3D11_COMPARISON_LESS;
		//desc.StencilEnable = FALSE;
		//desc.StencilReadMask = 0xFF;
		//desc.StencilWriteMask = 0xFF;
		//desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		//desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		//desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		//desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		//desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		//desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		//desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		//desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		//desc.DepthEnable = false;
		//desc.StencilEnable = false;
		//desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		//desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		//desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		//desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		//desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		//desc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
		//desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		//desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		//desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		//desc.BackFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;

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

		//rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		//rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		//rasterizerDesc.FrontCounterClockwise = FALSE;
		//rasterizerDesc.DepthBias = 0;
		//rasterizerDesc.DepthBiasClamp = 0;
		//rasterizerDesc.SlopeScaledDepthBias = 0;
		//rasterizerDesc.DepthClipEnable = true;
		//rasterizerDesc.ScissorEnable = FALSE;
		//rasterizerDesc.MultisampleEnable = false;
		//rasterizerDesc.AntialiasedLineEnable = FALSE;


		hr = device->CreateRasterizerState(&rasterizerDesc, mRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}

void RenderEffects::ShadowRender(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorMapSRV, ID3D11ShaderResourceView* depthMapSRV, ID3D11ShaderResourceView* shadowMapSRV
	, const FLOAT4X4& view, const FLOAT4X4& projection, const FLOAT4X4& lightView, const FLOAT4X4& lightProjection)
{
	CbScene cbScene;
	DirectX::XMStoreFloat4x4(&cbScene.lightViewProjection, DirectX::XMLoadFloat4x4(&lightView) * DirectX::XMLoadFloat4x4(&lightProjection));
	DirectX::XMStoreFloat4x4(&cbScene.inverseViewProjection, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection)));
	
	context->VSSetConstantBuffers(0, 1, mCbBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, mCbBuffer.GetAddressOf());
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &cbScene, 0, 0);

	context->VSSetShader(mVSShader.Get(), 0, 0);
	context->PSSetShader(mPSShader.Get(), 0, 0);

	context->PSSetShaderResources(0, 1, &colorMapSRV);
	context->PSSetShaderResources(1, 1, &depthMapSRV);
	context->PSSetShaderResources(2, 1, &shadowMapSRV);

	context->PSSetSamplers(0, 1, mSamplerState[0].GetAddressOf());
	context->PSSetSamplers(1, 1, mSamplerState[1].GetAddressOf());

	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizerState.Get());
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	context->Draw(4, 0);

	ID3D11ShaderResourceView* nullSRV[3] = {};
	context->PSSetShaderResources(0, 3, nullSRV);
}
