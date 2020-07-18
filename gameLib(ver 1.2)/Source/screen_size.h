#pragma once
#include <windows.h>

const LONG SCREEN_WIDTH = 1920;
const LONG SCREEN_HEIGHT = 1080;

//viewportÇÃÉZÉbÉgä÷êî
//void SetScreenSize(ID3D11DeviceContext* context)
//{
//	D3D11_VIEWPORT viewport;
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.Width = static_cast<float>(SCREEN_WIDTH);
//	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
//	viewport.MinDepth = 0.0f;
//	viewport.MaxDepth = 1.0f;
//	context->RSSetViewports(1, &viewport);
//}