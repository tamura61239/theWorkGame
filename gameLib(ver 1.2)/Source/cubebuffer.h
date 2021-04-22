#pragma once
#include"framebuffer.h"

//キューブマップ用のデプスバッファクラス
class CubeBuffer :public FrameBuffer
{
public:
	//コンストラクタ
	CubeBuffer(ID3D11Device* device, int width, int height, bool msaaFlag = false, int subsamples = 1, DXGI_FORMAT renderTargetTexture2dFormat = DXGI_FORMAT_R8G8B8A8_UNORM, int arraySize = 6,
		DXGI_FORMAT depthStencilTexture2dFormat = DXGI_FORMAT_R32_TYPELESS,
		bool renderTargetShaderResourceViewFlag = true, bool depthStencilShaderResourceViewFlag = true, bool mipsFlag = true);
};