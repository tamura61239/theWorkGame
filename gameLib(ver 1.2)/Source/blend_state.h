#pragma once
#include<d3d11.h>
#include <wrl.h>
#include "misc.h"
//ブレンドの種類
enum class BLEND_MODE { NONE, ALPHA, ADD, SUBTRACT, REPLACE, MULTIPLY, LIGHTEN, DARKEN, SCREEN, ALPHA_TO_COVERAGE, END };

namespace Descartes
{
	//生成
	ID3D11BlendState* CreateBlendState(ID3D11Device* device, BLEND_MODE mode);

}
//ブレンドステートクラス
class BlendState
{
public:
	Microsoft::WRL::ComPtr<ID3D11BlendState> state_object;
	//コンストラクタ(ブレンドの種類を選択する)
	BlendState(ID3D11Device* device, BLEND_MODE mode)
	{
		state_object.Attach(Descartes::CreateBlendState(device, mode));
	}
	//コンストラクタ(ブレンドデータからステートを作る)
	BlendState(ID3D11Device* device, const D3D11_BLEND_DESC* blend_desc)
	{
		HRESULT hr = device->CreateBlendState(blend_desc, state_object.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	~BlendState() = default;
	BlendState(BlendState&) = delete;
	BlendState& operator =(BlendState&) = delete;
	//ブレンドステートを設定
	void activate(ID3D11DeviceContext* context)
	{
		UINT sampleMask = 0xFFFFFFFF;
		//元のステートを取得しとく
		context->OMGetBlendState(mDefaultStateObject.ReleaseAndGetAddressOf(), 0, &sampleMask);
		context->OMSetBlendState(state_object.Get(), 0, 0xFFFFFFFF);
	}
	//設定したのを元に戻す
	void deactivate(ID3D11DeviceContext* context)
	{
		//取得しといたステートをセットする
		context->OMSetBlendState(mDefaultStateObject.Get(), 0, 0xFFFFFFFF);
	}
private:
	//元のブレントステート
	Microsoft::WRL::ComPtr<ID3D11BlendState> mDefaultStateObject;
};
