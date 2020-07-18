#pragma once
#include<d3d11.h>
#include <wrl.h>
#include "misc.h"

enum class BLEND_MODE { NONE, ALPHA, ADD, SUBTRACT, REPLACE, MULTIPLY, LIGHTEN, DARKEN, SCREEN, ALPHA_TO_COVERAGE, END };

namespace Descartes
{
	ID3D11BlendState* create_blend_state(ID3D11Device* device, BLEND_MODE mode);

}
class blend_state
{
public:
	Microsoft::WRL::ComPtr<ID3D11BlendState> state_object;
	blend_state(ID3D11Device* device, BLEND_MODE mode)
	{
		state_object.Attach(Descartes::create_blend_state(device, mode));
	}
	blend_state(ID3D11Device* device, const D3D11_BLEND_DESC* blend_desc)
	{
		HRESULT hr = device->CreateBlendState(blend_desc, state_object.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	~blend_state() = default;
	blend_state(blend_state&) = delete;
	blend_state& operator =(blend_state&) = delete;

	void activate(ID3D11DeviceContext* immediate_context)
	{
		UINT sample_mask = 0xFFFFFFFF;
		immediate_context->OMGetBlendState(default_state_object.ReleaseAndGetAddressOf(), 0, &sample_mask);
		immediate_context->OMSetBlendState(state_object.Get(), 0, 0xFFFFFFFF);
	}
	void deactivate(ID3D11DeviceContext* immediate_context)
	{
		immediate_context->OMSetBlendState(default_state_object.Get(), 0, 0xFFFFFFFF);
	}
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> default_state_object;
};
