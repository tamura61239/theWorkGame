#include "drow_shader.h"
#include"shader.h"
#include "misc.h"


/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

DrowShader::DrowShader(ID3D11Device* device, const char* vsName, const char* gsName, const char* psName, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
	HRESULT hr;
	if (strcmp(vsName, "") != 0)
	{//VSシェーダーの生成
		hr = CreateVSFromCso(device, vsName, mVSShader.GetAddressOf(), mInput.GetAddressOf(), input_element_desc, num_elements);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	if (strcmp(gsName, "") != 0)
	{//GSシェーダーの生成
		hr = CreateGSFromCso(device, gsName, mGSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	if (strcmp(psName, "") != 0)
	{//PSシェーダーの生成
		hr = CreatePSFromCso(device, psName, mPSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}
/*****************************************************/
//　　　　　　　　　　シェーダーを使用する関数
/*****************************************************/
/****************************設定***************************/
void DrowShader::Activate(ID3D11DeviceContext* context)
{
	context->VSSetShader(mVSShader.Get(), nullptr, 0);
	context->IASetInputLayout(mInput.Get());
	if (!mGSShader) {}
	else context->GSSetShader(mGSShader.Get(), nullptr, 0);
	context->PSSetShader(mPSShader.Get(), nullptr, 0);
}
/****************************元に戻す***************************/
void DrowShader::Deactivate(ID3D11DeviceContext* context)
{
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
}
