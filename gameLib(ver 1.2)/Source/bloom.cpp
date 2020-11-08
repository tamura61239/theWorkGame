#include "bloom.h"
#include"shader.h"
#include"misc.h"
#include"vector.h"
#include<string>
#ifdef USE_IMGUI
#include<imgui.h>
#endif

BloomRender::BloomRender(ID3D11Device* device, float screenWidth, float screenHight, const int nowScene) :mNowEditorNo(0), mNowScene(nowScene)
{
	memset(&mEditorData, 0, sizeof(mEditorData));
	unsigned int wight = static_cast<unsigned int>(screenWidth);
	unsigned int hight = static_cast<unsigned int>(screenHight);
	for (int i = 0; i < 5; i++)
	{
		mFrameBuffer.emplace_back(std::make_unique<FrameBuffer>(device, static_cast<int>(wight >> i), static_cast<float>(hight >> i), true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS));
	}
	for (int i = 1; i < 5; i++)
	{
		mSidoFrameBuffer.emplace_back(std::make_unique<FrameBuffer>(device, static_cast<int>(wight >> i), static_cast<float>(hight >> i), true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS));
	}
	mFrameBuffer.emplace_back(std::make_unique<FrameBuffer>(device, screenWidth, screenHight, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS));

	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout>input;

	hr = create_vs_from_cso(device, "Data/shader/bloom_vs.cso", mVSShader.GetAddressOf(), input.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "Data/shader/bloomStart_ps.cso", mPSShader[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "Data/shader/combined_bloom.cso", mPSShader[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "Data/shader/bloom_blur01_ps.cso", mPSBlurShader[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "Data/shader/bloom_blur02_ps.cso", mPSBlurShader[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//SamplerState‚Ì¶¬
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 16;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		desc.MaxAnisotropy = 16;
		memcpy(desc.BorderColor, &VECTOR4F(.0f, .0f, .0f, .0f), sizeof(VECTOR4F));
		hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//DepthStencilState‚Ì¶¬
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = false;
		desc.StencilEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.AntialiasedLineEnable = true;


		hr = device->CreateRasterizerState(&rasterizerDesc, mRasterizeState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		CD3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbBloom);
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCBbuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbBluer);
		hr = device->CreateBuffer(&desc, nullptr, mCbBluerbuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	for (int i = 0; i < 4; i++)
	{
		Load(i);
	}
}

void BloomRender::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("bloom");
	for (int i = 1; i < 5; i++)
	{
		D3D11_VIEWPORT viewport = mFrameBuffer[i]->GetViewPort();
		ImVec2 size = ImVec2(192, 108);

		ImGui::Image(mFrameBuffer[i]->GetRenderTargetShaderResourceView().Get(), size);
		if (i < 4)ImGui::SameLine();
	}
	auto& editorData = mEditorData[mNowScene];

	ImGui::RadioButton("title bloom", &mNowEditorNo, 0); ImGui::SameLine();
	ImGui::RadioButton("select bloom", &mNowEditorNo, 1); ImGui::SameLine();
	ImGui::RadioButton("game bloom", &mNowEditorNo, 2); ImGui::SameLine();
	ImGui::RadioButton("result bloom", &mNowEditorNo, 3);
	ImGui::RadioButton("blur 01", &editorData.mBlurType, 0); ImGui::SameLine();
	ImGui::RadioButton("blur 02", &editorData.mBlurType, 1);
	ImGui::SliderInt("filter count", &editorData.count, 0, 4);
	ImGui::SliderFloat("threshold", &editorData.threshold, 0, 10);
	ImGui::SliderFloat("widthBlur", &editorData.widthBlur, 0, 1);
	ImGui::SliderFloat("hightBlur", &editorData.hightBlur, 0, 1);
	ImGui::SliderFloat("devutation", &editorData.deviation, 0, 50);
	ImGui::SliderFloat("multiply", &editorData.multiply, 0, 10);
	int blurCount = static_cast<int>(editorData.blurCount);
	if (ImGui::SliderInt("blurCount", &blurCount, 1, 16))
	{
		editorData.blurCount = static_cast<float>(blurCount);
	}

	if (ImGui::Button("save"))Save(mNowEditorNo);
	if (ImGui::Button("all save"))
	{
		for (int i = 0; i < 4; i++)
		{
			Save(i);
		}
	}
	ImGui::End();
#endif
}

void BloomRender::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv, bool render)
{

	auto& editorData = mEditorData[mNowScene];
	ID3D11Buffer* buffer[] =
	{
		mCBbuffer.Get(),
		mCbBluerbuffer.Get()
	};
	context->PSSetConstantBuffers(0, 2, buffer);
	context->VSSetConstantBuffers(0, 2, buffer);
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	mFrameBuffer[0]->Clear(context);
	if (editorData.count < 0)return;

	mFrameBuffer[0]->Activate(context);
	CbBloom cbBloom;
	cbBloom.blurCount = editorData.blurCount;
	cbBloom.hightBlur = editorData.hightBlur;
	cbBloom.threshold = editorData.threshold;
	cbBloom.widthBlur = editorData.widthBlur;
	context->UpdateSubresource(mCBbuffer.Get(), 0, 0, &cbBloom, 0, 0);
	context->VSSetShader(mVSShader.Get(), 0, 0);
	context->PSSetShader(mPSShader[0].Get(), 0, 0);
	context->PSSetShaderResources(0, 1, &colorSrv);
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizeState.Get());
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);
	mFrameBuffer[0]->Deactivate(context);

	//Ý’è
	context->VSSetShader(mVSShader.Get(), 0, 0);
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizeState.Get());
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	switch (editorData.mBlurType)
	{
	case 0:
		Blur01(context);
		break;
	case 1:
		Blur02(context);
		break;
	}
	if (!render)
	{
		mFrameBuffer[5]->Clear(context);
		mFrameBuffer[5]->Activate(context);
		context->VSSetShader(mVSShader.Get(), 0, 0);
		context->PSSetShader(mPSShader[1].Get(), 0, 0);
		for (int i = 0; i < 5; i++)
		{
			context->PSSetShaderResources(i, 1, mFrameBuffer[i]->GetRenderTargetShaderResourceView().GetAddressOf());
		}
		context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
		context->RSSetState(mRasterizeState.Get());
		context->IASetInputLayout(nullptr);
		context->IASetVertexBuffers(0, 0, 0, 0, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		context->Draw(4, 0);
		mFrameBuffer[5]->Deactivate(context);
		return;
	}
	context->VSSetShader(mVSShader.Get(), 0, 0);
	context->PSSetShader(mPSShader[1].Get(), 0, 0);
	context->PSSetShaderResources(0, 1, &colorSrv);
	for (int i = 0; i < 5; i++)
	{
		context->PSSetShaderResources(i + 1, 1, mFrameBuffer[i]->GetRenderTargetShaderResourceView().GetAddressOf());
	}
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizeState.Get());
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);

}

void BloomRender::Load(const int scene)
{
	FILE* fp;
	std::string fileName = "Data/file/bloom" + std::to_string(scene) + ".bin";
	if (fopen_s(&fp, fileName.c_str(), "rb") == 0)
	{
		fread(&mEditorData[scene], sizeof(EditorData), 1, fp);
		fclose(fp);
	}
}

void BloomRender::Save(const int scene)
{
	FILE* fp;
	std::string fileName = "Data/file/bloom" + std::to_string(scene) + ".bin";
	fopen_s(&fp, fileName.c_str(), "wb");
	fwrite(&mEditorData[scene], sizeof(EditorData), 1, fp);
	fclose(fp);

}

void BloomRender::Blur01(ID3D11DeviceContext* context)
{
	auto& editorData = mEditorData[mNowScene];

	context->PSSetShader(mPSBlurShader[0].Get(), 0, 0);

	for (int i = 1; i < 5; i++)
	{
		if (editorData.count < i)
		{
			mFrameBuffer[i]->Clear(context);
			continue;
		}

		mFrameBuffer[i]->Clear(context);
		mFrameBuffer[i]->Activate(context);
		D3D11_VIEWPORT viewport = mFrameBuffer[i]->GetViewPort();
		CalucurateBluer(viewport.Width, viewport.Height, VECTOR2F(editorData.widthBlur, editorData.hightBlur), editorData.deviation, editorData.multiply);
		context->UpdateSubresource(mCbBluerbuffer.Get(), 0, 0, &mCbBluer, 0, 0);

		context->PSSetShaderResources(0, 1, mFrameBuffer[i - 1]->GetRenderTargetShaderResourceView().GetAddressOf());
		context->Draw(4, 0);
		mFrameBuffer[i]->Deactivate(context);
	}

}

void BloomRender::Blur02(ID3D11DeviceContext* context)
{
	auto& editorData = mEditorData[mNowScene];

	context->PSSetShader(mPSBlurShader[1].Get(), 0, 0);

	for (int i = 1; i < 5; i++)
	{
		if (editorData.count < i)
		{
			mFrameBuffer[i]->Clear(context);
			continue;
		}

		mSidoFrameBuffer[i - 1]->Clear(context);
		mSidoFrameBuffer[i - 1]->Activate(context);

		D3D11_VIEWPORT viewport = mSidoFrameBuffer[i - 1]->GetViewPort();
		CalucurateBluer(viewport.Width, viewport.Height, VECTOR2F(editorData.widthBlur, 0), editorData.deviation, editorData.multiply);
		context->UpdateSubresource(mCbBluerbuffer.Get(), 0, 0, &mCbBluer, 0, 0);

		context->PSSetShaderResources(0, 1, mFrameBuffer[i - 1]->GetRenderTargetShaderResourceView().GetAddressOf());
		context->Draw(4, 0);

		mSidoFrameBuffer[i - 1]->Deactivate(context);
		mFrameBuffer[i]->Clear(context);
		mFrameBuffer[i]->Activate(context);
		viewport = mFrameBuffer[i]->GetViewPort();
		CalucurateBluer(viewport.Width, viewport.Height, VECTOR2F(0, editorData.hightBlur), editorData.deviation, editorData.multiply);
		context->UpdateSubresource(mCbBluerbuffer.Get(), 0, 0, &mCbBluer, 0, 0);

		context->PSSetShaderResources(0, 1, mSidoFrameBuffer[i - 1]->GetRenderTargetShaderResourceView().GetAddressOf());
		context->Draw(4, 0);
		mFrameBuffer[i]->Deactivate(context);
	}

}

float BloomRender::GaussianDistribution(const VECTOR2F& position, const float rho)
{
	return static_cast<float>(exp(-(position.x * position.x + position.y * position.y) / (2.0f * rho * rho)));
}

void BloomRender::CalucurateBluer(const float width, const float hight, const VECTOR2F& dir, const float deviation, const float multiply)
{
	float uvX = 1.0f / width;
	float uvY = 1.0f / hight;

	mCbBluer.mOffset[0].z = GaussianDistribution(VECTOR2F(0, 0), deviation) * multiply;
	float totalWeigh = mCbBluer.mOffset[0].z;

	mCbBluer.mOffset[0].x = 0;
	mCbBluer.mOffset[0].y = 0;
	for (int i = 1; i < 8; i++)
	{
		mCbBluer.mOffset[i].x = dir.x * i * uvX;
		mCbBluer.mOffset[i].y = dir.y * i * uvY;
		mCbBluer.mOffset[i].z = GaussianDistribution(dir * float(i), deviation) * multiply;
		totalWeigh += mCbBluer.mOffset[i].z * 2.0f;
	}
	for (int i = 0; i < 8; i++)
	{
		mCbBluer.mOffset[i].z /= totalWeigh;
	}
	for (auto i = 8; i < 15; ++i)
	{
		mCbBluer.mOffset[i].x = -mCbBluer.mOffset[i - 7].x;
		mCbBluer.mOffset[i].y = -mCbBluer.mOffset[i - 7].y;
		mCbBluer.mOffset[i].z = mCbBluer.mOffset[i - 7].z;
	}

}
