#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>
#include"misc.h"

class SamplerState
{
public:
	SamplerState(ID3D11Device* device, D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_NEVER, VECTOR4F color = VECTOR4F(1, 1, 1, 1))
	{
		HRESULT hr = S_OK;
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = filter;
		desc.AddressU = addressMode;
		desc.AddressV = addressMode;
		desc.AddressW = addressMode;
		desc.ComparisonFunc = comparisonFunc;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 1;
		memcpy(desc.BorderColor, &color, sizeof(VECTOR4F));
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;

		hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	void Activate(ID3D11DeviceContext* context, UINT slot, bool vs = false, bool ps = false, bool gs = false, bool cs = false)
	{
		if (vs)context->VSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		if (ps)context->PSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		if (gs)context->GSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		if (cs)context->CSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		mSlotNum = slot;
		mSetFlag[0] = vs;
		mSetFlag[1] = ps;
		mSetFlag[2] = gs;
		mSetFlag[3] = cs;

	}
	void DeActivate(ID3D11DeviceContext* context)
	{
		ID3D11Buffer* buffer = nullptr;
		if (mSetFlag[0])context->VSSetConstantBuffers(mSlotNum, 1, &buffer);
		if (mSetFlag[1])context->PSSetConstantBuffers(mSlotNum, 1, &buffer);
		if (mSetFlag[2])context->GSSetConstantBuffers(mSlotNum, 1, &buffer);
		if (mSetFlag[3])context->CSSetConstantBuffers(mSlotNum, 1, &buffer);
	}
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState;
	UINT mSlotNum;
	bool mSetFlag[4] = { false };

};