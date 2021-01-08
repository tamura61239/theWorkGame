#pragma once
#include <d3d11.h>

HRESULT CreateVSFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc=nullptr, UINT num_elements=0);
HRESULT CreatePSFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);
HRESULT CreateGSFromCso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);
HRESULT CreateCSFromCso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);


