#pragma once
#include <windows.h>

const LONG SCREEN_WIDTH = 1920;
const LONG SCREEN_HEIGHT = 1080;

//viewportÇÃÉZÉbÉgä÷êî
//void SetScreenSize(ID3D11DeviceContext* context)
//{
//	D3D11_VIEWPORT mViewport;
//	mViewport.TopLeftX = 0;
//	mViewport.TopLeftY = 0;
//	mViewport.Width = static_cast<float>(SCREEN_WIDTH);
//	mViewport.Height = static_cast<float>(SCREEN_HEIGHT);
//	mViewport.MinDepth = 0.0f;
//	mViewport.MaxDepth = 1.0f;
//	context->RSSetViewports(1, &mViewport);
//}