#pragma once
#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"misc.h"

class CSBuffer
{
public:
	template<class T>
	//配列でない時
	CSBuffer(ID3D11Device* device, D3D11_USAGE usage, UINT bindFlags, UINT miscFlags, T obj, bool uav = false, bool srv = false,UINT cpuAccessFlag=0)
	{
		HRESULT hr;

		//bufferの生成
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = usage;
			desc.BindFlags = bindFlags;
			desc.MiscFlags = miscFlags;
			desc.StructureByteStride = miscFlags == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED ? sizeof(T) : 0;
			desc.ByteWidth = sizeof(T);
			desc.CPUAccessFlags = cpuAccessFlag;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&data, sizeof(data));
			data.pSysMem = &obj;
			hr = device->CreateBuffer(&desc, &data, mBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		//UAVやSRVの生成
		CreateView(device, miscFlags, sizeof(T), 1, uav, srv);
	}
	template<class T>
	//vector版
	CSBuffer(ID3D11Device* device, D3D11_USAGE usage, UINT bindFlags, UINT miscFlags, std::vector<T> objs, bool uav = false, bool srv = false, UINT cpuAccessFlag = 0)
	{
		HRESULT hr;

		//bufferの生成
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = usage;
			desc.BindFlags = bindFlags;
			desc.MiscFlags = miscFlags;
			desc.StructureByteStride = miscFlags == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED ? sizeof(T) : 0;
			desc.ByteWidth = objs.size() * sizeof(T);
			desc.CPUAccessFlags = cpuAccessFlag;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&data, sizeof(data));
			data.pSysMem = &objs[0];
			hr = device->CreateBuffer(&desc, &data, mBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		//UAVやSRVの生成
		CreateView(device, miscFlags, sizeof(T), objs.size(), uav, srv);
	}
	//データをGPU側に送る
	void Activate(ID3D11DeviceContext* context, UINT shot, bool uav = false, bool srv = false)
	{
		if (uav)
		{
			context->CSGetUnorderedAccessViews(shot, 1, mDefUAV.GetAddressOf());
			context->CSSetUnorderedAccessViews(shot, 1, mUAV.GetAddressOf(), nullptr);
		}
		if (srv)
		{
			context->CSGetShaderResources(shot, 1, mDefSRV.GetAddressOf());
			context->CSSetShaderResources(shot, 1, mSRV.GetAddressOf());
		}
		mUavFlag = uav;
		mSrvFlag = srv;
		mShot = shot;
	}
	//GPU側にセットしたのを元に戻す
	void DeActivate(ID3D11DeviceContext* context)
	{
		if (mUavFlag)context->CSSetUnorderedAccessViews(mShot, 1, mDefUAV.GetAddressOf(), nullptr);
		if (mSrvFlag)context->CSSetShaderResources(mShot, 1, mDefSRV.GetAddressOf());
	}
	//getter
	ID3D11Buffer* GetBuffer() { return mBuffer.Get(); }

private:
	//UAVやSRVの生成
	void CreateView(ID3D11Device* device, UINT miscFlags, UINT classSize, UINT elementCount, bool uav, bool srv);
	//バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mBuffer;
	//UAV
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mUAV;
	//SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	//GPUに設定したかどうか
	bool mUavFlag;
	bool mSrvFlag;
	//設定する時のNo
	UINT mShot;
	//元のデータ
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mDefUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mDefSRV;

};

