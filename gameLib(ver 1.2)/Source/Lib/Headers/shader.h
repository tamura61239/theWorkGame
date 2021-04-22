#pragma once
#include <d3d11.h>
//シェーダーを生成する
//VS
HRESULT CreateVSFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc=nullptr, UINT num_elements=0);
//PS
HRESULT CreatePSFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);
//GS
HRESULT CreateGSFromCso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);
//CS
HRESULT CreateCSFromCso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);


