#pragma once
#include<d3d11.h>
#include <wrl.h>
#include <assert.h>
#include <memory>
#include<vector>

class FrameBuffer
{
public:
	FrameBuffer(ID3D11Device* device, int width, int height, bool msaaFlag = false, int subsamples = 1, DXGI_FORMAT renderTargetTexture2dFormat = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depthStencilTexture2dFormat = DXGI_FORMAT_R24G8_TYPELESS,
		bool renderTargetShaderResourceViewFlag = true, bool depthStencilShaderResourceViewFlag = true, bool mipsFlag = false);
	//初期化
	void Clear(ID3D11DeviceContext* context, float r = 0, float g = 0, float b = 0, float a = 1, unsigned int clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float depth = 1, unsigned char stencil = 0);
	//開始
	void Activate(ID3D11DeviceContext* context);
	//終了
	void Deactivate(ID3D11DeviceContext* context);

	//getter
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetRenderTargetShaderResourceView() { return mRenderTargetShaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetDepthStencilShaderResourceView() { return mDepthStencilShaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>GetRenderargetView() { return mRenderTargetView; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>GetDepthStencilView() { return mDepthStencilView; }
	D3D11_VIEWPORT GetViewPort() { return viewport; }
	void SetPsTexture(ID3D11DeviceContext* context, const int number);
	void SetPsDepth(ID3D11DeviceContext* context, const int number);
private:
	//render target view data
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mRenderTargetShaderResourceView;
	//depth stencil view data
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mDepthStencilShaderResourceView;
	//default view data
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mDefaultRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDefaultDepthStencilView;
	unsigned int viewportsNumber;
	D3D11_VIEWPORT defaultViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	D3D11_VIEWPORT viewport;

};
// Multi render targets 
class MulltiRenderTargetFunction
{
public:
	MulltiRenderTargetFunction():viewportsNumber(0){}
	void SetFrameBuffer(std::shared_ptr<FrameBuffer>frameBuffer)
	{
		frameBuffers.push_back(frameBuffer);
	}
	void DeleteFrameBuffer(const int num)
	{
		frameBuffers.erase(frameBuffers.begin() + num);
	}
	void ClearFrameBuffer()
	{
		frameBuffers.clear();
	}
	//初期化
	void Clear(ID3D11DeviceContext* context, float r = 0, float g = 0, float b = 0, float a = 1, unsigned int clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float depth = 1, unsigned char stencil = 0);
	//開始
	void Activate(ID3D11DeviceContext* context);
	//終了
	void Deactivate(ID3D11DeviceContext* context);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetDepthStencilShaderResourceView() 
	{ 
		if (frameBuffers.size() <= 0)return nullptr;
		return frameBuffers[0]->GetDepthStencilShaderResourceView(); 
	}

private:
	std::vector<std::shared_ptr<FrameBuffer>>frameBuffers;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mDefaultRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDefaultDepthStencilView;
	unsigned int viewportsNumber;
	D3D11_VIEWPORT defaultViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

};