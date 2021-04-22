#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>
#include"misc.h"
//サンプラーステートクラス
class SamplerState
{
public:
	//コンストラクタ
	SamplerState(ID3D11Device* device, D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_NEVER, VECTOR4F color = VECTOR4F(1, 1, 1, 1))
	{
		//サンプラーステート
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
	//サンプラーステートをセットする
	void Activate(ID3D11DeviceContext* context, UINT slot, bool vs = false, bool ps = false, bool gs = false, bool cs = false)
	{
		if (vs)
		{
			context->VSGetSamplers(slot, 1, mDefaltSampler[0].GetAddressOf());
			context->VSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		}
		if (ps)
		{
			context->PSGetSamplers(slot, 1, mDefaltSampler[1].GetAddressOf());
			context->PSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		}
		if (gs)
		{
			context->GSGetSamplers(slot, 1, mDefaltSampler[2].GetAddressOf());
			context->GSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		}
		if (cs)
		{
			context->CSGetSamplers(slot, 1, mDefaltSampler[3].GetAddressOf());
			context->CSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		}
		mSlotNum = slot;
		mSetFlag[0] = vs;
		mSetFlag[1] = ps;
		mSetFlag[2] = gs;
		mSetFlag[3] = cs;

	}
	//セットしたサンプラーステートを元に戻す
	void DeActivate(ID3D11DeviceContext* context)
	{
		if (mSetFlag[0])context->VSSetSamplers(mSlotNum, 1, mDefaltSampler[0].GetAddressOf());
		if (mSetFlag[1])context->PSSetSamplers(mSlotNum, 1, mDefaltSampler[1].GetAddressOf());
		if (mSetFlag[2])context->GSSetSamplers(mSlotNum, 1, mDefaltSampler[2].GetAddressOf());
		if (mSetFlag[3])context->CSSetSamplers(mSlotNum, 1, mDefaltSampler[3].GetAddressOf());
	}
private:
	//サンプラーステート
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState;
	//セット前のサンプラーステートを保持
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mDefaltSampler[4] = { 0 };
	//GPU側の登録番号
	UINT mSlotNum;
	//セットしたシェーダーを知る
	bool mSetFlag[4] = { false };

};