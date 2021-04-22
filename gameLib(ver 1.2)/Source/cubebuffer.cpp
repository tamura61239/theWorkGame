#include "cubebuffer.h"
#include"misc.h"
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

CubeBuffer::CubeBuffer(ID3D11Device* device, int width, int height, bool msaaFlag, int subsamples, DXGI_FORMAT renderTargetTexture2dFormat, int arraySize, DXGI_FORMAT depthStencilTexture2dFormat, bool renderTargetShaderResourceViewFlag, bool depthStencilShaderResourceViewFlag, bool mipsFlag)
{
	HRESULT hr = S_OK;
	UINT qualityLevel;
	UINT sampleCount = subsamples;
	hr = device->CheckMultisampleQualityLevels(renderTargetTexture2dFormat, sampleCount, &qualityLevel);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	if (renderTargetTexture2dFormat != DXGI_FORMAT_UNKNOWN)
	{
		//レンダーターゲット用のバッファ生成
		D3D11_TEXTURE2D_DESC texture2dDesc = {};
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = arraySize;
		texture2dDesc.Format = renderTargetTexture2dFormat;
		texture2dDesc.SampleDesc.Count = msaaFlag ? sampleCount : 1;
		texture2dDesc.SampleDesc.Quality = msaaFlag ? qualityLevel - 1 : 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = renderTargetShaderResourceViewFlag ? D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_RENDER_TARGET;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS| D3D11_RESOURCE_MISC_TEXTURECUBE;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//レンダーターゲットの生成
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		renderTargetViewDesc.Format = renderTargetTexture2dFormat;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		renderTargetViewDesc.Texture2DArray.MipSlice = 0;
		renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
		renderTargetViewDesc.Texture2DArray.ArraySize = arraySize;
		hr = device->CreateRenderTargetView(texture2d.Get(), &renderTargetViewDesc, mRenderTargetView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (renderTargetShaderResourceViewFlag)
		{
			//レンダーターゲットに描画したものを書き込むテクスチャの生成
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
			shaderResourceViewDesc.Format = renderTargetTexture2dFormat;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
			shaderResourceViewDesc.TextureCube.MipLevels = 1;
			hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, mRenderTargetShaderResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

	}
	if (depthStencilTexture2dFormat != DXGI_FORMAT_UNKNOWN)
	{
		//デプスステンシルビュー用のバッファ生成

		D3D11_TEXTURE2D_DESC texture2dDesc = {};
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = arraySize;
		texture2dDesc.Format = DXGI_FORMAT_D32_FLOAT;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		//デプスステンシルビュー用のバッファ生成

		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		depthStencilViewDesc.Texture2DArray.ArraySize = arraySize;
		depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
		depthStencilViewDesc.Texture2DArray.MipSlice = 0;
		depthStencilViewDesc.Flags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		hr = device->CreateDepthStencilView(texture2d.Get(), &depthStencilViewDesc, mDepthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (depthStencilShaderResourceViewFlag)
		{
			//デプスステンシルビューで描画した深度値を書き込むテクスチャの生成

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
			shaderResourceViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
			shaderResourceViewDesc.TextureCube.MipLevels = 1;
			hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, mDepthStencilShaderResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}
	mViewport.Width = static_cast<float>(width);
	mViewport.Height = static_cast<float>(height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;

}
