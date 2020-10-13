#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"drow_shader.h"
#include<memory>
#include<vector>
#include<string>

class TitleTextureParticle
{
public:
	TitleTextureParticle(ID3D11Device* device);
	void LoadTextureData(const wchar_t* textureName, VECTOR2F leftTop, VECTOR2F size, VECTOR2F textureSize);
	void CreateBuffer(ID3D11Device* device);
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
	void SetStartFlag(const bool flag) { mStartFlag = flag; }
private:
	struct ParticleData
	{
		VECTOR3F position;
		VECTOR3F velocity;
		float speed;
		VECTOR4F color;
		float life;
	};
	struct CreateData
	{
		VECTOR2F leftTop;
		VECTOR2F size;
		VECTOR2F textureSize;
	};
	struct SetData
	{
		CreateData data;
		std::wstring texName;
	};
	struct CbCreate
	{
		FLOAT4X4 inverseViewProjection;
		VECTOR2F textureSize;
		float z;
		float startIndex;
		VECTOR2F leftTop;
		VECTOR2F viewport;
	};
	struct Cb
	{
		float elapsdTime;
		VECTOR3F endPosition;
	};
	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};

	std::vector<SetData>mSet;
	std::vector<int>particleCount;
	int mMaxCount;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mSRVs;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreateBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>buffer;

	bool mCreateFlag;
	bool mStartFlag;
	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreateShader;
	float mZPos;
};
