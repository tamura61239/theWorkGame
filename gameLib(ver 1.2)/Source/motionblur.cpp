#include "motionblur.h"
#include"shader.h"
#include"misc.h"

MotionBlur::MotionBlur(ID3D11Device* device)
{
	HRESULT hr;
	hr = CreateCSFromCso(device, "Data/shader/motionblur_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//desc.Width = 1920;
		//desc.Height = 1080;
		//desc.MipLevels = 0;
		//desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		//desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		//desc.CPUAccessFlags = 0;
		desc.Width = 1920/3;
		desc.Height = 1080/3;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = device->CreateTexture2D(&desc, nullptr, mTexture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;
		hr = device->CreateUnorderedAccessView(mTexture2d.Get(), &desc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(mTexture2d.Get(), &desc, mSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}

void MotionBlur::CreateNeighborMaxBuffer(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv)
{
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	context->CSSetShaderResources(0, 1, &srv);
	context->CSSetUnorderedAccessViews(0, 1, mUAV.GetAddressOf(), 0);

	context->Dispatch(640, 360, 1);

	context->CSSetShader(nullptr, nullptr, 0);
	ID3D11ShaderResourceView* s = nullptr;
	context->CSSetShaderResources(0, 1, &s);
	ID3D11UnorderedAccessView* u = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &u, 0);

}

void MotionBlur::SetPsTexture(ID3D11DeviceContext* context, const int number)
{
	context->PSSetShaderResources(number, 1, mSRV.GetAddressOf());
}
