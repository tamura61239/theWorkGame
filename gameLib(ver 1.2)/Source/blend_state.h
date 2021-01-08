#pragma once
#include<d3d11.h>
#include <wrl.h>
#include "misc.h"

enum class BLEND_MODE { NONE, ALPHA, ADD, SUBTRACT, REPLACE, MULTIPLY, LIGHTEN, DARKEN, SCREEN, ALPHA_TO_COVERAGE, END };

namespace Descartes
{
	ID3D11BlendState* CreateBlendState(ID3D11Device* device, BLEND_MODE mode);

}
class BlendState
{
public:
	Microsoft::WRL::ComPtr<ID3D11BlendState> state_object;
	BlendState(ID3D11Device* device, BLEND_MODE mode)
	{
		state_object.Attach(Descartes::CreateBlendState(device, mode));
	}
	BlendState(ID3D11Device* device, const D3D11_BLEND_DESC* blend_desc)
	{
		HRESULT hr = device->CreateBlendState(blend_desc, state_object.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	~BlendState() = default;
	BlendState(BlendState&) = delete;
	BlendState& operator =(BlendState&) = delete;

	void activate(ID3D11DeviceContext* context)
	{
		UINT sampleMask = 0xFFFFFFFF;
		context->OMGetBlendState(mDefaultStateObject.ReleaseAndGetAddressOf(), 0, &sampleMask);
		context->OMSetBlendState(state_object.Get(), 0, 0xFFFFFFFF);
	}
	void deactivate(ID3D11DeviceContext* context)
	{
		context->OMSetBlendState(mDefaultStateObject.Get(), 0, 0xFFFFFFFF);
	}
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> mDefaultStateObject;
};
