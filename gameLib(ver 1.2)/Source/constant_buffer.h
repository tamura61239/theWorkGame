#pragma once
#include<d3d11.h>
#include <wrl.h>
#include"misc.h"

//定数バッファクラス
template <class T>
class ConstantBuffer
{
public:
	//データ
	T data{};
	//コンストラクタ
	ConstantBuffer(ID3D11Device* device)
	{
		_ASSERT_EXPR(sizeof(T) % 16 == 0, L"constant buffer's need to be 16 byte aligned");
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(T);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = device->CreateBuffer(&desc, 0, mConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//定数バッファをGPU側に送る
	void Activate(ID3D11DeviceContext* context,UINT slot,bool vs=false,bool ps=false,bool gs=false,bool cs=false)
	{
		HRESULT hr = S_OK;

		D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
		D3D11_MAPPED_SUBRESOURCE mapped_buffer;

		hr = context->Map(mConstantBuffer.Get(), 0, map, 0, &mapped_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		memcpy_s(mapped_buffer.pData, sizeof(T), &data, sizeof(T));
		context->Unmap(mConstantBuffer.Get(), 0);
		
		if (vs)context->VSSetConstantBuffers(slot, 1, mConstantBuffer.GetAddressOf());
		if (ps)context->PSSetConstantBuffers(slot, 1, mConstantBuffer.GetAddressOf());
		if (gs)context->GSSetConstantBuffers(slot, 1, mConstantBuffer.GetAddressOf());
		if (cs)context->CSSetConstantBuffers(slot, 1, mConstantBuffer.GetAddressOf());
		mSlotNum = slot;
		mSetFlag[0] = vs;
		mSetFlag[1] = ps;
		mSetFlag[2] = gs;
		mSetFlag[3] = cs;
	}
	//解除
	void DeActivate(ID3D11DeviceContext* context)
	{
		ID3D11Buffer* buffer = nullptr;
		if (mSetFlag[0])context->VSSetConstantBuffers(mSlotNum, 1, &buffer);
		if (mSetFlag[1])context->PSSetConstantBuffers(mSlotNum, 1, &buffer);
		if (mSetFlag[2])context->GSSetConstantBuffers(mSlotNum, 1, &buffer);
		if (mSetFlag[3])context->CSSetConstantBuffers(mSlotNum, 1, &buffer);

	}
private:
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mConstantBuffer;
	//GPU側の登録番号
	UINT mSlotNum;
	//どのシェーダーにデータを送ったか
	bool mSetFlag[4] = { false };
};