#include "cs_buffer.h"

/********************************************/
//                 ‰Šú‰»ŠÖ”
/********************************************/
/**********************UAV‚âSRV‚Ì¶¬**********************/
void CSBuffer::CreateView(ID3D11Device* device, UINT miscFlags, UINT classSize, UINT elementCount, bool uav, bool srv)
{
	HRESULT hr;
	if (uav)
	{//UAV‚Ì¶¬
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		if (miscFlags == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			desc.Buffer.NumElements = elementCount;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		}
		else
		{
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			desc.Buffer.NumElements = (classSize * elementCount) / 4;

		}
		hr = device->CreateUnorderedAccessView(mBuffer.Get(), &desc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	if (srv)
	{//SRV¶¬
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		if (miscFlags == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			desc.Buffer.NumElements = elementCount;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		}
		else
		{
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.BufferEx.FirstElement = 0;
			desc.BufferEx.Flags = D3D11_SRV_DIMENSION_BUFFEREX;
			desc.BufferEx.NumElements = (classSize * elementCount) / 4 / 4;
		}
		hr = device->CreateShaderResourceView(mBuffer.Get(), &desc, mSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}

}
