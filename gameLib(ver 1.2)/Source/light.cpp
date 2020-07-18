#include "light.h"
#include "misc.h"

Light::Light()
{
	ZeroMemory(pointLight, sizeof(PointLight) * POINTMAX);
	ZeroMemory(spotLight, sizeof(SpotLight) * SPOTMAX);
	mLightDirection = VECTOR4F(1, 1, 1, 0);
	mLightColor = VECTOR4F(1, 1, 1, 1);
	mAmbientColor = VECTOR4F(1, 1, 1, 1);
}

void Light::CreateLightBuffer(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	// create constant buffer
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(CbLight);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbLight.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
}



void Light::SetPointLight(int index, VECTOR3F position, VECTOR3F color, float range)
{
	if (index < 0) return;
	if (index >= POINTMAX)return;
	pointLight[index].index = (float)index;
	pointLight[index].range = range;
	pointLight[index].type = 1.0f;
	pointLight[index].dumy = 0.0f;
	pointLight[index].position = VECTOR4F(position.x, position.y, position.z, 0);
	pointLight[index].color = VECTOR4F(color.x, color.y, color.z, 0);

}

void Light::SetSpotLight(int index, VECTOR3F position, VECTOR3F color, VECTOR3F dir, float range, float nearArea, float farArea)
{
	if (index < 0) return;
	if (index >= SPOTMAX)return;

	spotLight[index].index = (float)index;
	spotLight[index].range = range;
	spotLight[index].type = 1.0f;
	spotLight[index].nearArea = nearArea;
	spotLight[index].farArea = farArea;
	spotLight[index].dumy0 = 0.0f;
	spotLight[index].dumy1 = 0.0f;
	spotLight[index].dumy2 = 0.0f;

	spotLight[index].position = VECTOR4F(position.x, position.y, position.z, 0);
	spotLight[index].color = VECTOR4F(color.x, color.y, color.z, 0);
	spotLight[index].dir = VECTOR4F(dir.x, dir.y, dir.z, 0);

}

void Light::ConstanceLightBufferSetShader(ID3D11DeviceContext* context)
{
	context->PSSetConstantBuffers(3, 1, mCbLight.GetAddressOf());
	CbLight cbLight;
	memcpy(cbLight.pointLight, pointLight, sizeof(PointLight) * POINTMAX);
	memcpy(cbLight.spotLight, spotLight, sizeof(SpotLight) * SPOTMAX);
	context->UpdateSubresource(mCbLight.Get(), 0, 0, &cbLight, 0, 0);

}

