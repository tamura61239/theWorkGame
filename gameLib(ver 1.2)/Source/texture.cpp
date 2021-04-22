#include "texture.h"
#include "misc.h"
#include<DirectXTex.h>
#include <wrl.h>
#include <Shlwapi.h>

#include <map>
#include<string>
/*****************************************************/
//　　　　　　　　　　テクスチャを生成する関数
/*****************************************************/
/****************************テクスチャを読み込む*************************/

HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc, bool mise)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	//std::mapにデータを保持する
	static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;
	auto it = cache.find(file_name);
	if (it != cache.end())
	{
		//データを保持していたら取得する
		*shader_resource_view = it->second.Get();
		(*shader_resource_view)->AddRef();
	}
	else
	{
		//テクスチャを読み込む
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;
		std::wstring extension = PathFindExtensionW(file_name);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
		if (L".png" == extension || L".jpeg" == extension || L".jpg" == extension || L".jpe" == extension || L".gif" == extension || L".tiff" == extension || L".tif" == extension || L".bmp" == extension)
		{
			hr = DirectX::LoadFromWICFile(file_name, 0, &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else if (L".dds" == extension)
		{
			hr = DirectX::LoadFromDDSFile(file_name, 0, &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else if (L".tga" == extension || L".vda" == extension || L".icb" == extension || L".vst" == extension)
		{
			hr = DirectX::LoadFromTGAFile(file_name, &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			_ASSERT_EXPR(false, L"File format not supported.");
			return E_FAIL;
		}
		hr = DirectX::CreateShaderResourceViewEx(
			device,
			image.GetImages(),
			image.GetImageCount(),
			metadata,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			D3D11_RESOURCE_MISC_TEXTURECUBE,
			mise,
			shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//セーブする
		cache.insert(std::make_pair(file_name, *shader_resource_view));
	}
	//テクスチャのデータを取得する
	if (texture2d_desc == nullptr)return hr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	(*shader_resource_view)->GetResource(resource.GetAddressOf());
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}
/*************************ダミーテクスチャを生成する****************************/

HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view)
{
	HRESULT hr = S_OK;
	//テクスチャを生成するためのバッファの生成
	D3D11_TEXTURE2D_DESC texture2d_desc = {};
	texture2d_desc.Width = 1;
	texture2d_desc.Height = 1;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA subresource_data = {};
	u_int color = 0xFFFFFFFF;
	subresource_data.pSysMem = &color;
	subresource_data.SysMemPitch = 4;
	subresource_data.SysMemSlicePitch = 4;

	ID3D11Texture2D* texture2d;
	hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//テクスチャのSRVを生成
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	shader_resource_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(texture2d, &shader_resource_view_desc, shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	texture2d->Release();

	return hr;
}

/*
e.g.
referrer_filename <= L"data/bison.obj"
referent_filename <= L"/user/textures/bison.png"
combined_resource_path => L"/data/bison.png"
*/
void CombineResourcePath(wchar_t(&combined_resource_path)[256], const wchar_t* referrer_filename, const wchar_t* referent_filename)
{
	const wchar_t delimiters[] = { L'\\', L'/' };
	// extract directory from obj_filename
	wchar_t directory[256] = {};
	for (wchar_t delimiter : delimiters)
	{
		wchar_t* p = wcsrchr(const_cast<wchar_t*>(referrer_filename), delimiter);
		if (p)
		{
			memcpy_s(directory, 256, referrer_filename, (p - referrer_filename + 1) * sizeof(wchar_t));
			break;
		}
	}
	// extract filename from resource_filename
	wchar_t filename[256];
	wcscpy_s(filename, referent_filename);
	for (wchar_t delimiter : delimiters)
	{
		wchar_t* p = wcsrchr(filename, delimiter);
		if (p)
		{
			wcscpy_s(filename, p + 1);
			break;
		}
	}
	// combine directory and filename
	wcscpy_s(combined_resource_path, directory);
	wcscat_s(combined_resource_path, filename);
};
