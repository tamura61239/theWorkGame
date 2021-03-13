#pragma once
#include<d3d11.h>
#include <wrl.h>
#include"misc.h"

//ラスタライザーステート
class RasterizerState
{
public:
	RasterizerState(ID3D11Device* device, D3D11_FILL_MODE fillMode = D3D11_FILL_WIREFRAME, D3D11_CULL_MODE cullMode = D3D11_CULL_NONE, bool clockwise = false, bool depthClip = false, bool scissor = false, bool multisample = false, bool antialiasedLine = false)
	{
		HRESULT hr = S_OK;
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = fillMode;
		desc.CullMode = cullMode;
		desc.FrontCounterClockwise = clockwise;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = depthClip;
		desc.ScissorEnable = scissor;
		desc.MultisampleEnable = multisample;
		desc.AntialiasedLineEnable = antialiasedLine;
		hr = device->CreateRasterizerState(&desc, mRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	void Activate(ID3D11DeviceContext* context)
	{
		context->RSGetState(mDefaultState.GetAddressOf());
		context->RSSetState(mRasterizerState.Get());
	}
	void DeActivate(ID3D11DeviceContext* context)
	{
		context->RSSetState(mDefaultState.Get());
	}
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mDefaultState;
};
//深度ステンシルステート
class DepthStencilState
{
public:
	DepthStencilState(ID3D11Device* device, bool depth = true, D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_FUNC comparison = D3D11_COMPARISON_LESS)
	{
		HRESULT hr = S_OK;
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = depth;
		desc.DepthWriteMask = depthWriteMask;
		desc.DepthFunc = comparison;
		desc.StencilEnable = FALSE;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	void Activate(ID3D11DeviceContext* context)
	{
		context->OMGetDepthStencilState(mDefaultState.GetAddressOf(), nullptr);
		context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	}
	void DeActive(ID3D11DeviceContext* context)
	{
		context->OMSetDepthStencilState(mDefaultState.Get(), 0);
	}
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDefaultState;

};