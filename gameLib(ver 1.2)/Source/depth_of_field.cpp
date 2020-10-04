#include "depth_of_field.h"
#include"shader.h"
#include"misc.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

DepthOfField::DepthOfField(ID3D11Device* device):averageFlag(false)
{
	HRESULT hr;
	hr = create_cs_from_cso(device, "Data/shader/depth_conversion_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//desc.Width = 1920;
		//desc.Height = 1080;
		//desc.MipLevels = 0;
		//desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		//desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		//desc.CPUAccessFlags = 0;
		desc.Width = 1920;
		desc.Height = 1080;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = device->CreateTexture2D(&desc, nullptr, mTexture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;
		hr = device->CreateUnorderedAccessView(mTexture2d.Get(), &desc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(mTexture2d.Get(), &desc, mSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(Cb);
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbData.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	mCb.inFocusMin = 0.25f;
	mCb.inFocusMax = 0.75f;
	Load();
}

void DepthOfField::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("depth of field");
	if (ImGui::Button("save"))Save();
	if (ImGui::Button("load"))Load();
	ImGui::Checkbox("average focus", &averageFlag);
	ImGui::SliderFloat("min in focus", &mCb.inFocusMin, 0,0.5f);
	if (averageFlag)
	{
		mCb.inFocusMax = 1 - mCb.inFocusMin;
	}
	else
	{
		ImGui::SliderFloat("max in focus", &mCb.inFocusMax, 0.5f,1);
	}
	ImGui::End();
#endif
}

void DepthOfField::ConversionDepthTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv)
{
	context->CSSetConstantBuffers(0, 1, mCbData.GetAddressOf());
	context->UpdateSubresource(mCbData.Get(), 0, 0, &mCb, 0, 0);
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	context->CSSetShaderResources(0, 1, &srv);
	context->CSSetUnorderedAccessViews(0, 1, mUAV.GetAddressOf(), 0);

	context->Dispatch(192, 108, 1);

	context->CSSetShader(nullptr, nullptr, 0);
	ID3D11ShaderResourceView* s = nullptr;
	context->CSSetShaderResources(0, 1, &s);
	ID3D11UnorderedAccessView* u = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &u, 0);

}

void DepthOfField::SetPsTexture(ID3D11DeviceContext* context, const int number)
{
}

void DepthOfField::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/depth_of_field.bin", "rb") == 0)
	{
		fread(&mCb, sizeof(Cb), 1, fp);
		fclose(fp);
	}
}

void DepthOfField::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/depth_of_field.bin", "wb");
	fwrite(&mCb, sizeof(Cb), 1, fp);
	fclose(fp);

}
