#pragma once
#include<d3d11.h>
#include <wrl.h>
#include <assert.h>
#include <memory>
#include<vector>
#include<DirectXTex.h>
//デプスバッファクラス
class FrameBuffer
{
public:
	//コンストラクタ
	FrameBuffer() = default;
	FrameBuffer(ID3D11Device* device, int width, int height, bool msaaFlag = false, int subsamples = 1, DXGI_FORMAT renderTargetTexture2dFormat = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depthStencilTexture2dFormat = DXGI_FORMAT_R24G8_TYPELESS,
		bool renderTargetShaderResourceViewFlag = true, bool depthStencilShaderResourceViewFlag = true, bool mipsFlag = false);
	//デプスバッファのテクスチャを初期化する
	void Clear(ID3D11DeviceContext* context, float r = 0, float g = 0, float b = 0, float a = 1, unsigned int clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float depth = 1, unsigned char stencil = 0);
	//デプスバッファをセットする
	void Activate(ID3D11DeviceContext* context);
	//セットしたバッファを元に戻す
	void Deactivate(ID3D11DeviceContext* context);

	//getter
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetRenderTargetShaderResourceView() { return mRenderTargetShaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetDepthStencilShaderResourceView() { return mDepthStencilShaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>GetRenderargetView() { return mRenderTargetView; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>GetDepthStencilView() { return mDepthStencilView; }
	D3D11_VIEWPORT GetViewPort() { return mViewport; }
	//setter
	void SetPsTexture(ID3D11DeviceContext* context, const int number);
	void SetPsDepth(ID3D11DeviceContext* context, const int number);
	//SRVをddsファイル形式で保存する
	void SaveDDSFile(ID3D11DeviceContext* context, const wchar_t* fileName, ID3D11ShaderResourceView* srv);
	virtual~FrameBuffer() {}

protected:
	//レンダーターゲットビューのデータ
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mRenderTargetShaderResourceView;
	//デプスステンシルビューのデータ
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mDepthStencilShaderResourceView;
	//元のデータを保持する
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mDefaultRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDefaultDepthStencilView;
	unsigned int mViewportsNumber;
	D3D11_VIEWPORT mDefaultViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	//画面比率を保持
	D3D11_VIEWPORT mViewport;

};
//マルチレンダーターゲットクラス
class MulltiRenderTargetFunction
{
public:
	//コンストラクタ
	MulltiRenderTargetFunction():mViewportsNumber(0){}
	//setter
	void SetFrameBuffer(std::shared_ptr<FrameBuffer>frameBuffer)
	{
		mFrameBuffers.push_back(frameBuffer);
	}
	//デプスバッファを指定した分作る
	void DeleteFrameBuffer(const int num)
	{
		mFrameBuffers.erase(mFrameBuffers.begin() + num);
	}
	//デプスバッファを解放する
	void ClearFrameBuffer()
	{
		mFrameBuffers.clear();
	}
	//デプスバッファのテクスチャを初期化する
	void Clear(ID3D11DeviceContext* context, float r = 0, float g = 0, float b = 0, float a = 1, unsigned int clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float depth = 1, unsigned char stencil = 0);
	//デプスバッファをセットする
	void Activate(ID3D11DeviceContext* context);
	//セットしたバッファを元に戻す
	void Deactivate(ID3D11DeviceContext* context);
	//getter
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetDepthStencilShaderResourceView() 
	{ 
		if (mFrameBuffers.size() <= 0)return nullptr;
		return mFrameBuffers[0]->GetDepthStencilShaderResourceView(); 
	}

private:
	//デプスバッファ
	std::vector<std::shared_ptr<FrameBuffer>>mFrameBuffers;
	//元のデータ
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mDefaultRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDefaultDepthStencilView;
	unsigned int mViewportsNumber;
	D3D11_VIEWPORT mDefaultViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

};